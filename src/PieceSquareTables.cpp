//
// Created by Danny on 11/10/2026.
//


#include "chess/constants.h"
#include "chess/Evaluation.h"

namespace Chess::Evaluation {
namespace {

    // Each row represents one rank, from rank 1 to rank 8.
    // Each row stores files A-D; the helper mirrors them to E-H.
    using HalfTable = std::array<std::array<int, 4>, 8>;
    constexpr PieceSquareTable makeSymmetricTable(const HalfTable& half) {
        PieceSquareTable table{};

        for (std::size_t rank = 0; rank < 8; ++rank) {
            for (std::size_t file = 0; file < 8; ++file) {
                const std::size_t sourceFile =
                    std::min(file, 7 - file);

                table[rank * 8 + file] = half[rank][sourceFile];
            }
        }

        return table;
    }
} // namespace

    // ============================================================
    // PAWNS
    // ============================================================
    const PieceSquareTable PAWN_MG = makeSymmetricTable({{
        {{  0,   0,   0,   0}},  // Rank 1
        {{  5,  10,  10, -20}},  // Rank 2
        {{  5,  -5, -10,   0}},  // Rank 3
        {{  0,   0,   0,  20}},  // Rank 4
        {{  5,   5,  10,  25}},  // Rank 5
        {{ 10,  10,  20,  30}},  // Rank 6
        {{ 50,  50,  50,  50}},  // Rank 7
        {{  0,   0,   0,   0}}   // Rank 8
    }});

    const PieceSquareTable PAWN_EG = makeSymmetricTable({{
        {{  0,   0,   0,   0}},
        {{ 10,  10,  10,  10}},
        {{ 15,  15,  20,  20}},
        {{ 20,  20,  30,  30}},
        {{ 30,  30,  40,  40}},
        {{ 50,  50,  60,  60}},
        {{ 80,  80,  90,  90}},
        {{  0,   0,   0,   0}}
    }});

    // ============================================================
    // KNIGHTS
    // ============================================================

    const PieceSquareTable KNIGHT_MG = makeSymmetricTable({{
        {{-50, -40, -30, -30}},
        {{-40, -20,   0,   5}},
        {{-30,   5,  10,  15}},
        {{-30,   0,  15,  20}},
        {{-30,   5,  15,  20}},
        {{-30,   0,  10,  15}},
        {{-40, -20,   0,   5}},
        {{-50, -40, -30, -30}}
    }});

    const PieceSquareTable KNIGHT_EG = makeSymmetricTable({{
        {{-40, -30, -20, -20}},
        {{-30, -10,   0,   5}},
        {{-20,   5,  10,  15}},
        {{-20,   0,  15,  20}},
        {{-20,   5,  15,  20}},
        {{-20,   0,  10,  15}},
        {{-30, -10,   0,   5}},
        {{-40, -30, -20, -20}}
    }});

    // ============================================================
    // BISHOPS
    // ============================================================

    const PieceSquareTable BISHOP_MG = makeSymmetricTable({{
        {{-20, -10, -10, -10}},
        {{-10,   5,   0,   0}},
        {{-10,  10,  10,  10}},
        {{-10,   0,  10,  10}},
        {{-10,   5,   5,  10}},
        {{-10,   0,   5,  10}},
        {{-10,   0,   0,   0}},
        {{-20, -10, -10, -10}}
    }});

    const PieceSquareTable BISHOP_EG = makeSymmetricTable({{
        {{-20, -10, -10, -10}},
        {{-10,   5,   0,   0}},
        {{-10,  10,  10,  10}},
        {{-10,   0,  10,  10}},
        {{-10,   5,   5,  10}},
        {{-10,   0,   5,  10}},
        {{-10,   0,   0,   0}},
        {{-20, -10, -10, -10}}
    }});

    // ============================================================
    // ROOKS
    // ============================================================

    const PieceSquareTable ROOK_MG = makeSymmetricTable({{
        {{  0,   0,   5,  10}},
        {{  5,  10,  10,  10}},
        {{ -5,   0,   0,   0}},
        {{ -5,   0,   0,   0}},
        {{ -5,   0,   0,   0}},
        {{ -5,   0,   0,   0}},
        {{  5,  10,  10,  10}},
        {{  0,   0,   5,  10}}
    }});

    const PieceSquareTable ROOK_EG = makeSymmetricTable({{
        {{  0,   0,   5,  10}},
        {{  5,  10,  10,  10}},
        {{ -5,   0,   0,   0}},
        {{ -5,   0,   0,   0}},
        {{ -5,   0,   0,   0}},
        {{ -5,   0,   0,   0}},
        {{  5,  10,  10,  10}},
        {{  0,   0,   5,  10}}
    }});

    // ============================================================
    // QUEENS
    // ============================================================

    const PieceSquareTable QUEEN_MG = makeSymmetricTable({{
        {{-20, -10, -10,  -5}},
        {{-10,   0,   0,   0}},
        {{-10,   0,   5,   5}},
        {{ -5,   0,   5,   5}},
        {{ -5,   0,   5,   5}},
        {{-10,   5,   5,   5}},
        {{-10,   0,   5,   0}},
        {{-20, -10, -10,  -5}}
    }});

    const PieceSquareTable QUEEN_EG = makeSymmetricTable({{
        {{-20, -10, -10,  -5}},
        {{-10,   0,   0,   0}},
        {{-10,   0,   5,   5}},
        {{ -5,   0,   5,   5}},
        {{ -5,   0,   5,   5}},
        {{-10,   5,   5,   5}},
        {{-10,   0,   5,   0}},
        {{-20, -10, -10,  -5}}
    }});

    // ============================================================
    // KINGS
    // ============================================================

    // Middlegame: prefer safety and castled positions.
    const PieceSquareTable KING_MG = makeSymmetricTable({{
        {{ 20,  30,  10,   0}},
        {{ 20,  20,   0,   0}},
        {{-10, -20, -20, -20}},
        {{-20, -30, -30, -40}},
        {{-30, -40, -40, -50}},
        {{-30, -40, -40, -50}},
        {{-30, -40, -40, -50}},
        {{-30, -40, -40, -50}}
    }});

    // Endgame: centralisation becomes valuable.
    const PieceSquareTable KING_EG = makeSymmetricTable({{
        {{-50, -30, -30, -30}},
        {{-30, -10,   0,   0}},
        {{-30, -10,  20,  30}},
        {{-30, -10,  30,  40}},
        {{-30, -10,  30,  40}},
        {{-30, -10,  20,  30}},
        {{-30, -30,   0,   0}},
        {{-50, -30, -30, -30}}
    }});
}
