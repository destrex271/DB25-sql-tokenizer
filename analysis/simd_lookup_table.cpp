/*
 * Copyright (c) 2024 Chiradip Mandal
 * Author: Chiradip Mandal
 * Organization: Space-RF.org
 *
 * SIMD Vectorized Lookup Table for DB25 SQL Tokenizer
 * Demonstrates character classification using SIMD gather operations
 */

#include <iostream>
#include <chrono>
#include <vector>
#include <cstdint>
#include <iomanip>
#include <random>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64)
    #include <immintrin.h>
    #ifdef __SSE2__
        #define HAS_SSE2 1
    #endif
    #ifdef __SSSE3__
        #define HAS_SSSE3 1
    #endif
    #ifdef __SSE4_1__
        #define HAS_SSE41 1
    #endif
    #ifdef __SSE4_2__
        #define HAS_SSE42 1
    #endif
    #ifdef __AVX2__
        #define HAS_AVX2 1
    #endif
    #ifdef __AVX512F__
        #define HAS_AVX512 1
    #endif
#elif defined(__aarch64__) || defined(_M_ARM64)
    #include <arm_neon.h>
    #define HAS_NEON 1
#endif

// Character class flags (can be combined with bitwise OR)
enum CharClass : uint8_t {
    CHAR_NONE        = 0x00,
    CHAR_WHITESPACE  = 0x01,
    CHAR_ALPHA_UPPER = 0x02,
    CHAR_ALPHA_LOWER = 0x04,
    CHAR_DIGIT       = 0x08,
    CHAR_UNDERSCORE  = 0x10,
    CHAR_QUOTE       = 0x20,
    CHAR_OPERATOR    = 0x40,
    CHAR_DELIMITER   = 0x80,

    CHAR_ALPHA       = CHAR_ALPHA_UPPER | CHAR_ALPHA_LOWER,
    CHAR_IDENT_START = CHAR_ALPHA | CHAR_UNDERSCORE,
    CHAR_IDENT_CONT  = CHAR_IDENT_START | CHAR_DIGIT
};

// Lookup table for character classification (cache-line aligned)
alignas(64) static constexpr uint8_t char_lookup_table[256] = {
    // 0x00 - 0x1F: Control characters
    0, 0, 0, 0, 0, 0, 0, 0, 0, CHAR_WHITESPACE, CHAR_WHITESPACE, 0, 0, CHAR_WHITESPACE, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0x20 - 0x3F
    CHAR_WHITESPACE, CHAR_OPERATOR, CHAR_QUOTE, 0, 0, CHAR_OPERATOR, CHAR_OPERATOR, CHAR_QUOTE,
    CHAR_DELIMITER, CHAR_DELIMITER, CHAR_OPERATOR, CHAR_OPERATOR, CHAR_DELIMITER, CHAR_OPERATOR, CHAR_OPERATOR, CHAR_OPERATOR,
    // 0x30 - 0x3F: Digits
    CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT,
    CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT, CHAR_DIGIT,
    CHAR_DELIMITER, CHAR_DELIMITER, CHAR_OPERATOR, CHAR_OPERATOR, CHAR_OPERATOR, 0,
    // 0x40 - 0x5A: @ and uppercase
    0, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER, CHAR_ALPHA_UPPER,
    CHAR_ALPHA_UPPER,
    // 0x5B - 0x60
    CHAR_DELIMITER, 0, CHAR_DELIMITER, CHAR_OPERATOR, CHAR_UNDERSCORE, 0,
    // 0x61 - 0x7A: Lowercase
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER, CHAR_ALPHA_LOWER,
    CHAR_ALPHA_LOWER,
    // 0x7B - 0x7F
    CHAR_DELIMITER, CHAR_OPERATOR, CHAR_DELIMITER, CHAR_OPERATOR, 0,
    // 0x80 - 0xFF: Extended ASCII
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#ifdef HAS_SSE42
// SSE4.2 implementation using PCMPESTRI for string operations
class SSE42CharClassifier {
public:
    // Using SSE4.2 PCMPESTRI for character classification
    static size_t classify_whitespace_sse42(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        // Create a vector with whitespace characters
        const __m128i whitespace = _mm_set_epi8(
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            '\r', '\n', '\t', ' '
        );

        while (processed + 16 <= size) {
            __m128i chars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + processed));

            // Use PCMPESTRI to find whitespace characters
            for (int i = 0; i < 16; ++i) {
                __m128i single_char = _mm_set1_epi8(data[processed + i]);
                int is_ws = _mm_cmpestrc(whitespace, 4, single_char, 1,
                    _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY);
                output[processed + i] = is_ws ? 1 : 0;
            }

            processed += 16;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            uint8_t ch = data[i];
            output[i] = (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') ? 1 : 0;
        }

        return size;
    }

    // SSE4.2 with lookup table
    static size_t classify_identifier_start_sse42(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        while (processed + 16 <= size) {
            __m128i chars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + processed));

            // Manual lookup for 16 bytes
            alignas(16) uint8_t temp[16];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(temp), chars);

            for (int i = 0; i < 16; ++i) {
                temp[i] = char_lookup_table[temp[i]];
            }

            __m128i classes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(temp));
            __m128i mask = _mm_set1_epi8(CHAR_IDENT_START);
            __m128i result = _mm_and_si128(classes, mask);

            // Convert to boolean
            for (int i = 0; i < 16; ++i) {
                output[processed + i] = (reinterpret_cast<uint8_t*>(&result)[i]) ? 1 : 0;
            }

            processed += 16;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
        }

        return size;
    }
};
#endif

