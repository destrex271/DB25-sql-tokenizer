/*
 * Comprehensive operator test for DB25 SQL Tokenizer
 * Tests all SQL operators to ensure no regressions
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "simd_tokenizer.hpp"

using namespace db25;

struct TestCase {
    std::string sql;
    std::vector<std::string> expected_tokens;
    std::string description;
};

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::Unknown: return "Unknown";
        case TokenType::Keyword: return "Keyword";
        case TokenType::Identifier: return "Identifier";
        case TokenType::Number: return "Number";
        case TokenType::String: return "String";
        case TokenType::Operator: return "Operator";
        case TokenType::Delimiter: return "Delimiter";
        case TokenType::Comment: return "Comment";
        case TokenType::Whitespace: return "Whitespace";
        case TokenType::EndOfFile: return "EOF";
        default: return "?";
    }
}

bool test_operator(const TestCase& test) {
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
    
    // Compare with expected
    if (actual_tokens.size() != test.expected_tokens.size()) {
        std::cout << "✗ FAIL: " << test.description << "\n";
        std::cout << "  SQL: \"" << test.sql << "\"\n";
        std::cout << "  Expected " << test.expected_tokens.size() << " tokens, got " 
                  << actual_tokens.size() << "\n";
        std::cout << "  Expected: ";
        for (const auto& t : test.expected_tokens) std::cout << "[" << t << "] ";
        std::cout << "\n  Got:      ";
        for (const auto& t : actual_tokens) std::cout << "[" << t << "] ";
        std::cout << "\n";
        return false;
    }
    
    for (size_t i = 0; i < actual_tokens.size(); ++i) {
        if (actual_tokens[i] != test.expected_tokens[i]) {
            std::cout << "✗ FAIL: " << test.description << "\n";
            std::cout << "  SQL: \"" << test.sql << "\"\n";
            std::cout << "  Token " << i << " mismatch: expected [" 
                      << test.expected_tokens[i] << "], got [" 
                      << actual_tokens[i] << "]\n";
            return false;
        }
    }
    
    std::cout << "✓ PASS: " << test.description << "\n";
    return true;
}

int main() {
    std::cout << "DB25 Tokenizer - Comprehensive Operator Test\n";
    std::cout << "============================================\n\n";
    
    std::vector<TestCase> test_cases = {
        // Basic operators
        {"a = b", {"a", "=", "b"}, "Single equals"},
        {"a == b", {"a", "==", "b"}, "Double equals (comparison)"},
        {"a != b", {"a", "!=", "b"}, "Not equals"},
        {"a <> b", {"a", "<>", "b"}, "SQL not equals"},
        {"a < b", {"a", "<", "b"}, "Less than"},
        {"a > b", {"a", ">", "b"}, "Greater than"},
        {"a <= b", {"a", "<=", "b"}, "Less than or equal"},
        {"a >= b", {"a", ">=", "b"}, "Greater than or equal"},
        
        // Arithmetic operators
        {"a + b", {"a", "+", "b"}, "Addition"},
        {"a - b", {"a", "-", "b"}, "Subtraction"},
        {"a * b", {"a", "*", "b"}, "Multiplication"},
        {"a / b", {"a", "/", "b"}, "Division"},
        {"a % b", {"a", "%", "b"}, "Modulo"},
        
        // Logical operators
        {"a AND b", {"a", "AND", "b"}, "Logical AND"},
        {"a OR b", {"a", "OR", "b"}, "Logical OR"},
        {"NOT a", {"NOT", "a"}, "Logical NOT"},
        {"a && b", {"a", "&&", "b"}, "C-style AND"},
        {"a || b", {"a", "||", "b"}, "C-style OR"},
        
        // Bitwise operators
        {"a & b", {"a", "&", "b"}, "Bitwise AND"},
        {"a | b", {"a", "|", "b"}, "Bitwise OR"},
        {"a ^ b", {"a", "^", "b"}, "Bitwise XOR"},
        {"~a", {"~", "a"}, "Bitwise NOT"},
        {"a << b", {"a", "<<", "b"}, "Left shift"},
        {"a >> b", {"a", ">>", "b"}, "Right shift"},
        
        // Special operators
        {"a::text", {"a", "::", "text"}, "PostgreSQL cast"},
        {"a.b", {"a", ".", "b"}, "Dot notation"},
        {"a->b", {"a", "-", ">", "b"}, "Arrow (tokenized as separate)"},
        {"a->>b", {"a", "-", ">>", "b"}, "JSON arrow (tokenized as separate)"},
        
        // Invalid operators that should be tokenized as separate tokens
        {"a === b", {"a", "==", "=", "b"}, "Triple equals (invalid, tokenized as ==, =)"},
        {"a ==== b", {"a", "==", "==", "b"}, "Quadruple equals (invalid, tokenized as ==, ==)"},
        {"a !== b", {"a", "!=", "=", "b"}, "Not double equals (invalid, tokenized as !=, =)"},
        {"a <<< b", {"a", "<<", "<", "b"}, "Triple left shift (invalid, tokenized as <<, <)"},
        {"a >>> b", {"a", ">>", ">", "b"}, "Triple right shift (invalid, tokenized as >>, >)"},
        
        // Complex expressions
        {"(a+b)*c", {"(", "a", "+", "b", ")", "*", "c"}, "Expression with parentheses"},
        {"a >= 10 AND b <= 20", {"a", ">=", "10", "AND", "b", "<=", "20"}, "Complex condition"},
        {"CASE WHEN a == b THEN 1 ELSE 0 END", 
         {"CASE", "WHEN", "a", "==", "b", "THEN", "1", "ELSE", "0", "END"}, 
         "CASE statement with =="},
        {"SELECT * FROM t WHERE x != y", 
         {"SELECT", "*", "FROM", "t", "WHERE", "x", "!=", "y"}, 
         "SELECT with !="},
    };
    
    int passed = 0;
    int failed = 0;
    
    for (const auto& test : test_cases) {
        if (test_operator(test)) {
            passed++;
        } else {
            failed++;
        }
    }
    
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "Test Summary\n";
    std::cout << std::string(50, '=') << "\n";
    std::cout << "Total Tests: " << test_cases.size() << "\n";
    std::cout << "Passed:      " << passed << "\n";
    std::cout << "Failed:      " << failed << "\n";
    std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
              << (passed * 100.0 / test_cases.size()) << "%\n";
    
    if (failed > 0) {
        std::cout << "\n⚠️  Some tests failed! Please review the failures above.\n";
        return 1;
    } else {
        std::cout << "\n✅ All tests passed! No regressions detected.\n";
        return 0;
    }
}