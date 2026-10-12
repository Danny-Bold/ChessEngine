//
// Created by Danny on 27/09/2026.
//

#pragma once

#include <vector>

#include "Board.h"
#include "Move.h"

namespace Chess {
    std::vector<Move> generatePseudoWhiteMoves(const Board& board);
    std::vector<Move> generateWhiteMoves(Board& board);
    std::vector<Move> generatePseudoBlackMoves(const Board& board);
    std::vector<Move> generateBlackMoves(Board& board);

    void getWhitePawnMoves(const Board& board, std::vector<Move>& moves);
    void getWhiteKnightMoves(const Board& board, std::vector<Move>& moves);
    void getWhiteBishopMoves(const Board& board, std::vector<Move>& moves);
    void getWhiteRookMoves(const Board& board, std::vector<Move>& moves);
    void getWhiteQueenMoves(const Board& board, std::vector<Move>& moves);
    void getWhiteKingMoves(const Board& board, std::vector<Move>& moves);

    void getBlackPawnMoves(const Board& board, std::vector<Move>& moves);
    void getBlackKnightMoves(const Board& board, std::vector<Move>& moves);
    void getBlackBishopMoves(const Board& board, std::vector<Move>& moves);
    void getBlackRookMoves(const Board& board, std::vector<Move>& moves);
    void getBlackQueenMoves(const Board& board, std::vector<Move>& moves);
    void getBlackKingMoves(const Board& board, std::vector<Move>& moves);
}