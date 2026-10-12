//
// Created by Danny on 13/10/2026.
//

#pragma once

#include <cstdint>

#include "chess/perft.h"
#include "chess/Board.h"

namespace Chess {
    std::uint64_t perft(Board& board, int depth);
    void divide(Board& board, int depth);
}