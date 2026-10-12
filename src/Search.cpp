//
// Created by Danny on 11/10/2026.
//

#include <algorithm>
#include <limits>
#include <optional>
#include <chrono>

#include "chess/Search.h"
#include "chess/Evaluation.h"

namespace Chess {
    using Clock = std::chrono::steady_clock;

    SearchResult negamax(Board& board, int depth, int ply, int alpha, int beta, const std::atomic<bool>& stopRequested, SearchTiming& timing) {
        if (stopRequested.load(std::memory_order_relaxed)) {
            return {0, true};  // Respect the stop call, stop before doing any legal move generation
        }

        if (isTimeUp(timing)) {
            return {0, true};
        }

        if (board.isInsufficientMaterial() || board.getHalfMoveClock() >= 100 || board.isThreefoldRepetition()) {
            return {0, false};
        }

        if (depth == 0) {
            return {Evaluation::evaluate(board), false};
        }

        std::vector<Move> legalMoves = board.getLegalMoves();

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

            SearchResult child = negamax(board, depth - 1, ply + 1, -beta, -alpha, stopRequested, timing);

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

    SearchResult searchToDepth(Board& board, int depth, const std::atomic<bool>& stopRequested, SearchTiming& timing) {
        int alpha = -INF;
        int beta = INF;

        return negamax(board, depth, 1, alpha, beta, stopRequested, timing);
    }

    BestMoveResult findBestMoveDepth(Board& board, int depth, const std::atomic<bool>& stopRequested, std::optional<Move> preferredMove, SearchTiming& timing) {
        const Move noMove(
            Square::NO_SQUARE,
            Square::NO_SQUARE,
            EMPTY, EMPTY, EMPTY,
            false, false, false
        );

        if (stopRequested.load(std::memory_order_relaxed)) {
            return {noMove, true};
        }

        if (isTimeUp(timing)) {
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

            if (isTimeUp(timing)) {
                return {noMove, true};
            }

            board.makeMove(move);

            SearchResult result = negamax(
                board,
                depth - 1,
                1,
                -beta,
                -alpha,
                stopRequested,
                timing
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

    Move findBestMoveInfinite(Board& board, const std::atomic<bool>& stopRequested, SearchTiming& timing) {
        const Move noMove(
            Square::NO_SQUARE,
            Square::NO_SQUARE,
            EMPTY, EMPTY, EMPTY,
            false, false, false
        );

        int depth = 1;

        std::vector<Move> allMoves = board.getLegalMoves();

        if (allMoves.empty()) {
            return noMove;
        }

        Move bestMove = allMoves.front();
        std::optional<Move> preferredMove = std::nullopt;

        while (!stopRequested.load(std::memory_order_relaxed)) {
            BestMoveResult result = findBestMoveDepth(board, depth, stopRequested, preferredMove, timing);

            if (result.stopped) {
                break;
            }

            bestMove = result.move;
            preferredMove = bestMove;
            ++depth;
        }

        return bestMove;
    }
    bool isTimeUp(SearchTiming& timing) {
        std::lock_guard lock(timing.mutex);

        if (timing.pondering || !timing.deadline.has_value()) {
            return false;
        }

        return Clock::now() >= *timing.deadline;
    }

    std::optional<std::chrono::milliseconds> calculateTimeBudget(const GoParameters& params, bool whiteToMove) {
        using namespace std::chrono;

        // Explicit time limit takes priority.
        if (params.moveTime.has_value()) {
            return milliseconds(std::max(0, *params.moveTime));
        }

        // Select the clock and increment for the player to move.
        const auto remaining = whiteToMove ? params.wtime : params.btime;
        const auto increment = whiteToMove ? params.winc : params.binc;

        // No clock information means we cannot calculate a budget.
        if (!remaining.has_value()) {
            return std::nullopt;
        }

        const auto remainingMs = std::max(0, *remaining);
        const auto incrementMs = increment.value_or(0);
        const auto movesToGo = params.movesToGo.value_or(30);

        if (movesToGo <= 0 || remainingMs == 0) {
            return milliseconds(0);
        }

        const auto nominal =
            remainingMs / movesToGo + 4 * incrementMs / 5;

        const auto budget = std::min(
            9 * nominal / 10,
            remainingMs / 5
        );

        return milliseconds(std::max(1, budget));
    }
}
