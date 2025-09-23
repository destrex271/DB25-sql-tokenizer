/*
 * Copyright (c) 2024 Chiradip Mandal
 * Author: Chiradip Mandal
 * Organization: Space-RF.org
 * 
 * This file is part of DB25 SQL Tokenizer.
 * 
 * Licensed under the MIT License. See LICENSE file for details.
 */

#include "simd_tokenizer.hpp"
#include "char_classifier.hpp"

namespace db25 {

SimdTokenizer::SimdTokenizer(const std::byte* input, size_t size)
        : input_(input)
        , input_size_(size)
        , position_(0)
        , line_(1)
        , column_(1) {}
    
[[nodiscard]] std::vector<Token> SimdTokenizer::tokenize() {
        std::vector<Token> tokens;
        tokens.reserve(input_size_ / 8);
        
        while (position_ < input_size_) {
            size_t skip = dispatcher_.dispatch([this](auto processor) {
                return processor.skip_whitespace(
                    input_ + position_, 
                    input_size_ - position_
                );
            });
            
            if (skip > 0) {
                update_position(skip);
            }
            
            if (position_ >= input_size_) {
                break;
            }
            
            Token token = next_token();
            if (token.type != TokenType::Whitespace) {
                tokens.push_back(token);
            }
            
            if (token.type == TokenType::EndOfFile) {
                break;
            }
        }
        
        return tokens;
    }
    
[[nodiscard]] const char* SimdTokenizer::simd_level() const noexcept {
    return dispatcher_.level_name();
}

Token SimdTokenizer::next_token() {
        if (position_ >= input_size_) {
            return {TokenType::EndOfFile, "", Keyword::UNKNOWN, line_, column_};
        }
        
        size_t start = position_;
        size_t start_line = line_;
        size_t start_column = column_;
        
        uint8_t first_char = static_cast<uint8_t>(input_[position_]);

        // Use lookup table for character classification
        if (is_identifier_start(first_char)) {
            return scan_identifier_or_keyword(start, start_line, start_column);
        }

        if (is_digit(first_char)) {
            return scan_number(start, start_line, start_column);
        }

        if (is_quote(first_char)) {
            return scan_string(start, start_line, start_column, first_char);
        }
        
        if (first_char == '-' && position_ + 1 < input_size_ &&
            static_cast<uint8_t>(input_[position_ + 1]) == '-') {
            return scan_comment(start, start_line, start_column);
        }
        
        if (first_char == '/' && position_ + 1 < input_size_ &&
            static_cast<uint8_t>(input_[position_ + 1]) == '*') {
            return scan_block_comment(start, start_line, start_column);
        }
        
        return scan_operator_or_delimiter(start, start_line, start_column);
    }

Token SimdTokenizer::scan_identifier_or_keyword(size_t start, size_t start_line, size_t start_column) {
        while (position_ < input_size_) {
            uint8_t ch = static_cast<uint8_t>(input_[position_]);
            if (!is_identifier_cont(ch)) {
                break;
            }
            ++position_;
            ++column_;
        }
        
        std::string_view value(
            reinterpret_cast<const char*>(input_ + start),
            position_ - start
        );
        
        // Use generated keyword lookup
        Keyword kw = find_keyword(value);
        TokenType type = (kw != Keyword::UNKNOWN) ? TokenType::Keyword : TokenType::Identifier;
        
        // For even faster SIMD-based keyword matching (optional optimization)
        if (kw == Keyword::UNKNOWN && value.length() <= 32) {
            dispatcher_.dispatch([&](auto processor) {
                return is_keyword_simd(processor, 
                    input_ + start, 
                    value.length(), 
                    kw);
            });
            
            if (kw != Keyword::UNKNOWN) {
                type = TokenType::Keyword;
            }
        }
        
        return {type, value, kw, start_line, start_column};
    }

Token SimdTokenizer::scan_number(size_t start, size_t start_line, size_t start_column) {
        bool has_dot = false;
        bool has_exp = false;

        while (position_ < input_size_) {
            uint8_t ch = static_cast<uint8_t>(input_[position_]);

            if (is_digit(ch)) {
                ++position_;
                ++column_;
            } else if (ch == '.' && !has_dot && !has_exp) {
                has_dot = true;
                ++position_;
                ++column_;
            } else if ((ch == 'e' || ch == 'E') && !has_exp) {
                has_exp = true;
                ++position_;
                ++column_;
                
                if (position_ < input_size_) {
                    ch = static_cast<uint8_t>(input_[position_]);
                    if (ch == '+' || ch == '-') {
                        ++position_;
                        ++column_;
                    }
                }
            } else {
                break;
            }
        }
        
        std::string_view value(
            reinterpret_cast<const char*>(input_ + start),
            position_ - start
        );
        
        return {TokenType::Number, value, Keyword::UNKNOWN, start_line, start_column};
    }

Token SimdTokenizer::scan_string(size_t start, size_t start_line, size_t start_column, uint8_t quote) {
        ++position_;
        ++column_;
        
        while (position_ < input_size_) {
            uint8_t ch = static_cast<uint8_t>(input_[position_]);
            
            if (ch == quote) {
                if (position_ + 1 < input_size_ &&
                    static_cast<uint8_t>(input_[position_ + 1]) == quote) {
                    position_ += 2;
                    column_ += 2;
                } else {
                    ++position_;
                    ++column_;
                    break;
                }
            } else if (ch == '\n') {
                ++position_;
                ++line_;
                column_ = 1;
            } else {
                ++position_;
                ++column_;
            }
        }
        
        std::string_view value(
            reinterpret_cast<const char*>(input_ + start),
            position_ - start
        );
        
        return {TokenType::String, value, Keyword::UNKNOWN, start_line, start_column};
    }

Token SimdTokenizer::scan_comment(size_t start, size_t start_line, size_t start_column) {
        position_ += 2;
        column_ += 2;
        
        while (position_ < input_size_) {
            if (static_cast<uint8_t>(input_[position_]) == '\n') {
                ++position_;
                ++line_;
                column_ = 1;
                break;
            }
            ++position_;
            ++column_;
        }
        
        std::string_view value(
            reinterpret_cast<const char*>(input_ + start),
            position_ - start
        );
        
        return {TokenType::Comment, value, Keyword::UNKNOWN, start_line, start_column};
    }

Token SimdTokenizer::scan_block_comment(size_t start, size_t start_line, size_t start_column) {
        position_ += 2;
        column_ += 2;
        
        while (position_ + 1 < input_size_) {
            if (static_cast<uint8_t>(input_[position_]) == '*' &&
                static_cast<uint8_t>(input_[position_ + 1]) == '/') {
                position_ += 2;
                column_ += 2;
                break;
            } else if (static_cast<uint8_t>(input_[position_]) == '\n') {
                ++position_;
                ++line_;
                column_ = 1;
            } else {
                ++position_;
                ++column_;
            }
        }
        
        std::string_view value(
            reinterpret_cast<const char*>(input_ + start),
            position_ - start
        );
        
        return {TokenType::Comment, value, Keyword::UNKNOWN, start_line, start_column};
    }

Token SimdTokenizer::scan_operator_or_delimiter(size_t start, size_t start_line, size_t start_column) {
        uint8_t ch = static_cast<uint8_t>(input_[position_]);
        ++position_;
        ++column_;

        // Use lookup table to determine token type
        TokenType type = is_delimiter(ch) ? TokenType::Delimiter : TokenType::Operator;
        
        if (position_ < input_size_) {
            uint8_t next = static_cast<uint8_t>(input_[position_]);
            
            if ((ch == '<' && (next == '=' || next == '>')) ||
                (ch == '>' && next == '=') ||
                (ch == '!' && next == '=') ||
                (ch == '=' && next == '=') ||  // Add support for ==
                (ch == '|' && next == '|') ||
                (ch == '&' && next == '&') ||
                (ch == ':' && next == ':') ||
                (ch == '<' && next == '<') ||
                (ch == '>' && next == '>')) {
                ++position_;
                ++column_;
            }
        }
        
        std::string_view value(
            reinterpret_cast<const char*>(input_ + start),
            position_ - start
        );
        
        return {type, value, Keyword::UNKNOWN, start_line, start_column};
    }

void SimdTokenizer::update_position(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            uint8_t ch = static_cast<uint8_t>(input_[position_]);
            if (ch == '\n') {
                ++line_;
                column_ = 1;
            } else {
                ++column_;
            }
            ++position_;
        }
}

}  // namespace db25