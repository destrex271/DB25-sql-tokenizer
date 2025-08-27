// DB25 SQL Tokenizer - SQL File Test Suite
// =========================================
// Tests tokenizer against all queries in sql_test.sqls

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <chrono>
#include <map>
#include "simd_tokenizer.hpp"

using namespace db25;

struct SqlTestCase {
    std::string id;
    std::string description;
    std::string level;
    std::string sql;
    int line_number;
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

class SqlTestRunner {
private:
    std::vector<SqlTestCase> test_cases;
    std::map<std::string, int> token_statistics;
    std::map<std::string, double> performance_stats;
    
public:
    bool load_test_file(const std::string& filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Error: Cannot open file: " << filename << "\n";
            return false;
        }
        
        std::string line;
        SqlTestCase current_test;
        bool in_sql = false;
        int line_num = 0;
        
        while (std::getline(file, line)) {
            line_num++;
            
            if (line.substr(0, 5) == "--ID:") {
                current_test.id = line.substr(5);
                // Trim whitespace
                current_test.id.erase(0, current_test.id.find_first_not_of(" \t"));
                current_test.id.erase(current_test.id.find_last_not_of(" \t") + 1);
                current_test.line_number = line_num;
                in_sql = false;
            }
            else if (line.substr(0, 7) == "--DESC:") {
                current_test.description = line.substr(7);
                current_test.description.erase(0, current_test.description.find_first_not_of(" \t"));
                current_test.description.erase(current_test.description.find_last_not_of(" \t") + 1);
            }
            else if (line.substr(0, 8) == "--LEVEL:") {
                current_test.level = line.substr(8);
                current_test.level.erase(0, current_test.level.find_first_not_of(" \t"));
                current_test.level.erase(current_test.level.find_last_not_of(" \t") + 1);
                in_sql = true;
                current_test.sql.clear();
            }
            else if (line == "--END") {
                if (!current_test.id.empty() && !current_test.sql.empty()) {
                    test_cases.push_back(current_test);
                }
                current_test = SqlTestCase();
                in_sql = false;
            }
            else if (in_sql && !line.empty() && line.substr(0, 2) != "--") {
                if (!current_test.sql.empty()) {
                    current_test.sql += "\n";
                }
                current_test.sql += line;
            }
        }
        
        std::cout << "Loaded " << test_cases.size() << " test cases from " << filename << "\n";
        return !test_cases.empty();
    }
    
    void run_all_tests(bool verbose = false, bool show_tokens = false) {
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "Running DB25 Tokenizer Test Suite\n";
        std::cout << std::string(80, '=') << "\n\n";
        
        int passed = 0;
        int failed = 0;
        int total_tokens = 0;
        double total_time_ms = 0;
        
        for (const auto& test : test_cases) {
            std::cout << "[" << std::setw(20) << std::left << test.id << "] ";
            std::cout << "(" << std::setw(8) << test.level << ") ";
            
            if (verbose) {
                std::cout << "\n  Description: " << test.description;
                std::cout << "\n  SQL (line " << test.line_number << "): " << test.sql.substr(0, 50);
                if (test.sql.length() > 50) std::cout << "...";
                std::cout << "\n";
            }
            
            try {
                auto start = std::chrono::high_resolution_clock::now();
                
                SimdTokenizer tokenizer(
                    reinterpret_cast<const std::byte*>(test.sql.data()), 
                    test.sql.size()
                );
                auto tokens = tokenizer.tokenize();
                
                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double, std::milli>(end - start);
                
                // Count non-whitespace tokens
                int token_count = 0;
                for (const auto& token : tokens) {
                    if (token.type != TokenType::Whitespace && 
                        token.type != TokenType::EndOfFile) {
                        token_count++;
                        token_statistics[token_type_to_string(token.type)]++;
                    }
                }
                
                total_tokens += token_count;
                total_time_ms += duration.count();
                performance_stats[test.level] += duration.count();
                
                std::cout << "✓ PASS - " << std::setw(3) << token_count << " tokens in " 
                          << std::fixed << std::setprecision(3) << duration.count() << "ms";
                
                if (show_tokens) {
                    std::cout << "\n  Tokens:\n";
                    int count = 0;
                    for (const auto& token : tokens) {
                        if (token.type != TokenType::Whitespace && 
                            token.type != TokenType::EndOfFile) {
                            std::cout << "    [" << std::setw(10) << token_type_to_string(token.type) 
                                      << "] \"" << token.value << "\"\n";
                            if (++count >= 10 && !verbose) {
                                std::cout << "    ... (" << (token_count - 10) << " more tokens)\n";
                                break;
                            }
                        }
                    }
                }
                
                std::cout << "\n";
                passed++;
                
            } catch (const std::exception& e) {
                std::cout << "✗ FAIL - Exception: " << e.what() << "\n";
                failed++;
            }
        }
        
        // Print summary
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "Test Summary\n";
        std::cout << std::string(80, '=') << "\n";
        std::cout << "Tests Run:    " << test_cases.size() << "\n";
        std::cout << "Passed:       " << passed << "\n";
        std::cout << "Failed:       " << failed << "\n";
        std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
                  << (passed * 100.0 / test_cases.size()) << "%\n";
        
        // Performance statistics
        std::cout << "\n" << std::string(80, '-') << "\n";
        std::cout << "Performance Statistics\n";
        std::cout << std::string(80, '-') << "\n";
        std::cout << "Total Tokens:    " << total_tokens << "\n";
        std::cout << "Total Time:      " << std::fixed << std::setprecision(3) 
                  << total_time_ms << " ms\n";
        std::cout << "Avg Time/Query:  " << total_time_ms / test_cases.size() << " ms\n";
        std::cout << "Tokens/Second:   " << std::fixed << std::setprecision(0)
                  << (total_tokens * 1000.0 / total_time_ms) << "\n";
        std::cout << "SIMD Level:      " << SimdTokenizer(nullptr, 0).simd_level() << "\n";
        
        // Token type distribution
        std::cout << "\n" << std::string(80, '-') << "\n";
        std::cout << "Token Distribution\n";
        std::cout << std::string(80, '-') << "\n";
        for (const auto& [type, count] : token_statistics) {
            double percentage = (count * 100.0) / total_tokens;
            std::cout << std::setw(12) << type << ": " 
                      << std::setw(6) << count << " ("
                      << std::fixed << std::setprecision(1) << percentage << "%)\n";
        }
        
        // Performance by complexity level
        std::cout << "\n" << std::string(80, '-') << "\n";
        std::cout << "Performance by Complexity Level\n";
        std::cout << std::string(80, '-') << "\n";
        std::map<std::string, int> level_counts;
        for (const auto& test : test_cases) {
            level_counts[test.level]++;
        }
        for (const auto& [level, time] : performance_stats) {
            std::cout << std::setw(10) << level << ": " 
                      << std::fixed << std::setprecision(3)
                      << time / level_counts[level] << " ms/query average\n";
        }
    }
    
