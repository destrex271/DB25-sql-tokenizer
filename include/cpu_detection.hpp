#pragma once

#include <cstdint>
#include <atomic>
#include <bit>

#if defined(__x86_64__) || defined(_M_X64)
    #include <immintrin.h>
    #ifdef __has_include
        #if __has_include(<cpuid.h>)
            #include <cpuid.h>
        #endif
    #endif
#elif defined(__aarch64__) || defined(_M_ARM64)
    #include <arm_neon.h>
    #ifdef __linux__
        #include <sys/auxv.h>
        #include <asm/hwcap.h>
    #endif
#endif

namespace db25 {

enum class SimdLevel : uint8_t {
    None = 0,
    SSE42 = 1,
    AVX2 = 2,
    AVX512 = 3,
    NEON = 4
};

class CpuDetection {
private:
    static std::atomic<SimdLevel> detected_level_;
    static std::atomic<bool> detection_done_;
    
    static void detect_x86_features() noexcept {
        #if defined(__x86_64__) || defined(_M_X64)
        SimdLevel level = SimdLevel::None;
        
        #ifdef __GNUC__
        unsigned int eax, ebx, ecx, edx;
        
        // Check for SSE4.2
        if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
            if (ecx & bit_SSE4_2) {
                level = SimdLevel::SSE42;
            }
        }
        
        // Check for AVX2
        if (__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx)) {
            if (ebx & bit_AVX2) {
                level = SimdLevel::AVX2;
            }
            
            // Check for AVX-512
            if (ebx & bit_AVX512F && ebx & bit_AVX512BW && ebx & bit_AVX512VL) {
                level = SimdLevel::AVX512;
            }
        }
        #elif defined(_MSC_VER)
        int cpuInfo[4];
        
        // Check for SSE4.2
        __cpuid(cpuInfo, 1);
        if (cpuInfo[2] & (1 << 20)) {
            level = SimdLevel::SSE42;
        }
        
        // Check for AVX2
        __cpuidex(cpuInfo, 7, 0);
        if (cpuInfo[1] & (1 << 5)) {
            level = SimdLevel::AVX2;
        }
        
        // Check for AVX-512
        if ((cpuInfo[1] & (1 << 16)) && (cpuInfo[1] & (1 << 30)) && (cpuInfo[1] & (1 << 31))) {
            level = SimdLevel::AVX512;
        }
        #endif
        
        detected_level_.store(level, std::memory_order_release);
        #endif
    }
    
    static void detect_arm_features() noexcept {
        #if defined(__aarch64__) || defined(_M_ARM64)
        SimdLevel level = SimdLevel::NEON;
        detected_level_.store(level, std::memory_order_release);
        #endif
    }
    
public:
    [[nodiscard]] static SimdLevel detect() noexcept {
        bool expected = false;
        if (detection_done_.compare_exchange_strong(expected, true, 
                                                   std::memory_order_acq_rel,
                                                   std::memory_order_acquire)) {
            #if defined(__x86_64__) || defined(_M_X64)
                detect_x86_features();
            #elif defined(__aarch64__) || defined(_M_ARM64)
                detect_arm_features();
            #else
                detected_level_.store(SimdLevel::None, std::memory_order_release);
            #endif
        }
        
        return detected_level_.load(std::memory_order_acquire);
    }
    
    [[nodiscard]] static bool supports_sse42() noexcept {
        auto level = detect();
        return level >= SimdLevel::SSE42;
    }
    
    [[nodiscard]] static bool supports_avx2() noexcept {
        auto level = detect();
        return level >= SimdLevel::AVX2;
    }
    
    [[nodiscard]] static bool supports_avx512() noexcept {
        auto level = detect();
        return level >= SimdLevel::AVX512;
    }
    
    [[nodiscard]] static bool supports_neon() noexcept {
        auto level = detect();
        return level == SimdLevel::NEON;
    }
    
    [[nodiscard]] static const char* level_name() noexcept {
        switch (detect()) {
            case SimdLevel::None: return "Scalar";
            case SimdLevel::SSE42: return "SSE4.2";
            case SimdLevel::AVX2: return "AVX2";
            case SimdLevel::AVX512: return "AVX-512";
            case SimdLevel::NEON: return "ARM NEON";
        }
        return "Unknown";
    }
};

inline std::atomic<SimdLevel> CpuDetection::detected_level_{SimdLevel::None};
inline std::atomic<bool> CpuDetection::detection_done_{false};

}  // namespace db25