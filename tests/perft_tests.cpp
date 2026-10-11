//
// Created by Danny on 10/10/2026.
//


#include <array>
#include <cstdint>
#include <iostream>

#include "chess/Board.h"


int main() {  // TODO Make other positions to test from
    using namespace Chess;

    Board board; // Standard starting position

    constexpr std::array<std::uint64_t, 5> expected = {
        0,        // Depth 0 (unused here)
        20,       // Depth 1
        400,      // Depth 2
        8902,     // Depth 3
        197281    // Depth 4
    };

    bool passed = true;

    for (int depth = 1; depth <= 4; ++depth) {
        const auto actual = perft(board, depth);
        const auto reference = expected[depth];

        const bool depthPassed = actual == reference;
        passed = passed && depthPassed;

        std::cout
            << "Depth " << depth
            << ": expected " << reference
            << ", got " << actual
            << " [" << (depthPassed ? "PASS" : "FAIL") << "]\n";
    }

    if (!passed) {
        std::cerr << "Perft regression test FAILED.\n";
        return 1;
    }

    std::cout << "All perft tests passed.\n";
    return 0;
}