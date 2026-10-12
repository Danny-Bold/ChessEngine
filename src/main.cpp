#include "chess/Board.h"
#include "chess/UCI.h"


int main() {

    // std::cout << "Hello world from the Chess Engine!\n";

    // Chess::Board board;

    // for (int depth = 6; depth <= 6; depth++) {
    //     std::cout << "DEPTH = " << depth << ": " << perft(depth, board) << "\n";
    // }

    // board.loadFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");

    // std::cout << "KIWIPETE, DEPTH = 4: " << perft(4, board) << "\n";

    // board.assertConsistent()

    // board.makeMove("a2a3");
    // board.makeMove("b4b3");
    // board.makeMove("g2g3");
    // divide(board, 2);

    Chess::Board board;
    Chess::Uci uci(board);

    uci.run();

    return 0;
}

