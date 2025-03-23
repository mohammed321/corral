#include <random>
#include <cinttypes>
#include <chrono>

struct Random {
    std::default_random_engine rng;

    Random(uint64_t seed) : rng(seed) {
    }

    float get_random_float_between_a_inclusive_b_inclusive(float a, float b) {
        std::uniform_real_distribution<> dis(a, b);
        return dis(rng);
    }
    
    int32_t get_random_int_between_a_inclusive_b_inclusive(int32_t a, int32_t b) {
        std::uniform_int_distribution<> dis(a, b);
        return dis(rng);
    }

    static uint64_t get_hourly_seed() {
        auto now = std::chrono::system_clock::now();
        auto epoch = now.time_since_epoch();
        auto hours_since_epoch = std::chrono::duration_cast<std::chrono::hours>(epoch).count();
        return hours_since_epoch;
    }

};