    void generate_verification_output(const std::string& output_file) {
        std::ofstream out(output_file);
        if (!out) {
            std::cerr << "Error: Cannot create output file: " << output_file << "\n";
            return;
        }
        
        out << "# DB25 Tokenizer Verification Output\n";
        out << "# Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
        out << "# Test Cases: " << test_cases.size() << "\n\n";
        
        for (const auto& test : test_cases) {
            out << "## TEST: " << test.id << "\n";
            out << "## LEVEL: " << test.level << "\n";
            out << "## DESC: " << test.description << "\n";
            out << "## SQL:\n" << test.sql << "\n";
            out << "## TOKENS:\n";
            
            try {
                SimdTokenizer tokenizer(
                    reinterpret_cast<const std::byte*>(test.sql.data()), 
                    test.sql.size()
                );
                auto tokens = tokenizer.tokenize();
                
                int token_num = 0;
                for (const auto& token : tokens) {
                    if (token.type != TokenType::Whitespace && 
                        token.type != TokenType::EndOfFile) {
                        out << token_num++ << "|" 
                            << token_type_to_string(token.type) << "|"
                            << token.value << "|"
                            << token.line << "|" 
                            << token.column << "\n";
                    }
                }
                out << "## END\n\n";
                
            } catch (const std::exception& e) {
                out << "## ERROR: " << e.what() << "\n";
                out << "## END\n\n";
            }
        }
        
        std::cout << "\nVerification output written to: " << output_file << "\n";
    }
};

int main(int argc, char* argv[]) {
    std::string test_file = "test/sql_test.sqls";
    bool verbose = false;
    bool show_tokens = false;
    bool generate_output = false;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-t" || arg == "--tokens") {
            show_tokens = true;
        } else if (arg == "-o" || arg == "--output") {
            generate_output = true;
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options] [test_file]\n";
            std::cout << "Options:\n";
            std::cout << "  -v, --verbose   Show detailed output\n";
            std::cout << "  -t, --tokens    Show tokenization results\n";
            std::cout << "  -o, --output    Generate verification output file\n";
            std::cout << "  -h, --help      Show this help message\n";
            return 0;
        } else if (arg[0] != '-') {
            test_file = arg;
        }
    }
    
    SqlTestRunner runner;
    
    if (!runner.load_test_file(test_file)) {
        std::cerr << "Failed to load test file\n";
        return 1;
    }
    
    runner.run_all_tests(verbose, show_tokens);
    
    if (generate_output) {
        runner.generate_verification_output("tokenizer_verification.txt");
    }
    
    return 0;
}