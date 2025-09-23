/*
 * Copyright (c) 2024 Chiradip Mandal
 * Author: Chiradip Mandal
 * Organization: Space-RF.org
 *
 * Lookup Table Analysis for DB25 SQL Tokenizer
 * Comparing character classification approaches
 */

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <cstring>

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

// Lookup table for character classification
alignas(64) static constexpr uint8_t char_lookup_table[256] = {
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

// Current implementation (range checks)
inline bool is_identifier_start_range(uint8_t ch) {
    return (ch >= 'A' && ch <= 'Z') ||
           (ch >= 'a' && ch <= 'z') ||
           ch == '_';
}

inline bool is_identifier_cont_range(uint8_t ch) {
    return (ch >= 'A' && ch <= 'Z') ||
           (ch >= 'a' && ch <= 'z') ||
           (ch >= '0' && ch <= '9') ||
           ch == '_';
}

inline bool is_digit_range(uint8_t ch) {
    return ch >= '0' && ch <= '9';
}

inline bool is_whitespace_range(uint8_t ch) {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

// Lookup table implementation
inline bool is_identifier_start_lookup(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_IDENT_START) != 0;
}

inline bool is_identifier_cont_lookup(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_IDENT_CONT) != 0;
}

inline bool is_digit_lookup(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_DIGIT) != 0;
}

inline bool is_whitespace_lookup(uint8_t ch) {
    return (char_lookup_table[ch] & CHAR_WHITESPACE) != 0;
}

// Benchmark function
template<typename Func>
double benchmark(const std::string& name, const std::vector<uint8_t>& data, Func func) {
    const int iterations = 10000;  // Reduced for faster execution

    auto start = std::chrono::high_resolution_clock::now();

    int count = 0;
    for (int i = 0; i < iterations; ++i) {
        int local_count = 0;
        for (uint8_t ch : data) {
            if (func(ch)) {
                local_count++;
            }
        }
        count = local_count;  // Prevent optimization
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    double ns_per_char = static_cast<double>(duration.count()) / (iterations * data.size());

    std::cout << std::setw(30) << name << ": "
              << std::fixed << std::setprecision(3)
              << ns_per_char << " ns/char"
              << " (matches: " << count << ")" << std::endl;

    return ns_per_char;
}

int main() {
    std::cout << "DB25 SQL Tokenizer - Lookup Table Analysis\n";
    std::cout << "==========================================\n\n";

    // Generate test data with realistic SQL character distribution
    std::vector<uint8_t> test_data;
    std::mt19937 gen(42);
    std::discrete_distribution<> dist({
        30,  // Letters
        15,  // Digits
        25,  // Whitespace
        10,  // Operators
        10,  // Delimiters
        10   // Other
    });

    const char* chars[] = {
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_",
        "0123456789",
        " \t\n\r",
        "+-*/<>=!&|^~%",
        "()[]{}.,;:",
        "#$@`\\"
    };

    for (int i = 0; i < 10000; ++i) {
        int category = dist(gen);
        const char* category_chars = chars[category];
        size_t len = strlen(category_chars);
        std::uniform_int_distribution<> char_dist(0, len - 1);
        test_data.push_back(category_chars[char_dist(gen)]);
    }

    std::cout << "Test data size: " << test_data.size() << " characters\n\n";

    // Benchmark identifier start checks
    std::cout << "Identifier Start Classification:\n";
    std::cout << "---------------------------------\n";
    double range_time = benchmark("Range checks", test_data, is_identifier_start_range);
    double lookup_time = benchmark("Lookup table", test_data, is_identifier_start_lookup);
    std::cout << "Speedup: " << std::fixed << std::setprecision(2)
              << (range_time / lookup_time) << "x\n\n";

    // Benchmark identifier continuation checks
    std::cout << "Identifier Continuation Classification:\n";
    std::cout << "----------------------------------------\n";
    range_time = benchmark("Range checks", test_data, is_identifier_cont_range);
    lookup_time = benchmark("Lookup table", test_data, is_identifier_cont_lookup);
    std::cout << "Speedup: " << std::fixed << std::setprecision(2)
              << (range_time / lookup_time) << "x\n\n";

    // Benchmark digit checks
    std::cout << "Digit Classification:\n";
    std::cout << "---------------------\n";
    range_time = benchmark("Range checks", test_data, is_digit_range);
    lookup_time = benchmark("Lookup table", test_data, is_digit_lookup);
    std::cout << "Speedup: " << std::fixed << std::setprecision(2)
              << (range_time / lookup_time) << "x\n\n";

    // Benchmark whitespace checks
    std::cout << "Whitespace Classification:\n";
    std::cout << "--------------------------\n";
    range_time = benchmark("Range checks", test_data, is_whitespace_range);
    lookup_time = benchmark("Lookup table", test_data, is_whitespace_lookup);
    std::cout << "Speedup: " << std::fixed << std::setprecision(2)
              << (range_time / lookup_time) << "x\n\n";

    // Memory footprint analysis
    std::cout << "Memory Analysis:\n";
    std::cout << "----------------\n";
    std::cout << "Lookup table size: " << sizeof(char_lookup_table) << " bytes\n";
    std::cout << "Cache line aligned: Yes (alignas(64) specified)\n";
    std::cout << "Fits in L1 cache: Yes (256 bytes < 32KB)\n\n";

    // Character coverage validation
    std::cout << "Character Coverage Validation:\n";
    std::cout << "-------------------------------\n";
    int classified = 0;
    for (int i = 0; i < 256; ++i) {
        if (char_lookup_table[i] != 0) {
            classified++;
        }
    }
    std::cout << "Classified characters: " << classified << "/256 ("
              << (classified * 100.0 / 256) << "%)\n";

    return 0;
}