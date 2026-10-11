#include <iostream>
#include <cassert>

#include "chess/AttackTables.h"
#include "chess/Board.h"
#include "chess/MoveGen.h"


int perft(int depth, Chess::Board& board) {
    if (depth == 0) {
        return 1;
    }

    std::vector<Chess::Move> moves = board.getLegalMoves();

    int nodes = 0;

    for (Chess::Move& move : moves) {
        board.makeMove(move);
        board.assertConsistent();
        nodes += perft(depth - 1, board);
        board.undoMove(move);
        board.assertConsistent();
    }

    return nodes;
}

void divide(Chess::Board& board, int depth) {
    auto moves = board.getLegalMoves();

    uint64_t total = 0;

    for (const Chess::Move& move : moves) {
        board.makeMove(move);
        uint64_t nodes = perft(depth - 1, board);
        board.undoMove(move);

        std::cout << moveToString(move) << ": " << nodes << '\n';
        total += nodes;
    }

    std::cout << "Total: " << total << '\n';
}

void testMakeUndo(int depth, Chess::Board& board) {
    if (depth == 0) {
        return;
    }

    std::vector<Chess::Move> moves = board.getLegalMoves();

    for (Chess::Move& move : moves) {
        // Snapshot the complete board state before making the move.
        Chess::Board before = board;

        board.makeMove(move);
        board.assertConsistent();

        // Test every descendant before undoing this move.
        testMakeUndo(depth - 1, board);

        board.undoMove(move);
        board.assertConsistent();

        // Replace this with your actual board-state comparison.
        assert(board == before);
    }
}

int main() {

    // std::cout << "Hello world from the Chess Engine!\n";

    Chess::Board board;

    // for (int depth = 6; depth <= 6; depth++) {
    //     std::cout << "DEPTH = " << depth << ": " << perft(depth, board) << "\n";
    // }

    board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    std::cout << "KIWIPETE, DEPTH = 4: " << perft(4, board) << "\n";

    // board.assertConsistent()

    // board.makeMove(Chess::Move(Chess::Square::A2, Chess::Square::A3, Chess::WHITE_PAWN, Chess::EMPTY, Chess::EMPTY, false, false, false));
    // board.makeMove(Chess::Move(Chess::Square::B4, Chess::Square::B3, Chess::BLACK_PAWN, Chess::EMPTY, Chess::EMPTY, false, false, false));
    // board.makeMove(Chess::Move(Chess::Square::G2, Chess::Square::G3, Chess::WHITE_PAWN, Chess::EMPTY, Chess::EMPTY, false, false, false));
    // board.makeMove(Chess::Move(Chess::Square::D8, Chess::Square::A5, Chess::BLACK_QUEEN, Chess::EMPTY, Chess::EMPTY, false, false, false));
    // board.print();
    // divide(board, 4);

}