#ifdef HAS_SSSE3
// SSSE3 implementation using PSHUFB for byte shuffle (lookup)
class SSSE3CharClassifier {
public:
    // SSSE3 PSHUFB can be used as a 16-byte lookup table!
    static size_t classify_identifier_start_ssse3(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        while (processed + 16 <= size) {
            __m128i chars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + processed));

            // Manual lookup for 16 bytes (simplified)
            alignas(16) uint8_t temp[16];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(temp), chars);

            for (int i = 0; i < 16; ++i) {
                temp[i] = char_lookup_table[temp[i]];
            }

            __m128i classes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(temp));
            __m128i mask = _mm_set1_epi8(CHAR_IDENT_START);
            __m128i result = _mm_and_si128(classes, mask);

            // Store results
            for (int i = 0; i < 16; ++i) {
                output[processed + i] = (reinterpret_cast<uint8_t*>(&result)[i]) ? 1 : 0;
            }

            processed += 16;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
        }

        return size;
    }

    // Advanced SSSE3 using PSHUFB as a real lookup table
    static size_t classify_identifier_start_ssse3_advanced(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        while (processed + 16 <= size) {
            __m128i input = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + processed));

            // Check for uppercase (A-Z: 0x41-0x5A)
            __m128i is_upper = _mm_and_si128(
                _mm_cmpgt_epi8(input, _mm_set1_epi8(0x40)),
                _mm_cmplt_epi8(input, _mm_set1_epi8(0x5B))
            );

            // Check for lowercase (a-z: 0x61-0x7A)
            __m128i is_lower = _mm_and_si128(
                _mm_cmpgt_epi8(input, _mm_set1_epi8(0x60)),
                _mm_cmplt_epi8(input, _mm_set1_epi8(0x7B))
            );

            // Check for underscore (0x5F)
            __m128i is_underscore = _mm_cmpeq_epi8(input, _mm_set1_epi8('_'));

            // Combine
            __m128i is_ident_start = _mm_or_si128(_mm_or_si128(is_upper, is_lower), is_underscore);

            // Store results
            _mm_storeu_si128(reinterpret_cast<__m128i*>(output + processed), is_ident_start);

            // Convert from 0xFF/0x00 to 0x01/0x00
            for (int i = 0; i < 16; ++i) {
                output[processed + i] = output[processed + i] ? 1 : 0;
            }

            processed += 16;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
        }

        return size;
    }
};
#endif

#ifdef HAS_SSE2
// SSE2 implementation - baseline SSE available on all x64
class SSE2CharClassifier {
public:
    static size_t classify_identifier_start_sse2(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        while (processed + 16 <= size) {
            __m128i chars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + processed));

            // Check for uppercase (A-Z)
            __m128i upper_start = _mm_cmpgt_epi8(chars, _mm_set1_epi8('A' - 1));
            __m128i upper_end = _mm_cmplt_epi8(chars, _mm_set1_epi8('Z' + 1));
            __m128i is_upper = _mm_and_si128(upper_start, upper_end);

            // Check for lowercase (a-z)
            __m128i lower_start = _mm_cmpgt_epi8(chars, _mm_set1_epi8('a' - 1));
            __m128i lower_end = _mm_cmplt_epi8(chars, _mm_set1_epi8('z' + 1));
            __m128i is_lower = _mm_and_si128(lower_start, lower_end);

