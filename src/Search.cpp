//
// Created by Danny on 11/10/2026.
//

#include <algorithm>
#include <limits>

#include "chess/Search.h"
#include "chess/Evaluation.h"

namespace Chess {
    SearchResult negamax(Board& board, int depth, int ply, int alpha, int beta, const std::atomic<bool>& stopRequested) {
        if (stopRequested.load(std::memory_order_relaxed)) {
            return {0, true};  // Respect the stop call, and stop before doing any legal move generation
        }

        if (depth == 0) {
            return {Evaluation::evaluate(board), false};
        }

        std::vector<Move> legalMoves = board.getLegalMoves();

        if (board.isInsufficientMaterial() || board.getHalfMoveClock() >= 100) {  // TODO threefold repetition
            return {0, false};
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
                return {-MATE_SCORE + ply, false};
            }

            return {0, false};
        }

        int bestScore = -INF;

        for (const Move& move : legalMoves) {
            board.makeMove(move);

            SearchResult child = negamax(board, depth - 1, ply + 1, -beta, -alpha, stopRequested);

            board.undoMove(move);

            if (child.stopped) {
                return {0, true};
            }

            int score = -child.score;

            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, score);

            if (alpha >= beta) {
                break;
            }
        }

        return {bestScore, false};
    }

    SearchResult searchToDepth(Board& board, int depth, const std::atomic<bool>& stopRequested) {
        int alpha = -INF;
        int beta = INF;

        return negamax(board, depth, 1, alpha, beta, stopRequested);
    }

    BestMoveResult findBestMoveDepth(Board& board, int depth, const std::atomic<bool>& stopRequested, std::optional<Move> preferredMove) {
        const Move noMove(
            Square::NO_SQUARE,
            Square::NO_SQUARE,
            EMPTY, EMPTY, EMPTY,
            false, false, false
        );

        if (stopRequested.load(std::memory_order_relaxed)) {
            return {noMove, true};
        }

        std::vector<Move> legalMoves = board.getLegalMoves();

        if (legalMoves.empty()) {
            return {noMove, false};
        }

        if (preferredMove.has_value()) {
            auto it = std::find(legalMoves.begin(), legalMoves.end(), preferredMove.value());

            if (it != legalMoves.end()) {
                std::iter_swap(legalMoves.begin(), it);
            }
        }

        Move bestMove = legalMoves.front();
        int bestScore = -INF;

        int alpha = -INF;
        const int beta = INF;

        for (const Move& move : legalMoves) {
            if (stopRequested.load(std::memory_order_relaxed)) {
                return {bestMove, true};
            }

            board.makeMove(move);

            SearchResult result = negamax(
                board,
                depth - 1,
                1,
                -beta,
                -alpha,
                stopRequested
            );

            board.undoMove(move);

            if (result.stopped) {
                return {bestMove, true};
            }

            const int score = -result.score;

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }

            alpha = std::max(alpha, score);
        }

        return {bestMove, false};
    }

    Move findBestMoveInfinite(Board& board, const std::atomic<bool>& stopRequested) {
        int depth = 1;

        std::vector<Move> allMoves = board.getLegalMoves();

        if (allMoves.empty()) {
            return {
                Square::NO_SQUARE,
                Square::NO_SQUARE,
                EMPTY, EMPTY, EMPTY,
                false, false, false
            };
        }

        Move bestMove = allMoves.front();
        std::optional<Move> preferredMove = std::nullopt;

        while (!stopRequested.load(std::memory_order_relaxed)) {
            BestMoveResult result = findBestMoveDepth(board, depth, stopRequested, preferredMove);

            if (result.stopped) {
                break;
            }

            bestMove = result.move;
            preferredMove = bestMove;
            ++depth;
        }

        return bestMove;
    }
}