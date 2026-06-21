#include <iostream>
#include <cstdint>
#include "engine/LimitOrderBook.hpp"

// Windows vs Linux implementation for the hardware cycle counter
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

inline uint64_t rdtsc() { return __rdtsc(); }

// --- THE FIX: Define the helper function before it is used ---
inline void wait_for_enter(const char* message) {
    std::cout << message;
    std::cin.get(); // Pauses execution until Enter is pressed
}

int main() {
    OrderPool pool(1000000);
    LimitOrderBook lob(pool, 100000, 1000000);

    // Prime the book
    lob.add_order(1, 10050, 500, false);
    lob.add_order(2, 10060, 300, false);
    lob.add_order(3, 10040, 100, true);

    // Benchmark SIMD Range Query
    uint64_t start = rdtsc();
    uint64_t liq = lob.get_liquidity_in_range(10045, 10065);
    uint64_t end = rdtsc();

    std::cout << "Liquidity between 10045 and 10065: " << liq << " shares.\n";
    std::cout << "SIMD Sweep took " << (end - start) << " CPU cycles.\n\n";

    // Benchmark Matching
    start = rdtsc();
    lob.add_order(4, 10060, 600, true);
    end = rdtsc();

    std::cout << "Sweep-to-fill matching took " << (end - start) << " CPU cycles.\n\n";

    // Benchmark Cancellation
    start = rdtsc();
    lob.cancel_order(3);
    end = rdtsc();

    std::cout << "O(1) Cancellation took " << (end - start) << " CPU cycles.\n\n";

    // Now the compiler knows exactly what this means
    wait_for_enter("Press [Enter] to exit");

    return 0;
}