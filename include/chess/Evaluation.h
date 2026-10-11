//
// Created by Danny on 11/10/2026.
//

#pragma once

#include "chess/Board.h"

namespace Chess::Evaluation {

    const int pawnValue = 100;
    const int knightValue = 320;
    const int bishopValue = 330;
    const int rookValue = 500;
    const int queenValue = 500;

    using PieceSquareTable = std::array<int, 64>;

    extern const PieceSquareTable PAWN_MG;
    extern const PieceSquareTable KNIGHT_MG;
    extern const PieceSquareTable BISHOP_MG;
    extern const PieceSquareTable ROOK_MG;
    extern const PieceSquareTable QUEEN_MG;
    extern const PieceSquareTable KING_MG;

    extern const PieceSquareTable PAWN_EG;
    extern const PieceSquareTable KNIGHT_EG;
    extern const PieceSquareTable BISHOP_EG;
    extern const PieceSquareTable ROOK_EG;
    extern const PieceSquareTable QUEEN_EG;
    extern const PieceSquareTable KING_EG;

    constexpr int MAX_GAME_PHASE = 24;

    int getGamePhase(const Board& board);

    int getMaterialValue(const Board& board);

    using PositionalScores = std::array<int, 2>;
    PositionalScores getPositionalCorrection(const Board& board);

    int evaluate(const Board& board);

    constexpr Square flipVertical(Square square) {
        return static_cast<Square>(
            static_cast<int>(square) ^ 56
        );
    }

}