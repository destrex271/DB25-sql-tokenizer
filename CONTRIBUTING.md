# Contributing to DB25 SQL Tokenizer

**Maintainer:** Chiradip Mandal  
**Organization:** Space-RF.org  
**Email:** chiradip@chiradip.com

Thank you for your interest in contributing to the DB25 SQL Tokenizer! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Setup](#development-setup)
4. [Contribution Process](#contribution-process)
5. [Code Standards](#code-standards)
6. [Testing Requirements](#testing-requirements)
7. [Performance Guidelines](#performance-guidelines)
8. [Documentation Standards](#documentation-standards)
9. [Pull Request Process](#pull-request-process)
10. [Community](#community)

## Code of Conduct

### Our Pledge

We pledge to make participation in our project a harassment-free experience for everyone, regardless of age, body size, disability, ethnicity, gender identity, level of experience, education, socio-economic status, nationality, personal appearance, race, religion, or sexual identity.

### Our Standards

**Positive behaviors include:**
- Using welcoming and inclusive language
- Being respectful of differing viewpoints
- Gracefully accepting constructive criticism
- Focusing on what is best for the community
- Showing empathy towards other community members

**Unacceptable behaviors include:**
- Harassment of any kind
- Publishing others' private information
- Trolling or insulting/derogatory comments
- Other conduct which could be considered inappropriate

## Getting Started

### Prerequisites

Before contributing, ensure you have:

1. **C++23 compatible compiler:**
   - Clang 15+ (recommended)
   - GCC 13+
   - MSVC 2022+

2. **Build tools:**
   - CMake 3.20+
   - Git 2.30+
   - Python 3.8+ (for scripts)

3. **Hardware requirements:**
   - CPU with SIMD support (SSE4.2 minimum)
   - 4GB RAM minimum
   - 1GB free disk space

### First Steps

1. Fork the repository on GitHub
2. Clone your fork locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/DB25-sql-tokenizer.git
   cd DB25-sql-tokenizer
   ```

3. Add upstream remote:
   ```bash
   git remote add upstream https://github.com/Space-RF/DB25-sql-tokenizer.git
   ```

4. Create a development branch:
   ```bash
   git checkout -b feature/your-feature-name
   ```

## Development Setup

### Building the Project

```bash
# Configure build
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
              -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
cmake --build build -j$(nproc)

# Run tests
cd build && ctest --output-on-failure
```

### Development Configurations

#### Debug Build
```bash
cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug \
                     -DENABLE_SANITIZERS=ON \
                     -DENABLE_COVERAGE=ON
```

#### Release Build with Optimizations
```bash
cmake -B build-release -DCMAKE_BUILD_TYPE=Release \
                       -DMARCH_NATIVE=ON \
                       -DENABLE_LTO=ON
```

#### Profile Build
```bash
cmake -B build-profile -DCMAKE_BUILD_TYPE=RelWithDebInfo \
                      -DENABLE_PROFILING=ON
```

### IDE Setup

#### Visual Studio Code

`.vscode/settings.json`:
```json
{
    "cmake.configureSettings": {
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
    },
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "editor.formatOnSave": true,
    "clang-format.executable": "clang-format"
}
```

#### CLion

1. Open project as CMake project
2. Set CMake options in Settings → Build → CMake
3. Enable ClangFormat: Settings → Editor → Code Style

## Contribution Process

### 1. Issue First

Before starting work:
- Check existing issues for duplicates
- Create an issue describing your proposed change
- Wait for maintainer feedback
- Reference the issue in your PR

### 2. Types of Contributions

#### Bug Fixes
- Provide minimal reproduction case
- Include before/after behavior
- Add regression test

#### Performance Improvements
- Include benchmark results
- Document methodology
- Show comparisons across architectures

#### New Features
- Discuss design in issue first
- Follow existing patterns
- Maintain backward compatibility

#### Documentation
- Fix typos and clarifications
- Add examples
- Improve explanations

### 3. Branch Naming

Use descriptive branch names:
- `feature/simd-avx512-support`
- `fix/memory-leak-tokenizer`
- `perf/optimize-keyword-lookup`
- `docs/update-tutorial`

## Code Standards

### C++ Style Guide

#### Naming Conventions

```cpp
// Classes: PascalCase
class SimdTokenizer {
    // Member variables: trailing underscore
    size_t position_;
    
    // Methods: snake_case
    void skip_whitespace();
    
    // Constants: UPPER_CASE
    static constexpr size_t MAX_TOKEN_SIZE = 4096;
    
    // Namespaces: lowercase
    namespace db25 {
    }
};

// Functions: snake_case
size_t find_next_token(const std::byte* data);

// Template parameters: PascalCase
template<typename TokenType>
class TokenProcessor {
};
```

#### Formatting

Use clang-format with provided `.clang-format`:

```bash
# Format single file
clang-format -i src/simd_tokenizer.cpp

# Format all files
find . -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

#### Modern C++ Guidelines

```cpp
// Use modern C++ features
auto tokens = tokenizer.tokenize();  // auto where appropriate

// Prefer algorithms over raw loops
std::ranges::for_each(tokens, process_token);

// Use structured bindings
auto [token, error] = parse_token();

// Concepts for template constraints
template<typename T>
requires std::integral<T>
void process(T value);

// string_view for non-owning strings
std::string_view get_token_value();

// [[nodiscard]] for important returns
[[nodiscard]] bool validate_token();
```

### SIMD Code Guidelines

```cpp
// Always provide scalar fallback
size_t skip_whitespace(const std::byte* data, size_t size) {
    #ifdef __AVX2__
        return skip_whitespace_avx2(data, size);
    #elif defined(__SSE4_2__)
        return skip_whitespace_sse42(data, size);
    #elif defined(__ARM_NEON)
        return skip_whitespace_neon(data, size);
    #else
        return skip_whitespace_scalar(data, size);
    #endif
}

// Document SIMD assumptions
// Assumes: 16-byte aligned data, size >= 16
[[gnu::target("avx2")]]
size_t process_avx2(const std::byte* data, size_t size);
```

## Testing Requirements

### Unit Tests

Every contribution must include tests:

```cpp
// test/test_tokenizer.cpp
TEST_CASE("Tokenizer handles empty input") {
    std::string sql = "";
    SimdTokenizer tokenizer(
        reinterpret_cast<const std::byte*>(sql.data()),
        sql.size()
    );
    auto tokens = tokenizer.tokenize();
    REQUIRE(tokens.empty());
}

TEST_CASE("Tokenizer identifies keywords") {
    std::string sql = "SELECT FROM WHERE";
    SimdTokenizer tokenizer(...);
    auto tokens = tokenizer.tokenize();
    
    REQUIRE(tokens[0].type == TokenType::Keyword);
    REQUIRE(tokens[0].keyword_id == Keyword::SELECT);
    // ...
}
```

### Integration Tests

Test against `sql_test.sqls`:

```cpp
TEST_CASE("All SQL test cases pass") {
    SqlTestRunner runner;
    REQUIRE(runner.load_test_file("test/sql_test.sqls"));
    
    auto results = runner.run_all_tests();
    for (const auto& [name, passed] : results) {
        INFO("Test case: " << name);
        REQUIRE(passed);
    }
}
```

### Performance Tests

Include benchmarks for performance changes:

```cpp
BENCHMARK("Tokenize complex query") {
    std::string sql = load_complex_query();
    return SimdTokenizer(...).tokenize();
};

BENCHMARK("Skip whitespace SIMD vs Scalar") | {
    std::string input(1000000, ' ');
    
    BENCHMARK("SIMD") {
        skip_whitespace_simd(input.data(), input.size());
    };
    
    BENCHMARK("Scalar") {
        skip_whitespace_scalar(input.data(), input.size());
    };
};
```

### Test Coverage

Maintain minimum 90% test coverage:

```bash
# Generate coverage report
cmake -B build -DENABLE_COVERAGE=ON
cmake --build build
cd build && ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## Performance Guidelines

### Benchmarking Protocol

1. **Baseline Measurement:**
   ```bash
   # Before changes
   ./benchmark --baseline > baseline.json
   ```

2. **Apply Changes**

3. **Compare Results:**
   ```bash
   # After changes
   ./benchmark --compare baseline.json
   ```

### Performance Criteria

Changes must not regress performance:
- Throughput: ≥17 MB/s average
- Latency: <100μs for 1KB query
- Memory: No increase in allocations

### Profiling

Use appropriate tools:

```bash
# Linux: perf
perf record ./test_sql_file
perf report

# macOS: Instruments
instruments -t "Time Profiler" ./test_sql_file

# Valgrind for memory
valgrind --tool=massif ./test_sql_file
```

## Documentation Standards

### Code Comments

```cpp
// Brief description for simple functions
size_t count_tokens(const std::vector<Token>& tokens);

/**
 * @brief Tokenizes SQL input using SIMD acceleration
 * 
 * @param data Pointer to input buffer
 * @param size Size of input in bytes
 * @return Vector of tokens, empty on error
 * 
 * @note Input must remain valid during tokenization
 * @throws std::bad_alloc on memory exhaustion
 * 
 * @example
 *   auto tokens = tokenize(sql.data(), sql.size());
 */
std::vector<Token> tokenize(const std::byte* data, size_t size);
```

### Documentation Updates

Update relevant docs for your changes:
- API changes: Update `docs/API.md`
- New features: Update `docs/TUTORIAL.md`
- Architecture changes: Update `docs/ARCHITECTURE.md`
- Performance: Update benchmark results

## Pull Request Process

### PR Checklist

Before submitting:
- [ ] Code follows style guidelines
- [ ] Tests pass locally
- [ ] Added/updated tests for changes
- [ ] Documentation updated
- [ ] Benchmarks show no regression
- [ ] Commit messages are clear
- [ ] Branch is up-to-date with main

### PR Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Performance improvement
- [ ] Documentation update

## Testing
- [ ] Unit tests pass
- [ ] Integration tests pass
- [ ] Manual testing performed

## Performance Impact
- Benchmark results: [link or paste]
- Memory usage: [no change/improved/acceptable increase]

## Related Issues
Fixes #123

## Checklist
- [ ] Code follows project style
- [ ] Self-review completed
- [ ] Documentation updated
```

### Review Process

1. **Automated Checks:**
   - CI/CD runs tests
   - Code coverage verified
   - Style check passes

2. **Human Review:**
   - Architecture compliance
   - Performance validation
   - Documentation quality

3. **Merge Criteria:**
   - All checks pass
   - Approved by maintainer
   - No merge conflicts

## Community

### Getting Help

- **GitHub Issues:** Bug reports and feature requests
- **Discussions:** General questions and ideas
- **Email:** chiradip@chiradip.com for direct contact

### Contributing Ideas

We welcome ideas for:
- Performance optimizations
- New SQL dialect support
- Better error handling
- Documentation improvements
- Testing enhancements

### Recognition

Contributors are recognized in:
- AUTHORS file
- Release notes
- Project documentation

## Advanced Topics

### Adding SIMD Support

When adding new SIMD implementations:

1. **Detection Code:**
```cpp
// src/cpu_features.cpp
bool supports_avx512() {
    #ifdef __x86_64__
        // CPU detection logic
    #endif
    return false;
}
```

2. **Implementation:**
```cpp
// src/simd/avx512.cpp
[[gnu::target("avx512f,avx512bw")]]
size_t process_avx512(const std::byte* data, size_t size) {
    // AVX-512 implementation
}
```

3. **Dispatcher Update:**
```cpp
// src/simd_dispatcher.cpp
case SimdLevel::AVX512:
    return func(SimdProcessor<SimdLevel::AVX512>{});
```

4. **Testing:**
```cpp
// test/test_simd.cpp
TEST_CASE("AVX-512 processing") {
    REQUIRE(supports_avx512());
    // Test AVX-512 specific behavior
}
```

### Memory Optimization

When optimizing memory:

1. Profile current usage
2. Identify allocation hotspots
3. Apply optimization
4. Verify with benchmarks
5. Document changes

## License

By contributing, you agree that your contributions will be licensed under the MIT License.