//
// Created by Danny on 06/06/2026.
//

#include "../include/chess/AttackTables.h"

#include <cstdint>

namespace Chess {
    uint64_t whitePawnAttacks[64];
    uint64_t blackPawnAttacks[64];

    uint64_t knightAttacks[64];

    uint64_t bishopNE[64];
    uint64_t bishopSE[64];
    uint64_t bishopNW[64];
    uint64_t bishopSW[64];

    uint64_t rookN[64];
    uint64_t rookE[64];
    uint64_t rookS[64];
    uint64_t rookW[64];

    uint64_t kingAttacks[64];

    constexpr int knightRankOffsets[8] =
        {
        -2, -2, -1, -1, 1, 1, 2, 2
    };

    constexpr int knightFileOffsets[8] = {
        -1, 1, -2, 2, -2, 2, -1, 1
    };

    constexpr int kingFileOffsets[8] = {  // East, going clockwise
        1, 1, 0, -1, -1, -1, 0, 1
    };

    constexpr int kingRankOffsets[8] {
        0, -1, -1, -1, 0, 1, 1, 1
    };

    void initialiseAttackTables()
    {
        static const bool initialised = [] {
            initialise();
            return true;
        }();

        (void)initialised;
    }

    void initialise() {
        initialisePawnAttacks();  // TODO This was created after pawn move generation. Refactor/simplify?
        initialiseKnightAttacks();
        initialiseBishopAttacks();
        initialiseRookAttacks();
        initialiseKingAttacks();
    }

    void initialisePawnAttacks() {
        for (int square = 0; square < 64; square++) {
            int rank = square / 8;
            int file = square % 8;

            // White pawn attacking squares first: +7, +9

            int whiteAttackRank = rank + 1;
            int blackAttackRank = rank - 1;
            int rightAttackFile = file + 1;
            int leftAttackFile = file - 1;

            if (0 <= whiteAttackRank && whiteAttackRank < 8 && 0 <= leftAttackFile && leftAttackFile < 8) {
                whitePawnAttacks[square] |= (1ULL << (square + 7));
            }

            if (0 <= whiteAttackRank && whiteAttackRank < 8 && 0 <= rightAttackFile && rightAttackFile < 8) {
                whitePawnAttacks[square] |= (1ULL << (square + 9));
            }

            if (0 <= blackAttackRank && blackAttackRank < 8 && 0 <= leftAttackFile && leftAttackFile < 8) {
                blackPawnAttacks[square] |= (1ULL << (square - 7));
            }

            if (0 <= blackAttackRank && blackAttackRank < 8 && 0 <= rightAttackFile && rightAttackFile < 8) {
                blackPawnAttacks[square] |= (1ULL << (square - 9));
            }
        }
    }

    void initialiseKnightAttacks() {
        for (int square = 0; square < 64; square++) {
            knightAttacks[square] = 0;

            int rank = square / 8;
            int file = square % 8;

            for (int i = 0; i < 8; i++) {
                int targetRank = rank + knightRankOffsets[i];
                int targetFile = file + knightFileOffsets[i];

                // Check both rank and file are valid (0-7 inclusive):

                if (0 <= targetFile && targetFile < 8 && 0 <= targetRank && targetRank < 8) {
                    int targetSquare = targetRank * 8 + targetFile;
                    knightAttacks[square] |= (1ULL << targetSquare);
                }
            }
        }
    }

    void initialiseBishopAttacks() {
        for (int square = 0; square < 64; square++) {
            bishopNE[square] = 0;
            bishopSE[square] = 0;
            bishopNW[square] = 0;
            bishopSW[square] = 0;

            int rank = square / 8;
            int file = square % 8;

            for (int i = 1; i < 8; i++) {  // i represents distance. Maximum 7 moves, cannot stay on same square.
                // NE first - (+1, +1) direction)
                int targetRankNE = rank + i;
                int targetFileNE = file + i;

                if (0 <= targetFileNE && targetFileNE < 8 && 0 <= targetRankNE && targetRankNE < 8) {
                    int targetSquare = targetRankNE * 8 + targetFileNE;
                    bishopNE[square] |= (1ULL << targetSquare);
                }

                // SE - (-1, +1)

                int targetRankSE = rank - i;
                int targetFileSE = file + i;

                if (0 <= targetFileSE && targetFileSE < 8 && 0 <= targetRankSE && targetRankSE < 8) {
                    int targetSquare = targetRankSE * 8 + targetFileSE;
                    bishopSE[square] |= (1ULL << targetSquare);
                }

                // NW - (+1, -1)

                int targetRankNW = rank + i;
                int targetFileNW = file - i;

                if (0 <= targetFileNW && targetFileNW < 8 && 0 <= targetRankNW && targetRankNW < 8) {
                    int targetSquare = targetRankNW * 8 + targetFileNW;
                    bishopNW[square] |= (1ULL << targetSquare);
                }

                // SW - (-1, -1)

                int targetRankSW = rank - i;
                int targetFileSW = file - i;

                if (0 <= targetFileSW && targetFileSW < 8 && 0 <= targetRankSW && targetRankSW < 8) {
                    int targetSquare = targetRankSW * 8 + targetFileSW;
                    bishopSW[square] |= (1ULL << targetSquare);
                }
            }
        }
    }

    void initialiseRookAttacks() {
        for (int square = 0; square < 64; square++) {
            rookN[square] = 0;
            rookE[square] = 0;
            rookS[square] = 0;
            rookW[square] = 0;

            int rank = square / 8;
            int file = square % 8;

            for (int i = 1; i < 8; i++) {

                // North - positive rank, don't adjust file

                int targetRankN = rank + i;

                if (targetRankN < 8) {
                    int targetSquareN = targetRankN * 8 + file;
                    rookN[square] |= (1ULL << targetSquareN);
                }

                // East - positive file, don't adjust rank

                int targetFileE = file + i;

                if (targetFileE < 8) {
                    int targetSquareE = rank * 8 + targetFileE;
                    rookE[square] |= (1ULL << targetSquareE);
                }

                // South - negative rank, don't adjust file

                int targetRankS = rank - i;

                if (0 <= targetRankS) {
                    int targetSquareS = targetRankS * 8 + file;
                    rookS[square] |= (1ULL << targetSquareS);
                }

                // West - negative file, don't adjust rank

                int targetFileW = file - i;

                if (0 <= targetFileW) {
                    int targetSquareW = rank * 8 + targetFileW;
                    rookW[square] |= (1ULL << targetSquareW);
                }
            }
        }
    }

    void initialiseKingAttacks() {
        for (int square = 0; square < 64; square++) {
            kingAttacks[square] = 0;

            int rank = square / 8;
            int file = square % 8;

            for (int i = 0; i < 8; i++) {
                int targetRank = rank + kingRankOffsets[i];
                int targetFile = file + kingFileOffsets[i];

                // Check both rank and file are valid (0-7 inclusive):

                if (0 <= targetFile && targetFile < 8 && 0 <= targetRank && targetRank < 8) {
                    int targetSquare = targetRank * 8 + targetFile;
                    kingAttacks[square] |= (1ULL << targetSquare);
                }
            }
        }
    }
}
