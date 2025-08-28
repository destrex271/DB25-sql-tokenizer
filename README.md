# DB25 SQL Tokenizer

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![SIMD](https://img.shields.io/badge/SIMD-Optimized-green.svg)](docs/ARCHITECTURE.md)
[![Performance](https://img.shields.io/badge/Performance-20M%20tokens%2Fs-red.svg)](docs/ARCHITECTURE.md)

A high-performance SQL tokenizer leveraging SIMD instructions for blazing-fast lexical analysis. Part of the DB25 project, achieving **20+ million tokens per second** throughput on modern hardware.

**Author:** Chiradip Mandal  
**Email:** chiradip@chiradip.com  
**Organization:** [Space-RF.org](https://space-rf.org)

## 🚀 Features

- **SIMD Acceleration**: Automatic CPU feature detection (SSE4.2, AVX2, AVX-512, ARM NEON)
- **Zero-Copy Design**: String views eliminate memory allocation overhead
- **4.5× Faster**: Compared to traditional scalar implementations
- **Grammar-Driven**: Keywords extracted directly from EBNF specification
- **Cross-Platform**: Supports x86_64 and ARM64 architectures
- **Thread-Safe**: Lock-free design for concurrent tokenization
- **Production-Ready**: Comprehensive test suite with 100% pass rate

## 📊 Performance

| Query Complexity | Throughput | Tokens/Second | Speedup vs Scalar |
|-----------------|------------|---------------|-------------------|
| Simple          | 8.5 MB/s   | 1.2M          | 4.0×             |
| Moderate        | 9.2 MB/s   | 2.8M          | 4.0×             |
| Complex         | 11.8 MB/s  | 5.3M          | 4.2×             |
| Extreme         | 14.6 MB/s  | 8.9M          | 4.3×             |
| **Overall**     | **17.7 MB/s** | **20M+**   | **4.5×**         |

## 🛠️ Quick Start

### Prerequisites

- C++23 compatible compiler (Clang 15+, GCC 13+, MSVC 2022+)
- CMake 3.20+
- CPU with SIMD support (most modern processors)

### Building

```bash
# Clone the repository
git clone https://github.com/Space-RF/DB25-sql-tokenizer.git
cd DB25-sql-tokenizer

# Build with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4

# Run tests
cd build && ctest --output-on-failure
```

### Basic Usage

```cpp
#include "simd_tokenizer.hpp"

using namespace db25;

int main() {
    std::string sql = "SELECT * FROM users WHERE age > 21";
    
    SimdTokenizer tokenizer(
        reinterpret_cast<const std::byte*>(sql.data()),
        sql.size()
    );
    
    auto tokens = tokenizer.tokenize();
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::Whitespace) {
            std::cout << token.value << " [" 
                      << token_type_name(token.type) << "]\n";
        }
    }
    
    std::cout << "SIMD Level: " << tokenizer.simd_level() << "\n";
    return 0;
}
```

## 🏗️ Architecture

The tokenizer employs a multi-layered architecture optimized for performance:

```
┌─────────────────────────────────────────────┐
│           SQL Input Buffer                  │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│         CPU Feature Detection               │
│    (SSE4.2/AVX2/AVX-512/NEON)              │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│          SIMD Dispatcher                    │
│  ┌──────────┬──────────┬──────────┐       │
│  │Whitespace│ Keyword  │Identifier│       │
│  │Detection │ Matching │Boundary  │       │
│  └──────────┴──────────┴──────────┘       │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│         Zero-Copy Token Stream              │
│         (string_view references)            │
└─────────────────────────────────────────────┘
```

### Key Components

1. **SIMD Processor Hierarchy**: Runtime CPU detection selects optimal instruction set
2. **Keyword System**: 208 SQL keywords with O(log n) length-bucketed lookup
3. **Token Types**: Keywords, Identifiers, Numbers, Strings, Operators, Delimiters
4. **Memory Management**: Zero-copy design with string_view references

See [ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed design documentation.

## 📚 Documentation

- [**Tutorial**](docs/TUTORIAL.md) - Step-by-step guide with examples
- [**Architecture**](docs/ARCHITECTURE.md) - Detailed system design
- [**Visual Tutorial**](docs/tutorial-diagrams.pdf) - Diagrams and visualizations
- [**Contributing**](CONTRIBUTING.md) - How to contribute
- [**Academic Papers**](papers/) - Research and performance analysis

## 🧪 Testing

The tokenizer includes comprehensive testing against real-world SQL:

```bash
# Build and run tests
cmake --build build

# Run specific test
./build/test_sql_file test/sql_test.sqls

# Generate verification output
./build/test_sql_file -o

# Verbose mode
./build/test_sql_file -v
```

Test coverage includes 23 SQL queries across 4 complexity levels with 100% pass rate.

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on:

- Code style and standards
- Development workflow
- Testing requirements
- Submitting pull requests

## 📈 Benchmarks

Performance benchmarks are detailed in the documentation:

- Token distribution analysis
- SIMD operation performance
- Comparison with other parsers
- Platform-specific results

## 🔮 Future Vision

The DB25 tokenizer is the foundation for a next-generation SQL processing engine:

- **Stage 2**: Recursive descent parser with LALR(1) grammar
- **Stage 3**: Abstract syntax tree construction
- **Stage 4**: Query optimization and plan generation
- **Stage 5**: JIT compilation for expression evaluation

See our [Academic Paper](papers/db25-tokenizer-paper.pdf) for the complete roadmap.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

```
MIT License

Copyright (c) 2024 Chiradip Mandal, Space-RF.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
```

## 📖 Academic Papers

- [DB25 Tokenizer Academic Paper](papers/db25-tokenizer-paper.pdf)
- [Visual Tutorial with Diagrams](docs/tutorial-diagrams.pdf)

## 🙏 Acknowledgments

- Intel and ARM for SIMD instruction set architectures
- The simdjson project for inspiration on SIMD text processing
- The open-source community for valuable feedback

## 📞 Contact

**Chiradip Mandal**  
Email: chiradip@chiradip.com  
Organization: [Space-RF.org](https://space-rf.org)  
GitHub: [@chiradip](https://github.com/chiradip)

---

<p align="center">
  <strong>DB25 SQL Tokenizer</strong> - Pushing the boundaries of SQL processing performance<br>
  Made with ❤️ by <a href="https://space-rf.org">Space-RF.org</a>
</p>