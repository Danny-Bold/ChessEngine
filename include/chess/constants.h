//
// Created by Danny on 05/06/2026.
//

#pragma once

#include <cstdint>
#include <string>

namespace Chess {
    using CastlingRights = uint8_t;

    constexpr CastlingRights WK = 1;
    constexpr CastlingRights WQ = 2;
    constexpr CastlingRights BK = 4;
    constexpr CastlingRights BQ = 8;

    constexpr uint64_t notAFile = 0xfefefefefefefefeULL;
    constexpr uint64_t notHFile = 0x7f7f7f7f7f7f7f7fULL;

    constexpr uint64_t rank1 = 0x00000000000000FFULL;
    constexpr uint64_t rank2 = 0x000000000000FF00ULL;
    constexpr uint64_t rank7 = 0x00FF000000000000ULL;
    constexpr uint64_t rank8 = 0xFF00000000000000ULL;

    enum class Colour : uint8_t {
        WHITE,
        BLACK
    };

    enum class Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    NO_SQUARE = 255
    };

    enum Piece : int {
    EMPTY,
    WHITE_PAWN,
    WHITE_ROOK,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_QUEEN,
    WHITE_KING,

    BLACK_PAWN,
    BLACK_ROOK,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_QUEEN,
    BLACK_KING};

    inline char pieceToChar(Piece piece) {
        switch (piece) {
            case WHITE_PAWN:   return 'P';
            case WHITE_ROOK:   return 'R';
            case WHITE_KNIGHT: return 'N';
            case WHITE_BISHOP: return 'B';
            case WHITE_QUEEN:  return 'Q';
            case WHITE_KING:   return 'K';

            case BLACK_PAWN:   return 'p';
            case BLACK_ROOK:   return 'r';
            case BLACK_KNIGHT: return 'n';
            case BLACK_BISHOP: return 'b';
            case BLACK_QUEEN:  return 'q';
            case BLACK_KING:   return 'k';

            case EMPTY:        return '.';
        }

        return '?';
    }
}