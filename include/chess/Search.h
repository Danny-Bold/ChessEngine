//
// Created by Danny on 11/10/2026.
//

#pragma once

#include <atomic>
#include <chrono>
#include <optional>
#include <mutex>

#include "chess/GoParameters.h"
#include "chess/Board.h"

namespace Chess {
    using Clock = std::chrono::steady_clock;

    constexpr int INF = 1'000'000;
    constexpr int MATE_SCORE = 100'000;

    struct SearchResult {
        int score;
        bool stopped;
        std::vector<Move> principalVariation;
    };

    struct BestMoveResult {
        Move move;
        bool stopped;
        std::vector<Move> principalVariation;
        int depth = 0;
    };

    struct SearchTiming {
        std::mutex mutex;
        bool pondering = false;
        std::optional<Clock::time_point> deadline;
    };

    SearchResult negamax(Board& board, int depth, int ply, int alpha, int beta, const std::atomic<bool>& stopRequested, SearchTiming& timing);

    SearchResult searchToDepth(Board& board, int depth, const std::atomic<bool>& stopRequested, SearchTiming& timing);

    BestMoveResult findBestMoveDepth(Board& board, int depth, const std::atomic<bool>& stopRequested, std::optional<Move> preferredMove, SearchTiming& timing);

    BestMoveResult findBestMoveInfinite(Board& board, const std::atomic<bool>& stopRequested, SearchTiming& timing);

    bool isTimeUp(SearchTiming& timing);

    std::optional<std::chrono::milliseconds> calculateTimeBudget(const GoParameters& params, bool whiteToMove);
}