            // Check for underscore
            __m128i is_underscore = _mm_cmpeq_epi8(chars, _mm_set1_epi8('_'));

            // Combine all conditions
            __m128i is_ident_start = _mm_or_si128(_mm_or_si128(is_upper, is_lower), is_underscore);

            // Store results (0xFF becomes 1, 0x00 stays 0)
            for (int i = 0; i < 16; ++i) {
                output[processed + i] = (reinterpret_cast<uint8_t*>(&is_ident_start)[i]) ? 1 : 0;
            }

            processed += 16;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            uint8_t ch = data[i];
            output[i] = ((ch >= 'A' && ch <= 'Z') ||
                        (ch >= 'a' && ch <= 'z') ||
                        ch == '_') ? 1 : 0;
        }

        return size;
    }

    // SSE2 with manual lookup table
    static size_t classify_identifier_start_sse2_lookup(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        while (processed + 16 <= size) {
            __m128i chars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + processed));

            // Manual lookup for 16 bytes
            alignas(16) uint8_t temp[16];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(temp), chars);

            for (int i = 0; i < 16; ++i) {
                temp[i] = char_lookup_table[temp[i]];
            }

            __m128i classes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(temp));
            __m128i mask = _mm_set1_epi8(CHAR_IDENT_START);
            __m128i result = _mm_and_si128(classes, mask);

            // Store results
            for (int i = 0; i < 16; ++i) {
                output[processed + i] = (reinterpret_cast<uint8_t*>(&result)[i]) ? 1 : 0;
            }

            processed += 16;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
        }

        return size;
    }
};
#endif

#ifdef HAS_AVX2
// AVX2 implementation using gather instructions
class AVX2CharClassifier {
public:
    // Process 32 characters at once using AVX2
    static size_t classify_identifier_start_avx2(const uint8_t* data, size_t size, uint8_t* output) {
        const __m256i mask = _mm256_set1_epi8(CHAR_IDENT_START);
        size_t processed = 0;

        // Process 32 bytes at a time
        while (processed + 32 <= size) {
            // Load 32 characters
            __m256i chars = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + processed));

            // Split into two 16-byte chunks for gather (AVX2 gather works on 32-bit indices)
            __m128i chars_low = _mm256_extracti128_si256(chars, 0);
            __m128i chars_high = _mm256_extracti128_si256(chars, 1);

            // Convert to 32-bit indices for gather
            __m256i indices_low = _mm256_cvtepu8_epi32(chars_low);
            __m256i indices_high = _mm256_cvtepu8_epi32(_mm_srli_si128(chars_low, 8));

            // Gather from lookup table (8 bytes at a time)
            __m256i classes_0_7 = _mm256_i32gather_epi32(
                reinterpret_cast<const int*>(char_lookup_table), indices_low, 1);
            __m256i classes_8_15 = _mm256_i32gather_epi32(
                reinterpret_cast<const int*>(char_lookup_table), indices_high, 1);

            // Pack results back to bytes
            __m256i classes_low = _mm256_packus_epi32(
                _mm256_and_si256(classes_0_7, _mm256_set1_epi32(0xFF)),
                _mm256_and_si256(classes_8_15, _mm256_set1_epi32(0xFF)));

            // Process high 16 bytes similarly
            indices_low = _mm256_cvtepu8_epi32(chars_high);
            indices_high = _mm256_cvtepu8_epi32(_mm_srli_si128(chars_high, 8));

            __m256i classes_16_23 = _mm256_i32gather_epi32(
                reinterpret_cast<const int*>(char_lookup_table), indices_low, 1);
            __m256i classes_24_31 = _mm256_i32gather_epi32(
                reinterpret_cast<const int*>(char_lookup_table), indices_high, 1);

            __m256i classes_high = _mm256_packus_epi32(
                _mm256_and_si256(classes_16_23, _mm256_set1_epi32(0xFF)),
                _mm256_and_si256(classes_24_31, _mm256_set1_epi32(0xFF)));

            // Combine low and high results
            __m256i classes = _mm256_packus_epi16(classes_low, classes_high);

            // Check against mask
            __m256i result = _mm256_and_si256(classes, mask);

