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
    };

    std::optional<Move> parseUCIMove(const Board& board, std::string_view notation);
}
