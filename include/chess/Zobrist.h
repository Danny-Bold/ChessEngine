//
// Created by Danny on 12/10/2026.
//

#pragma once
#include <cstdint>


namespace Chess::Zobrist {

    using Hash = std::uint64_t;

    extern std::array<std::array<Hash, 64>, 12> pieceSquare;

    // Side to move, castling rights and en passant.
    extern Hash sideToMove;
    extern std::array<Hash, 16> castlingRights;
    extern std::array<Hash, 8> enPassantFile;

    void initialiseZobristHashes();
}