            // Store results
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(output + processed), result);

            processed += 32;
        }

        // Handle remaining bytes with scalar code
        for (size_t i = processed; i < size; ++i) {
            output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
        }

        return size;
    }

    // Simpler approach: Process 16 bytes at a time without full gather
    static size_t classify_identifier_start_avx2_simple(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        while (processed + 16 <= size) {
            __m128i chars = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + processed));

            // Manual lookup for 16 bytes (unrolled)
            alignas(16) uint8_t temp[16];
            _mm_storeu_si128(reinterpret_cast<__m128i*>(temp), chars);

            for (int i = 0; i < 16; ++i) {
                temp[i] = char_lookup_table[temp[i]];
            }

            __m128i classes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(temp));
            __m128i mask = _mm_set1_epi8(CHAR_IDENT_START);
            __m128i result = _mm_and_si128(classes, mask);

            _mm_storeu_si128(reinterpret_cast<__m128i*>(output + processed), result);
            processed += 16;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
        }

        return size;
    }
};
#endif

#ifdef HAS_AVX512
// AVX-512 implementation with powerful gather and permute instructions
class AVX512CharClassifier {
public:
    // Process 64 characters at once using AVX-512
    static size_t classify_identifier_start_avx512(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        // Process 64 bytes at a time
        while (processed + 64 <= size) {
            // Load 64 characters
            __m512i chars = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data + processed));

            // AVX-512 VPERMI2B can do a 64-byte lookup directly!
            // Load lookup table into two vectors (128 bytes each half)
            __m512i lookup_low = _mm512_broadcast_i32x4(
                _mm_loadu_si128(reinterpret_cast<const __m128i*>(char_lookup_table)));
            __m512i lookup_high = _mm512_broadcast_i32x4(
                _mm_loadu_si128(reinterpret_cast<const __m128i*>(char_lookup_table + 128)));

            // Use permutexvar to perform the lookup
            __m512i classes;

            // Split chars into low and high halves for lookup
            __mmask64 high_mask = _mm512_cmpge_epu8_mask(chars, _mm512_set1_epi8(128));

            // Perform lookup using permutexvar_epi8 (AVX-512VBMI)
            #ifdef __AVX512VBMI__
            // If AVX-512VBMI is available, we can do direct byte permutation
            __m512i lookup_full = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(char_lookup_table));
            __m512i lookup_full2 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(char_lookup_table + 64));
            __m512i lookup_full3 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(char_lookup_table + 128));
            __m512i lookup_full4 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(char_lookup_table + 192));

            classes = _mm512_permutexvar_epi8(chars, lookup_full);
            #else
            // Without VBMI, we need to use a different approach
            // Process in 16-byte chunks
            alignas(64) uint8_t temp[64];
            _mm512_storeu_si512(temp, chars);

            for (int i = 0; i < 64; ++i) {
                temp[i] = char_lookup_table[temp[i]];
            }

            classes = _mm512_loadu_si512(temp);
            #endif

            // Check against identifier start mask
            __m512i mask = _mm512_set1_epi8(CHAR_IDENT_START);
            __m512i result = _mm512_and_si512(classes, mask);

            // Convert to boolean (0 or 1)
            __mmask64 match_mask = _mm512_test_epi8_mask(result, result);

            // Store results
            for (int i = 0; i < 64; ++i) {
                output[processed + i] = (match_mask & (1ULL << i)) ? 1 : 0;
            }

            processed += 64;
        }

        // Handle remaining bytes with AVX2 or scalar
        #ifdef HAS_AVX2
        while (processed + 32 <= size) {
            AVX2CharClassifier::classify_identifier_start_avx2_simple(
                data + processed, 32, output + processed);
            processed += 32;
        }
        #endif

        // Handle final remaining with scalar
        for (size_t i = processed; i < size; ++i) {
            output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
        }

        return size;
    }

    // Optimized version using AVX-512 mask operations
    static size_t classify_whitespace_avx512(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;

        // Create comparison vectors for whitespace characters
        __m512i space = _mm512_set1_epi8(' ');
        __m512i tab = _mm512_set1_epi8('\t');
        __m512i newline = _mm512_set1_epi8('\n');
        __m512i cr = _mm512_set1_epi8('\r');

        while (processed + 64 <= size) {
            __m512i chars = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(data + processed));

            // Check for each whitespace character
            __mmask64 is_space = _mm512_cmpeq_epi8_mask(chars, space);
            __mmask64 is_tab = _mm512_cmpeq_epi8_mask(chars, tab);
            __mmask64 is_newline = _mm512_cmpeq_epi8_mask(chars, newline);
            __mmask64 is_cr = _mm512_cmpeq_epi8_mask(chars, cr);

            // Combine all whitespace masks
            __mmask64 is_whitespace = is_space | is_tab | is_newline | is_cr;

            // Store results
            for (int i = 0; i < 64; ++i) {
                output[processed + i] = (is_whitespace & (1ULL << i)) ? 1 : 0;
            }

            processed += 64;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            uint8_t ch = data[i];
            output[i] = (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r') ? 1 : 0;
        }

        return size;
    }
};
#endif

