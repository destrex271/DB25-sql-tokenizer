/*
 * Copyright (c) 2024 Chiradip Mandal
 * Author: Chiradip Mandal
 * Organization: Space-RF.org
 *
 * This file is part of DB25 SQL Tokenizer.
 *
 * Licensed under the MIT License. See LICENSE file for details.
 */

#pragma once

#include <cstdint>

namespace db25 {

// Character class flags (can be combined with bitwise OR)
enum CharClass : uint8_t {
    CHAR_NONE        = 0x00,
    CHAR_WHITESPACE  = 0x01,  // ' ', '\t', '\n', '\r'
    CHAR_ALPHA_UPPER = 0x02,  // A-Z
    CHAR_ALPHA_LOWER = 0x04,  // a-z
    CHAR_DIGIT       = 0x08,  // 0-9
    CHAR_UNDERSCORE  = 0x10,  // _
    CHAR_QUOTE       = 0x20,  // ' "
    CHAR_OPERATOR    = 0x40,  // + - * / = < > ! & | ^ ~ %
    CHAR_DELIMITER   = 0x80,  // ( ) [ ] { } , ; :

    // Composite classes
    CHAR_ALPHA       = CHAR_ALPHA_UPPER | CHAR_ALPHA_LOWER,
    CHAR_IDENT_START = CHAR_ALPHA | CHAR_UNDERSCORE,
    CHAR_IDENT_CONT  = CHAR_IDENT_START | CHAR_DIGIT
};

// Lookup table for character classification (cache-line aligned)
alignas(64) inline constexpr uint8_t char_lookup_table[256] = {
    // 0x00 - 0x1F: Control characters
    0, 0, 0, 0, 0, 0, 0, 0, 0, CHAR_WHITESPACE, CHAR_WHITESPACE, 0, 0, CHAR_WHITESPACE, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    // 0x20 - 0x3F: Space, punctuation, digits
    CHAR_WHITESPACE,  // ' '
    CHAR_OPERATOR,    // !
    CHAR_QUOTE,       // "
    0,                // #
    0,                // $
    CHAR_OPERATOR,    // %
    CHAR_OPERATOR,    // &
    CHAR_QUOTE,       // '
    CHAR_DELIMITER,   // (
    CHAR_DELIMITER,   // )
    CHAR_OPERATOR,    // *
    CHAR_OPERATOR,    // +
    CHAR_DELIMITER,   // ,
    CHAR_OPERATOR,    // -
    CHAR_OPERATOR,    // .
    CHAR_OPERATOR,    // /

    // 0x30 - 0x39: Digits
    CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT,
    CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT,

    // 0x3A - 0x3F
    CHAR_DELIMITER,   // :
    CHAR_DELIMITER,   // ;
    CHAR_OPERATOR,    // <
    CHAR_OPERATOR,    // =
    CHAR_OPERATOR,    // >
    0,                // ?

    // 0x40 - 0x5A: @ and uppercase letters
    0,                // @
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER,

    // 0x5B - 0x60
    CHAR_DELIMITER,   // [
    0,                // backslash
    CHAR_DELIMITER,   // ]
    CHAR_OPERATOR,    // ^
    CHAR_UNDERSCORE,  // _
    0,                // `

    // 0x61 - 0x7A: Lowercase letters
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER,

    // 0x7B - 0x7F
    CHAR_DELIMITER,   // {
    CHAR_OPERATOR,    // |
    CHAR_DELIMITER,   // }
    CHAR_OPERATOR,    // ~
    0,                // DEL

    // 0x80 - 0xFF: Extended ASCII (not used in SQL)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Inline helper functions using the lookup table
inline bool is_identifier_start(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_IDENT_START) != 0;
}

inline bool is_identifier_cont(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_IDENT_CONT) != 0;
}

inline bool is_digit(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_DIGIT) != 0;
}

inline bool is_whitespace(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_WHITESPACE) != 0;
}

inline bool is_operator(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_OPERATOR) != 0;
}

inline bool is_delimiter(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_DELIMITER) != 0;
}

inline bool is_quote(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_QUOTE) != 0;
}

}  // namespace db25