//
// Created by Danny on 11/10/2026.
//

#include <algorithm>
#include <limits>

#include "chess/Search.h"
#include "chess/Evaluation.h"

namespace Chess {
    int negamax(Board& board, int depth, int ply, int alpha, int beta) {
        std::vector<Move> legalMoves = board.getLegalMoves();

        if (board.isInsufficientMaterial() || board.getHalfMoveClock() >= 100) {  // TODO threefold repetition
            return 0;
        }

        if (legalMoves.empty()) {
            // Either checkmate or stalemate. Check which.
            bool checkmate;

            if (board.whiteToMove()) {
                checkmate = board.isSquareAttacked(board.whiteKingPosition(), Colour::BLACK);
            }
            else {
                checkmate = board.isSquareAttacked(board.blackKingPosition(), Colour::WHITE);
            }

            if (checkmate) {
                return -MATE_SCORE + ply;
            }

            return 0;
        }

        if (depth == 0) {
            return Evaluation::evaluate(board);
        }

        int bestScore = -INF;

        for (const Move& move : legalMoves) {
            board.makeMove(move);

            int score = -negamax(board, depth - 1, ply + 1, -beta, -alpha);

            board.undoMove(move);

            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);

            if (alpha >= beta) {
                break;
            }
        }

        return bestScore;
    }

    int searchToDepth(Board& board, int depth) {
        int alpha = -INF;
        int beta = INF;

        return negamax(board, depth, 1, alpha, beta);
    }

    Move findBestMoveDepth(Board& board, int depth) {
        int bestScore = -INF;
        Move bestMove = Chess::Move(Square::NO_SQUARE, Square::NO_SQUARE, EMPTY, EMPTY, EMPTY, false, false, false);

        std::vector<Move> legalMoves = board.getLegalMoves();

        for (const Move& move : legalMoves) {
            board.makeMove(move);
            int score = -searchToDepth(board, depth - 1);  // Negate because one move deep implies the enemy is moving, so scores are returned from their perspective

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            board.undoMove(move);
        }

        return bestMove;
    }
}