#ifdef HAS_NEON
// ARM NEON implementation
class NEONCharClassifier {
public:
    static size_t classify_identifier_start_neon(const uint8_t* data, size_t size, uint8_t* output) {
        size_t processed = 0;
        const uint8x16_t mask = vdupq_n_u8(CHAR_IDENT_START);

        while (processed + 16 <= size) {
            uint8x16_t chars = vld1q_u8(data + processed);

            // Manual lookup for 16 bytes
            uint8_t temp[16];
            vst1q_u8(temp, chars);

            for (int i = 0; i < 16; ++i) {
                temp[i] = char_lookup_table[temp[i]];
            }

            uint8x16_t classes = vld1q_u8(temp);
            uint8x16_t result = vandq_u8(classes, mask);

            vst1q_u8(output + processed, result);
            processed += 16;
        }

        // Handle remaining
        for (size_t i = processed; i < size; ++i) {
            output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
        }

        return size;
    }
};
#endif

// Scalar implementation for comparison
size_t classify_identifier_start_scalar(const uint8_t* data, size_t size, uint8_t* output) {
    for (size_t i = 0; i < size; ++i) {
        output[i] = (char_lookup_table[data[i]] & CHAR_IDENT_START) ? 1 : 0;
    }
    return size;
}

// Benchmark function
template<typename Func>
double benchmark_classification(const std::string& name, const std::vector<uint8_t>& data,
                               Func func, std::vector<uint8_t>& output) {
    const int iterations = 10000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        func(data.data(), data.size(), output.data());
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);

    double ns_per_char = static_cast<double>(duration.count()) / (iterations * data.size());

    // Count matches
    int matches = 0;
    for (uint8_t val : output) {
        if (val) matches++;
    }

    std::cout << std::setw(35) << name << ": "
              << std::fixed << std::setprecision(3)
              << ns_per_char << " ns/char"
              << " (matches: " << matches << "/" << data.size() << ")" << std::endl;

    return ns_per_char;
}

