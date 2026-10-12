//
// Created by Danny on 05/06/2026.
//

#pragma once

#include <vector>
#include <cstdint>
#include <array>

#include "constants.h"
#include "Move.h"
#include "Zobrist.h"

namespace Chess {
    class Board {
        friend void getWhitePawnMoves(const Board& board, std::vector<Move>& moves);
        friend void getWhiteRookMoves(const Board& board, std::vector<Move>& moves);
        friend void getWhiteKnightMoves(const Board& board, std::vector<Move>& moves);
        friend void getWhiteBishopMoves(const Board& board, std::vector<Move>& moves);
        friend void getWhiteQueenMoves(const Board& board, std::vector<Move>& moves);
        friend void getWhiteKingMoves(const Board& board, std::vector<Move>& moves);

        friend void getBlackPawnMoves(const Board& board, std::vector<Move>& moves);
        friend void getBlackRookMoves(const Board& board, std::vector<Move>& moves);
        friend void getBlackKnightMoves(const Board& board, std::vector<Move>& moves);
        friend void getBlackBishopMoves(const Board& board, std::vector<Move>& moves);
        friend void getBlackQueenMoves(const Board& board, std::vector<Move>& moves);
        friend void getBlackKingMoves(const Board& board, std::vector<Move>& moves);

    public:
        Board();

        void print() const;
        void printMailbox() const;

        uint64_t getWhitePieces() const;
        uint64_t getBlackPieces() const;
        uint64_t getAllPieces() const;
        uint64_t getPieces(Piece piece) const;
        uint64_t getEmpty() const;

        bool whiteToMove() const;

        bool isSquareAttacked(Square square, Colour colour) const;

        bool isInsufficientMaterial() const;
        bool isThreefoldRepetition() const;

        void removePieceAt(Square square);
        void insertPieceAt(Square square, Piece piece);
        void makeMove(const Move& move);
        void makeMove(std::string_view notation);
        void undoMove(const Move& move);

        Piece pieceAt(Square sq) const;

        Square whiteKingPosition() const;
        Square blackKingPosition() const;

        CastlingRights getCastlingRights() const;

        int getHalfMoveClock() const;

        std::vector<Move> getLegalMoves() const;

        void assertConsistent() const;

        void setPosition(const std::array<Piece, 64>& position);
        void setPosition(const std::array<std::string, 8>& ranks);  // Overload to use strings

        void setEnPassantSquare(Square square);

        void loadFEN(const std::string& fen);

        Zobrist::Hash computeZobristHash() const;

        bool operator==(const Board& other) const;

    private:
        uint64_t whitePawns;
        uint64_t whiteRooks;
        uint64_t whiteKnights;
        uint64_t whiteBishops;
        uint64_t whiteQueens;
        uint64_t whiteKing;

        uint64_t blackPawns;
        uint64_t blackRooks;
        uint64_t blackKnights;
        uint64_t blackBishops;
        uint64_t blackQueens;
        uint64_t blackKing;

        std::array<Piece, 64> board;

        Square enPassantSquare;
        CastlingRights castlingRights;

        std::vector<History> history;

        Colour sideToMove;

        int halfMoveClock;  // Measure time from last capture or pawn push

        Zobrist::Hash zobristHash = 0;

    };

    inline int toIndex(Square sq) {
        return static_cast<int>(sq);
    }
    inline Square shift(Square sq, int delta) {
        return static_cast<Square>(static_cast<int>(sq) + delta);
    }

    constexpr Colour opposite(Colour colour) {
        return colour == Colour::WHITE ? Colour::BLACK : Colour::WHITE;
    }

    inline std::string squareToString(Square square) {
        int index = static_cast<int>(square);

        char file = 'a' + (index % 8);
        char rank = '1' + (index / 8);

        return std::string{file, rank};
    }

    inline std::string moveToString(const Move& move) {
        if (move.from == Square::NO_SQUARE && move.to == Square::NO_SQUARE) {
            return "0000";
        }

        std::string result =
            squareToString(move.from) +
            squareToString(move.to);

        // Add promotion piece, e.g. e7e8q
        if (move.promotionPiece != EMPTY) {
            switch (move.promotionPiece) {
                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    result += 'q';
                    break;

                case WHITE_ROOK:
                case BLACK_ROOK:
                    result += 'r';
                    break;

                case WHITE_BISHOP:
                case BLACK_BISHOP:
                    result += 'b';
                    break;

                case WHITE_KNIGHT:
                case BLACK_KNIGHT:
                    result += 'n';
                    break;

                default:
                    break;
            }
        }

        return result;
    }

    std::uint64_t perft(Board& board, int depth);

    void divide(Board& board, int depth);

}
