//
// Created by Danny on 11/10/2026.
//

#include <bit>
#include <cstdint>

#include "chess/Evaluation.h"

#include <stdexcept>


namespace Chess::Evaluation {
    namespace {

        struct PieceTables {
            const PieceSquareTable* mg;
            const PieceSquareTable* eg;
        };

        PieceTables getTables(Piece piece) {
            switch (piece) {
                case WHITE_PAWN:
                case BLACK_PAWN:
                    return {&PAWN_MG, &PAWN_EG};

                case WHITE_KNIGHT:
                case BLACK_KNIGHT:
                    return {&KNIGHT_MG, &KNIGHT_EG};

                case WHITE_BISHOP:
                case BLACK_BISHOP:
                    return {&BISHOP_MG, &BISHOP_EG};

                case WHITE_ROOK:
                case BLACK_ROOK:
                    return {&ROOK_MG, &ROOK_EG};

                case WHITE_QUEEN:
                case BLACK_QUEEN:
                    return {&QUEEN_MG, &QUEEN_EG};

                case WHITE_KING:
                case BLACK_KING:
                    return {&KING_MG, &KING_EG};

                default:
                    throw std::invalid_argument("Invalid piece");
            }
        }

        constexpr std::array<Piece, 12> ALL_PIECES = {
            WHITE_PAWN, WHITE_KNIGHT, WHITE_BISHOP,
            WHITE_ROOK, WHITE_QUEEN, WHITE_KING,
            BLACK_PAWN, BLACK_KNIGHT, BLACK_BISHOP,
            BLACK_ROOK, BLACK_QUEEN, BLACK_KING
        };

        constexpr bool isWhite(Piece piece) {
            switch (piece) {
                case WHITE_PAWN:
                case WHITE_KNIGHT:
                case WHITE_BISHOP:
                case WHITE_ROOK:
                case WHITE_QUEEN:
                case WHITE_KING:
                    return true;

                default:
                    return false;
            }
        }
    }

    int getGamePhase(const Board& board) {
        uint64_t whiteKnights = board.getPieces(WHITE_KNIGHT);
        uint64_t whiteBishops = board.getPieces(WHITE_BISHOP);
        uint64_t whiteRooks = board.getPieces(WHITE_ROOK);
        uint64_t whiteQueens = board.getPieces(WHITE_QUEEN);

        uint64_t blackKnights = board.getPieces(BLACK_KNIGHT);
        uint64_t blackBishops = board.getPieces(BLACK_BISHOP);
        uint64_t blackRooks = board.getPieces(BLACK_ROOK);
        uint64_t blackQueens = board.getPieces(BLACK_QUEEN);

        int score = 0;

        score += std::popcount(whiteKnights) + std::popcount(blackKnights);
        score += std::popcount(whiteBishops) + std::popcount(blackBishops);
        score += 2 * (std::popcount(whiteRooks) + std::popcount(blackRooks));
        score += 4 * (std::popcount(whiteQueens) + std::popcount(blackQueens));

        return score;  // Maximum value is 24 for a starting position
    }

    int getMaterialValue(const Board& board) {
        uint64_t whitePawns = board.getPieces(WHITE_PAWN);
        uint64_t whiteKnights = board.getPieces(WHITE_KNIGHT);
        uint64_t whiteBishops = board.getPieces(WHITE_BISHOP);
        uint64_t whiteRooks = board.getPieces(WHITE_ROOK);
        uint64_t whiteQueens = board.getPieces(WHITE_QUEEN);

        uint64_t blackPawns = board.getPieces(BLACK_PAWN);
        uint64_t blackKnights = board.getPieces(BLACK_KNIGHT);
        uint64_t blackBishops = board.getPieces(BLACK_BISHOP);
        uint64_t blackRooks = board.getPieces(BLACK_ROOK);
        uint64_t blackQueens = board.getPieces(BLACK_QUEEN);

        int score = 0;
        score += pawnValue * (std::popcount(whitePawns) - std::popcount(blackPawns));
        score += knightValue * (std::popcount(whiteKnights) - std::popcount(blackKnights));
        score += bishopValue * (std::popcount(whiteBishops) - std::popcount(blackBishops));
        score += rookValue * (std::popcount(whiteRooks) - std::popcount(blackRooks));
        score += queenValue * (std::popcount(whiteQueens) - std::popcount(blackQueens));

        return score;
    }

    PositionalScores getPositionalCorrection(const Board& board) {
        int mgScore = 0;
        int egScore = 0;

        for (const Piece piece : ALL_PIECES) {
            uint64_t pieces = board.getPieces(piece);

            const PieceTables tables = getTables(piece);
            const bool white = isWhite(piece);

            while (pieces != 0) {
                // Find the least significant occupied square.
                const int squareIndex = std::countr_zero(pieces);

                // Remove that square from the bitboard.
                pieces &= pieces - 1;

                Square square = static_cast<Square>(squareIndex);

                // PSTs are defined from White's perspective.
                if (!white) {
                    square = flipVertical(square);
                }

                const auto index = static_cast<std::size_t>(square);

                mgScore += (*tables.mg)[index];
                egScore += (*tables.eg)[index];
            }
        }
        return {mgScore, egScore};
    }

    int evaluate(const Board& board) {
        int score;

        int matVal = getMaterialValue(board);

        const auto [mgPositional, egPositional] = getPositionalCorrection(board);

        const int phase = std::min(getGamePhase(board), MAX_GAME_PHASE);

        int mgScore = matVal + mgPositional;
        int egScore = matVal + egPositional;

        score = (mgScore * phase + egScore * (MAX_GAME_PHASE - phase)) / MAX_GAME_PHASE;

        return (board.whiteToMove()) ? score : -score;
    }
}