int main() {
    std::cout << "DB25 SQL Tokenizer - SIMD Lookup Table Analysis\n";
    std::cout << "================================================\n\n";

    // Detect CPU features
    std::cout << "CPU Features:\n";
    std::cout << "-------------\n";
#ifdef HAS_AVX512
    std::cout << "AVX-512: Available\n";
    #ifdef __AVX512VBMI__
    std::cout << "AVX-512 VBMI: Available (optimized byte permute)\n";
    #else
    std::cout << "AVX-512 VBMI: Not available\n";
    #endif
#else
    std::cout << "AVX-512: Not available\n";
#endif
#ifdef HAS_AVX2
    std::cout << "AVX2: Available\n";
#else
    std::cout << "AVX2: Not available\n";
#endif
#ifdef HAS_SSE42
    std::cout << "SSE4.2: Available\n";
#else
    std::cout << "SSE4.2: Not available\n";
#endif
#ifdef HAS_SSSE3
    std::cout << "SSSE3: Available\n";
#else
    std::cout << "SSSE3: Not available\n";
#endif
#ifdef HAS_SSE2
    std::cout << "SSE2: Available\n";
#else
    std::cout << "SSE2: Not available\n";
#endif
#ifdef HAS_NEON
    std::cout << "NEON: Available\n";
#else
    std::cout << "NEON: Not available\n";
#endif
    std::cout << "\n";

    // Generate test data
    std::vector<uint8_t> test_data;
    std::mt19937 gen(42);

    // Create realistic SQL-like data
    const std::string sample = "SELECT user_id, user_name, COUNT(*) as total FROM users WHERE status = 'active' AND created_at > '2024-01-01' GROUP BY user_id ORDER BY total DESC LIMIT 100;";

    // Repeat sample to create larger test set
    for (int i = 0; i < 100; ++i) {
        test_data.insert(test_data.end(), sample.begin(), sample.end());
    }

    std::cout << "Test data size: " << test_data.size() << " bytes\n\n";

    // Output buffer
    std::vector<uint8_t> output(test_data.size());

    // Run benchmarks
    std::cout << "Character Classification Benchmarks:\n";
    std::cout << "------------------------------------\n";

    double scalar_time = benchmark_classification("Scalar lookup table", test_data,
                                                  classify_identifier_start_scalar, output);

#ifdef HAS_SSE2
    double sse2_time = benchmark_classification("SSE2 (range checks)", test_data,
                                                SSE2CharClassifier::classify_identifier_start_sse2, output);
    std::cout << "SSE2 speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / sse2_time) << "x\n";

    double sse2_lookup_time = benchmark_classification("SSE2 (with lookup)", test_data,
                                                       SSE2CharClassifier::classify_identifier_start_sse2_lookup, output);
    std::cout << "SSE2 lookup speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / sse2_lookup_time) << "x\n\n";
#endif

#ifdef HAS_SSSE3
    double ssse3_time = benchmark_classification("SSSE3 (manual lookup)", test_data,
                                                 SSSE3CharClassifier::classify_identifier_start_ssse3, output);
    std::cout << "SSSE3 speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / ssse3_time) << "x\n";

    double ssse3_adv_time = benchmark_classification("SSSE3 (advanced)", test_data,
                                                     SSSE3CharClassifier::classify_identifier_start_ssse3_advanced, output);
    std::cout << "SSSE3 advanced speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / ssse3_adv_time) << "x\n\n";
#endif

#ifdef HAS_SSE42
    double sse42_time = benchmark_classification("SSE4.2 (with lookup)", test_data,
                                                 SSE42CharClassifier::classify_identifier_start_sse42, output);
    std::cout << "SSE4.2 speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / sse42_time) << "x\n";

    double sse42_ws_time = benchmark_classification("SSE4.2 (whitespace)", test_data,
                                                    SSE42CharClassifier::classify_whitespace_sse42, output);
    std::cout << "SSE4.2 whitespace speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / sse42_ws_time) << "x\n\n";
#endif

#ifdef HAS_AVX512
    double avx512_time = benchmark_classification("AVX-512 (64-byte chunks)", test_data,
                                                  AVX512CharClassifier::classify_identifier_start_avx512, output);

    std::cout << "AVX-512 speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / avx512_time) << "x\n";

    double avx512_ws_time = benchmark_classification("AVX-512 whitespace (mask ops)", test_data,
                                                     AVX512CharClassifier::classify_whitespace_avx512, output);

    std::cout << "AVX-512 whitespace speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / avx512_ws_time) << "x\n\n";
#endif

#ifdef HAS_AVX2
    double avx2_simple_time = benchmark_classification("AVX2 simple (16-byte chunks)", test_data,
                                                       AVX2CharClassifier::classify_identifier_start_avx2_simple, output);

    std::cout << "AVX2 simple speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / avx2_simple_time) << "x\n\n";

    // Note: Full gather implementation commented out as it's complex and may not be faster
    // double avx2_gather_time = benchmark_classification("AVX2 with gather", test_data,
    //                                                    AVX2CharClassifier::classify_identifier_start_avx2, output);
#endif

#ifdef HAS_NEON
    double neon_time = benchmark_classification("NEON (16-byte chunks)", test_data,
                                               NEONCharClassifier::classify_identifier_start_neon, output);

    std::cout << "NEON speedup: " << std::fixed << std::setprecision(2)
              << (scalar_time / neon_time) << "x\n\n";
#endif

    // Memory bandwidth analysis
    std::cout << "\nMemory Bandwidth Analysis:\n";
    std::cout << "--------------------------\n";
    double bytes_per_sec = test_data.size() / (scalar_time * 1e-9);
    std::cout << "Scalar throughput:  " << std::fixed << std::setprecision(2)
              << bytes_per_sec / 1e9 << " GB/s\n";

#ifdef HAS_AVX512
    bytes_per_sec = test_data.size() / (avx512_time * 1e-9);
    std::cout << "AVX-512 throughput: " << std::fixed << std::setprecision(2)
              << bytes_per_sec / 1e9 << " GB/s\n";
#endif

#ifdef HAS_AVX2
    bytes_per_sec = test_data.size() / (avx2_simple_time * 1e-9);
    std::cout << "AVX2 throughput:    " << std::fixed << std::setprecision(2)
              << bytes_per_sec / 1e9 << " GB/s\n";
#endif

    std::cout << "\nAnalysis:\n";
    std::cout << "---------\n";
    std::cout << "• SIMD lookup tables process multiple characters in parallel\n";
    std::cout << "• Performance limited by memory bandwidth and lookup latency\n";
    std::cout << "• Best for workloads with predictable patterns\n";
    std::cout << "• Consider prefetching for larger buffers\n";

    return 0;
}