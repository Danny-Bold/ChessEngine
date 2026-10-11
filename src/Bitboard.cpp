//
// Created by Danny on 06/06/2026.
//

#include <iostream>
#include "../include/chess/Bitboard.h"
#include "../include/chess/constants.h"

namespace Chess {
    Square popLSB(uint64_t& bb) {
        int square = std::countr_zero(bb);
        bb &= bb - 1;  // Set the LSB to zero - works because we're passing by reference
        return static_cast<Square>(square);
    }

    Square popMSB(uint64_t& bb) {
        int square = 63 - std::countl_zero(bb);
        bb &= ~(1ULL << square);
        return static_cast<Square>(square);
    }

    Square getLSB(const uint64_t& bb) {
        int square = std::countr_zero(bb);
        return static_cast<Square>(square);
    }

    Square getMSB(const uint64_t& bb) {
        int square = 63 - std::countl_zero(bb);
        return static_cast<Square>(square);
    }

    void printBitboard(uint64_t bb)
    {
        for (int rank = 7; rank >= 0; rank--)
        {
            std::cout << rank + 1 << " ";

            for (int file = 0; file < 8; file++)
            {
                int square = rank * 8 + file;

                if (bb & (1ULL << square))
                {
                    std::cout << " X";
                }
                else
                {
                    std::cout << " .";
                }
            }

            std::cout << '\n';
        }

        std::cout << "   a b c d e f g h\n";
    }
}