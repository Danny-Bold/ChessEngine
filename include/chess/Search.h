//
// Created by Danny on 11/10/2026.
//

#pragma once

#include <atomic>
#include <chrono>
#include <optional>

#include "chess/Board.h"

namespace Chess {
    using Clock = std::chrono::steady_clock;

    constexpr int INF = 1'000'000;
    constexpr int MATE_SCORE = 100'000;

    struct SearchResult {
        int score;
        bool stopped;
    };

    struct BestMoveResult {
        Move move;
        bool stopped;
    };

    SearchResult negamax(Board& board, int depth, int ply, int alpha, int beta, const std::atomic<bool>& stopRequested, const std::optional<Clock::time_point>& deadline);

    SearchResult searchToDepth(Board& board, int depth, const std::atomic<bool>& stopRequested, std::optional<Clock::time_point> deadline = std::nullopt);

    BestMoveResult findBestMoveDepth(Board& board, int depth, const std::atomic<bool>& stopRequested, std::optional<Move> preferredMove = std::nullopt, std::optional<Clock::time_point> deadline = std::nullopt);

    Move findBestMoveInfinite(Board& board, const std::atomic<bool>& stopRequested, std::optional<Clock::time_point> deadline = std::nullopt);
}