//
// Created by Danny on 11/10/2026.
//

#pragma once

#include "chess/Board.h"

namespace Chess {
    constexpr int INF = 1'000'000;
    constexpr int MATE_SCORE = 100'000;

    int negamax(Board& board, int depth, int ply, int alpha, int beta);

    int searchToDepth(Board& board, int depth);
}