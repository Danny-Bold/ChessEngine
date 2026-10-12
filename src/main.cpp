#include "chess/Board.h"
#include "chess/UCI.h"


int main() {  // TODO Consider adding intro line/credits etc
    Chess::Board board;
    Chess::Uci uci(board);

    uci.run();

    return 0;
}

