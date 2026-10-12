//
// Created by Danny on 13/10/2026.
//


#include <iostream>
#include <cstdint>

#include "chess/perft.h"
#include "chess/Board.h"

namespace Chess {
    std::uint64_t perft(Board& board, int depth) {
        if (depth == 0) {
            return 1;
        }

        const auto moves = board.getLegalMoves();
        std::uint64_t nodes = 0;

        for (auto move : moves) {
            board.makeMove(move);
            nodes += perft(board, depth - 1);
            board.undoMove(move);
        }

        return nodes;
    }

    void divide(Board& board, int depth) {
        auto moves = board.getLegalMoves();

        uint64_t total = 0;

        for (const Move& move : moves) {
            board.makeMove(move);
            uint64_t nodes = perft(board, depth - 1);
            board.undoMove(move);

            std::cout << moveToString(move) << ": " << nodes << '\n';
            total += nodes;
        }

        std::cout << "Total: " << total << '\n';
    }
}
