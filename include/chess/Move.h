//
// Created by Danny on 05/06/2026.
//

#pragma once

#include "constants.h"

namespace Chess {
    struct Move {
        Square from;
        Square to;

        Piece movingPiece;
        Piece capturedPiece;

        Piece promotionPiece;
        bool isEnPassant;
        bool isCastle;
        bool isDoublePawnPush;

        bool operator==(const Move& other) const = default;

        Move(
            Square from,
            Square to,
            Piece movingPiece,
            Piece capturedPiece = EMPTY,
            Piece promotionPiece = EMPTY,
            bool isEnPassant = false,
            bool isCastle = false,
            bool isDoublePawnPush = false
        )
            : from(from),
              to(to),
              movingPiece(movingPiece),
              capturedPiece(capturedPiece),
              promotionPiece(promotionPiece),
              isEnPassant(isEnPassant),
              isCastle(isCastle),
              isDoublePawnPush(isDoublePawnPush) {}
        };

    struct History {
        Square enPassantSquare;
        CastlingRights castlingRights;
        int halfMoveClock;
    };
}