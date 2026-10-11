//
// Created by Danny on 11/10/2026.
//

#pragma once

#include <optional>

#include "chess/Move.h"
#include "chess/Board.h"


namespace Chess {
    class Uci {
    public:
        explicit Uci(Board& board);
        void run();

    private:
        Board& board_;

        bool handleCommand(const std::string& line);
        void handlePosition(const std::string& line);
        void handleGo(const std::string& line);
    };

    std::optional<Move> parseUCIMove(const Board& board, std::string_view notation);

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
