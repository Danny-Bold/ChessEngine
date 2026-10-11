//
// Created by Danny on 11/10/2026.
//

#include <iostream>

#include "chess/Board.h"


bool testMakeUndo(int depth, Chess::Board& board)  //  TODO Make other positions to test from
{
    if (depth == 0) {
        return true;
    }

    const std::vector<Chess::Move> moves = board.getLegalMoves();

    for (const Chess::Move& move : moves) {
        const Chess::Board before = board;

        board.makeMove(move);

        const bool descendantsPassed = testMakeUndo(depth - 1, board);

        board.undoMove(move);

        if (board != before) {
            std::cerr << "Board state was not restored after make/unmake.\n";
            return false;
        }

        if (!descendantsPassed) {
            return false;
        }
    }

    return true;
}

int main()
{
    Chess::Board board;

    if (!testMakeUndo(4, board)) {
        std::cerr << "Make/unmake test FAILED.\n";
        return 1;
    }

    std::cout << "Make/unmake test PASSED.\n";
    return 0;
}