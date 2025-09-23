/*
 * Test for Invalid Operators - DB25 SQL Tokenizer
 * Ensures that invalid operators like === and !== are properly rejected
 * or tokenized as separate valid operators
 */

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include "simd_tokenizer.hpp"

using namespace db25;

struct InvalidOperatorTest {
    std::string sql;
    std::vector<std::string> expected_tokens;
    std::string description;
};

bool test_invalid_operator(const InvalidOperatorTest& test) {
    SimdTokenizer tokenizer(
        reinterpret_cast<const std::byte*>(test.sql.data()),
        test.sql.size()
    );

    auto tokens = tokenizer.tokenize();

    // Extract non-whitespace, non-EOF tokens
    std::vector<std::string> actual_tokens;
    for (const auto& token : tokens) {
        if (token.type != TokenType::Whitespace &&
            token.type != TokenType::EndOfFile) {
            actual_tokens.push_back(std::string(token.value));
        }
    }

    // Strict comparison
    if (actual_tokens.size() != test.expected_tokens.size()) {
        std::cerr << "✗ FAIL: " << test.description << "\n";
        std::cerr << "  SQL: \"" << test.sql << "\"\n";
        std::cerr << "  Expected " << test.expected_tokens.size()
                  << " tokens, got " << actual_tokens.size() << "\n";
        std::cerr << "  Expected: ";
        for (const auto& t : test.expected_tokens) std::cerr << "[" << t << "] ";
        std::cerr << "\n  Got:      ";
        for (const auto& t : actual_tokens) std::cerr << "[" << t << "] ";
        std::cerr << "\n";
        return false;
    }

    for (size_t i = 0; i < actual_tokens.size(); ++i) {
        if (actual_tokens[i] != test.expected_tokens[i]) {
            std::cerr << "✗ FAIL: " << test.description << "\n";
            std::cerr << "  SQL: \"" << test.sql << "\"\n";
            std::cerr << "  Token " << i << " mismatch: expected ["
                      << test.expected_tokens[i] << "], got ["
                      << actual_tokens[i] << "]\n";
            return false;
        }
    }

    std::cout << "✓ PASS: " << test.description << "\n";
    return true;
}

int main() {
    std::cout << "DB25 Tokenizer - Invalid Operator Test\n";
    std::cout << "=======================================\n\n";

    // These tests ensure that invalid multi-character operators
    // are correctly split into valid operators
    std::vector<InvalidOperatorTest> tests = {
        // Triple equals should tokenize as == followed by =
        {"a === b", {"a", "==", "=", "b"}, "Triple equals splits correctly"},

        // Quadruple equals should tokenize as == followed by ==
        {"x ==== y", {"x", "==", "==", "y"}, "Quadruple equals splits correctly"},

        // Not double equals should tokenize as != followed by =
        {"a !== b", {"a", "!=", "=", "b"}, "Not double equals splits correctly"},

        // Triple less than should tokenize as << followed by <
        {"a <<< b", {"a", "<<", "<", "b"}, "Triple less-than splits correctly"},

        // Triple greater than should tokenize as >> followed by >
        {"a >>> b", {"a", ">>", ">", "b"}, "Triple greater-than splits correctly"},

        // Mixed invalid operators in expression
        {"(a === b) && (c !== d)",
         {"(", "a", "==", "=", "b", ")", "&&", "(", "c", "!=", "=", "d", ")"},
         "Mixed invalid operators in expression"},

        // Invalid operators in SQL context
        {"SELECT * WHERE x === 10 OR y !== 20",
         {"SELECT", "*", "WHERE", "x", "==", "=", "10", "OR", "y", "!=", "=", "20"},
         "Invalid operators in SQL WHERE clause"},

        // Ensure == is not broken when valid
        {"a == b", {"a", "==", "b"}, "Valid double equals remains intact"},

        // Ensure != is not broken when valid
        {"a != b", {"a", "!=", "b"}, "Valid not equals remains intact"},

        // Ensure << is not broken when valid
        {"a << b", {"a", "<<", "b"}, "Valid left shift remains intact"},

        // Ensure >> is not broken when valid
        {"a >> b", {"a", ">>", "b"}, "Valid right shift remains intact"},

        // Edge case: multiple invalids in a row
        {"a ===== b", {"a", "==", "==", "=", "b"}, "Five equals splits correctly"},

        // Edge case: invalid at end of input
        {"value ===", {"value", "==", "="}, "Invalid operator at end of input"},

        // Edge case: invalid at start of input
        {"=== value", {"==", "=", "value"}, "Invalid operator at start of input"}
    };

    int passed = 0;
    int failed = 0;

    // Run all tests
    for (const auto& test : tests) {
        if (test_invalid_operator(test)) {
            passed++;
        } else {
            failed++;
            // Assert failure for immediate CI/CD feedback
            assert(false && "Invalid operator test failed - tokenizer not handling invalid operators correctly!");
        }
    }

    // Summary
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "Test Summary\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "Total Tests: " << tests.size() << "\n";
    std::cout << "Passed:      " << passed << "\n";
    std::cout << "Failed:      " << failed << "\n";

    if (failed > 0) {
        std::cerr << "\n⚠️  CRITICAL: Invalid operators not handled correctly!\n";
        std::cerr << "This is a security risk - invalid operators must be rejected.\n";
        return 1;
    } else {
        std::cout << "\n✅ All invalid operators correctly tokenized.\n";
        std::cout << "No invalid multi-character operators accepted.\n";
        return 0;
    }
}