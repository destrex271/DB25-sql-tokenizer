# DB25 SQL Tokenizer Tutorial

**Author:** Chiradip Mandal  
**Organization:** Space-RF.org  
**Email:** chiradip@chiradip.com

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Basic Usage](#basic-usage)
4. [Understanding Token Types](#understanding-token-types)
5. [SIMD Optimization](#simd-optimization)
6. [Advanced Usage](#advanced-usage)
7. [Performance Tuning](#performance-tuning)
8. [Integration Examples](#integration-examples)

## Introduction

The DB25 SQL Tokenizer is a high-performance lexical analyzer that converts SQL text into a stream of tokens. It leverages SIMD (Single Instruction, Multiple Data) instructions to achieve unprecedented performance.

### What is Tokenization?

Tokenization is the process of breaking down SQL text into meaningful units:

```sql
SELECT * FROM users WHERE age > 21
```

Becomes:

```
[Keyword: SELECT]
[Operator: *]
[Keyword: FROM]
[Identifier: users]
[Keyword: WHERE]
[Identifier: age]
[Operator: >]
[Number: 21]
```

## Installation

### From Source

```bash
git clone https://github.com/Space-RF/DB25-sql-tokenizer.git
cd DB25-sql-tokenizer
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
sudo cmake --install build
```

### Using as a Library

In your `CMakeLists.txt`:

```cmake
find_package(DB25Tokenizer REQUIRED)
target_link_libraries(your_app PRIVATE DB25::Tokenizer)
```

## Basic Usage

### Example 1: Simple Tokenization

```cpp
#include <iostream>
#include "simd_tokenizer.hpp"

int main() {
    using namespace db25;
    
    std::string sql = "SELECT name, age FROM users";
    
    // Create tokenizer
    SimdTokenizer tokenizer(
        reinterpret_cast<const std::byte*>(sql.data()),
        sql.size()
    );
    
    // Tokenize
    auto tokens = tokenizer.tokenize();
    
    // Process tokens
    for (const auto& token : tokens) {
        if (token.type != TokenType::Whitespace) {
            std::cout << "Token: " << token.value 
                      << " [Type: " << static_cast<int>(token.type) 
                      << ", Line: " << token.line 
                      << ", Col: " << token.column << "]\n";
        }
    }
    
    return 0;
}
```

### Example 2: Processing Complex Queries

```cpp
#include "simd_tokenizer.hpp"
#include <fstream>
#include <sstream>

std::vector<db25::Token> tokenize_file(const std::string& filename) {
    // Read SQL file
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sql = buffer.str();
    
    // Tokenize with performance measurement
    auto start = std::chrono::high_resolution_clock::now();
    
    db25::SimdTokenizer tokenizer(
        reinterpret_cast<const std::byte*>(sql.data()),
        sql.size()
    );
    auto tokens = tokenizer.tokenize();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);
    
    std::cout << "Tokenized " << tokens.size() << " tokens in " 
              << duration.count() << " ms\n";
    std::cout << "Throughput: " << (tokens.size() * 1000.0 / duration.count()) 
              << " tokens/second\n";
    
    return tokens;
}
```

## Understanding Token Types

The tokenizer recognizes the following token types:

### 1. Keywords (26% of typical SQL)

Reserved SQL words like SELECT, FROM, WHERE:

```cpp
bool is_sql_keyword(const db25::Token& token) {
    return token.type == db25::TokenType::Keyword;
}

// Keywords have associated enum values
if (token.keyword_id == db25::Keyword::SELECT) {
    // Handle SELECT statement
}
```

### 2. Identifiers (28% of typical SQL)

Table names, column names, aliases:

```cpp
// Identifiers follow SQL naming rules
// Can start with letter or underscore
// Can contain letters, digits, underscores
if (token.type == db25::TokenType::Identifier) {
    std::cout << "Identifier: " << token.value << "\n";
}
```

### 3. Numbers (4% of typical SQL)

Integer and floating-point literals:

```cpp
// Supports various formats
// 123, 45.67, 1.23e10, -89, 0.5
if (token.type == db25::TokenType::Number) {
    double value = std::stod(std::string(token.value));
}
```

### 4. Strings (4% of typical SQL)

String literals with quote handling:

```cpp
// Supports single and double quotes
// Handles escaped quotes: 'It''s', "He said ""hello"""
if (token.type == db25::TokenType::String) {
    // Remove surrounding quotes
    std::string str(token.value.substr(1, token.value.length() - 2));
}
```

### 5. Operators (12% of typical SQL)

Mathematical and comparison operators:

```cpp
// =, <>, <=, >=, !=, +, -, *, /, ||, &&
if (token.type == db25::TokenType::Operator) {
    if (token.value == "=") {
        // Equality comparison
    } else if (token.value == "<>") {
        // Not equal
    }
}
```

### 6. Delimiters (25% of typical SQL)

Parentheses, commas, semicolons:

```cpp
// (, ), [, ], {, }, ,, ;
if (token.type == db25::TokenType::Delimiter) {
    if (token.value == "(") {
        // Start of expression or function call
    }
}
```

## SIMD Optimization

### Automatic CPU Detection

The tokenizer automatically detects and uses the best available SIMD instruction set:

```cpp
db25::SimdTokenizer tokenizer(data, size);
std::cout << "Using SIMD level: " << tokenizer.simd_level() << "\n";
// Output: "ARM NEON" or "AVX2" or "SSE4.2"
```

### Performance Comparison

```cpp
void benchmark_simd_vs_scalar() {
    std::string sql = load_large_sql_file();
    
    // SIMD tokenization
    auto simd_start = now();
    db25::SimdTokenizer simd_tokenizer(...);
    auto simd_tokens = simd_tokenizer.tokenize();
    auto simd_time = now() - simd_start;
    
    // Scalar tokenization (hypothetical)
    auto scalar_start = now();
    auto scalar_tokens = scalar_tokenize(sql);
    auto scalar_time = now() - scalar_start;
    
    std::cout << "SIMD speedup: " << (scalar_time / simd_time) << "x\n";
    // Typical output: "SIMD speedup: 4.5x"
}
```

## Advanced Usage

### Custom Token Processing

```cpp
class SqlAnalyzer {
private:
    std::map<db25::TokenType, int> token_stats;
    
public:
    void analyze(const std::vector<db25::Token>& tokens) {
        for (const auto& token : tokens) {
            token_stats[token.type]++;
            
            // Custom processing based on token type
            switch (token.type) {
                case db25::TokenType::Keyword:
                    handle_keyword(token);
                    break;
                case db25::TokenType::Identifier:
                    validate_identifier(token);
                    break;
                // ...
            }
        }
    }
    
    void print_statistics() {
        std::cout << "Token Distribution:\n";
        for (const auto& [type, count] : token_stats) {
            std::cout << "  Type " << static_cast<int>(type) 
                      << ": " << count << "\n";
        }
    }
};
```

### Streaming Tokenization

For very large SQL files:

```cpp
class StreamingTokenizer {
private:
    static constexpr size_t CHUNK_SIZE = 1024 * 1024; // 1MB chunks
    
public:
    void process_large_file(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        std::vector<char> buffer(CHUNK_SIZE);
        
        while (file.read(buffer.data(), CHUNK_SIZE) || file.gcount() > 0) {
            size_t bytes_read = file.gcount();
            
            // Find safe split point (not in middle of token)
            size_t split_point = find_token_boundary(buffer, bytes_read);
            
            // Tokenize chunk
            db25::SimdTokenizer tokenizer(
                reinterpret_cast<const std::byte*>(buffer.data()),
                split_point
            );
            auto tokens = tokenizer.tokenize();
            
            // Process tokens
            process_tokens(tokens);
            
            // Move remaining bytes to beginning
            std::memmove(buffer.data(), 
                        buffer.data() + split_point,
                        bytes_read - split_point);
            file.seekg(-(bytes_read - split_point), std::ios::cur);
        }
    }
};
```

## Performance Tuning

### 1. Pre-allocate Token Vector

```cpp
// Estimate tokens based on SQL size
size_t estimated_tokens = sql_size / 8;  // Average 8 chars per token
tokens.reserve(estimated_tokens);
```

### 2. Batch Processing

```cpp
// Process multiple queries efficiently
std::vector<std::vector<db25::Token>> batch_tokenize(
    const std::vector<std::string>& queries) 
{
    std::vector<std::vector<db25::Token>> all_tokens;
    all_tokens.reserve(queries.size());
    
    for (const auto& sql : queries) {
        db25::SimdTokenizer tokenizer(
            reinterpret_cast<const std::byte*>(sql.data()),
            sql.size()
        );
        all_tokens.push_back(tokenizer.tokenize());
    }
    
    return all_tokens;
}
```

### 3. Cache-Friendly Access

```cpp
// Process tokens in order for better cache locality
void process_tokens_efficiently(const std::vector<db25::Token>& tokens) {
    // First pass: keywords
    for (const auto& token : tokens) {
        if (token.type == db25::TokenType::Keyword) {
            process_keyword(token);
        }
    }
    
    // Second pass: identifiers
    for (const auto& token : tokens) {
        if (token.type == db25::TokenType::Identifier) {
            process_identifier(token);
        }
    }
}
```

## Integration Examples

### With SQL Parser

```cpp
class SqlParser {
private:
    db25::SimdTokenizer tokenizer;
    std::vector<db25::Token> tokens;
    size_t current_token = 0;
    
public:
    void parse(const std::string& sql) {
        // Tokenize
        tokenizer = db25::SimdTokenizer(
            reinterpret_cast<const std::byte*>(sql.data()),
            sql.size()
        );
        tokens = tokenizer.tokenize();
        current_token = 0;
        
        // Parse
        auto statement = parse_statement();
    }
    
    std::unique_ptr<Statement> parse_statement() {
        if (check(db25::TokenType::Keyword)) {
            auto keyword = advance();
            if (keyword.keyword_id == db25::Keyword::SELECT) {
                return parse_select();
            } else if (keyword.keyword_id == db25::Keyword::INSERT) {
                return parse_insert();
            }
            // ...
        }
        throw ParseError("Expected statement");
    }
    
    db25::Token advance() {
        return tokens[current_token++];
    }
    
    bool check(db25::TokenType type) {
        return current_token < tokens.size() && 
               tokens[current_token].type == type;
    }
};
```

### With Query Optimizer

```cpp
class QueryOptimizer {
public:
    void optimize(const std::vector<db25::Token>& tokens) {
        // Build token index for fast lookup
        std::unordered_map<std::string_view, std::vector<size_t>> index;
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            if (tokens[i].type == db25::TokenType::Identifier) {
                index[tokens[i].value].push_back(i);
            }
        }
        
        // Find optimization opportunities
        find_redundant_joins(tokens, index);
        optimize_where_clauses(tokens, index);
        suggest_indexes(tokens, index);
    }
};
```

## Best Practices

1. **Always check token type before accessing value**
2. **Use string_view to avoid copies**
3. **Reserve vector capacity when token count is predictable**
4. **Process tokens in batches for better cache performance**
5. **Leverage SIMD detection for optimal performance**

## Troubleshooting

### Common Issues

**Issue:** Slow performance on small queries  
**Solution:** SIMD has overhead for very small inputs. Consider batching.

**Issue:** Memory usage grows with large files  
**Solution:** Use streaming tokenization for files > 100MB.

**Issue:** Different results on different CPUs  
**Solution:** This is expected for performance, not correctness. Results are identical.

## Conclusion

The DB25 SQL Tokenizer provides industrial-strength tokenization with exceptional performance. By leveraging SIMD instructions and zero-copy design, it achieves throughput suitable for the most demanding applications.

For more details, see:
- [Architecture Documentation](ARCHITECTURE.md)
- [API Reference](API.md)
- [Performance Analysis](../papers/performance.pdf)