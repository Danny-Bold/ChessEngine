//
// Created by Danny on 12/10/2026.
//

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace Chess {
    struct GoParameters {
        bool infinite = false;
        bool ponder = false;

        std::optional<int> depth;
        std::optional<int> mate;
        std::optional<uint64_t> nodes;
        std::optional<int> moveTime;

        std::optional<int> wtime;
        std::optional<int> btime;
        std::optional<int> winc;
        std::optional<int> binc;
        std::optional<int> movesToGo;

        std::vector<std::string> searchMoves;
    };
}
