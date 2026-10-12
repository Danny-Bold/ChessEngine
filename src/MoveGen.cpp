//
// Created by Danny on 05/06/2026.
//

#include <vector>
#include <cstdint>

#include "chess/Bitboard.h"
#include "chess/constants.h"
#include "chess/Board.h"
#include "chess/Move.h"
#include "chess/AttackTables.h"

namespace Chess {
    std::vector<Move> generatePseudoWhiteMoves(const Board& board) {
        std::vector<Move> moves;

        getWhitePawnMoves(board, moves);
        getWhiteKnightMoves(board, moves);
        getWhiteBishopMoves(board, moves);
        getWhiteRookMoves(board, moves);
        getWhiteQueenMoves(board, moves);
        getWhiteKingMoves(board, moves);

        return moves;
    }

    std::vector<Move> generateWhiteMoves(Board& board) {
        std::vector<Move> pseudo = generatePseudoWhiteMoves(board);

        for (auto it = pseudo.begin(); it != pseudo.end(); ) {
            Move move = *it;

            board.makeMove(move);
            Square kingPos = board.whiteKingPosition();

            bool legal = !board.isSquareAttacked(kingPos, Colour::BLACK);  // Move is legal if king is not in check

            board.undoMove(move);

            if (!legal) {
                it = pseudo.erase(it);
            }
            else {
                ++it;
            }
        }
        return pseudo;
    }

    std::vector<Move> generatePseudoBlackMoves(const Board& board) {
        std::vector<Move> moves;

        getBlackPawnMoves(board, moves);
        getBlackKnightMoves(board, moves);
        getBlackBishopMoves(board, moves);
        getBlackRookMoves(board, moves);
        getBlackQueenMoves(board, moves);
        getBlackKingMoves(board, moves);

        return moves;
    }

    std::vector<Move> generateBlackMoves(Board& board) {
        std::vector<Move> pseudo = generatePseudoBlackMoves(board);

        for (auto it = pseudo.begin(); it != pseudo.end(); ) {
            Move move = *it;

            board.makeMove(move);
            Square kingPos = board.blackKingPosition();
            bool legal = !board.isSquareAttacked(kingPos, Colour::WHITE);  // Move is legal if king is not in check

            board.undoMove(move);

            if (!legal) {
                it = pseudo.erase(it);
            }
            else {
                ++it;
            }
        }
        return pseudo;
    }

    void getWhitePawnMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t pawns = board.whitePawns;
        uint64_t empty = board.getEmpty();
        uint64_t enemies = board.getBlackPieces();

        uint64_t singlePush = pawns << 8 & empty;

        uint64_t normalPush = singlePush & ~rank8;
        uint64_t promotionPush = singlePush & rank8;

        uint64_t leftAttack = (pawns & notAFile) << 7 & enemies;
        uint64_t rightAttack = (pawns & notHFile) << 9 & enemies;

        uint64_t leftAttackNormal = leftAttack & ~rank8;
        uint64_t leftAttackPromotion = leftAttack & rank8;

        uint64_t rightAttackNormal = rightAttack & ~rank8;
        uint64_t rightAttackPromotion = rightAttack & rank8;

        // For double pushes, we want the squares for one push and the square for two push to be empty, AND the pawn to be on the 2nd rank

        uint64_t rank2Pawns = pawns & rank2;
        uint64_t doublePush = ((rank2Pawns << 8) & empty) << 8 & empty;

        // Now add to moves

        while (normalPush) {
            Square to = popLSB(normalPush);
            Square from = shift(to, -8);
            moves.push_back(Move(from, to, WHITE_PAWN, EMPTY, EMPTY, false, false, false));
        }

        while (promotionPush) {
            Square to = popLSB(promotionPush);
            Square from = shift(to, -8);

            moves.push_back(Move(from, to, WHITE_PAWN, EMPTY, WHITE_QUEEN,  false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, EMPTY, WHITE_ROOK,   false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, EMPTY, WHITE_KNIGHT, false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, EMPTY, WHITE_BISHOP, false, false, false));
        }

        while (doublePush) {
            Square to = popLSB(doublePush);
            Square from = shift(to, -16);
            moves.push_back(Move(from, to, WHITE_PAWN, EMPTY, EMPTY, false, false, true));
        }

        while (leftAttackNormal) {
            Square to = popLSB(leftAttackNormal);
            Square from = shift(to, -7);
            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), EMPTY, false, false, false));
        }

        while (leftAttackPromotion) {
            Square to = popLSB(leftAttackPromotion);
            Square from = shift(to, -7);

            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), WHITE_QUEEN,  false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), WHITE_ROOK,   false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), WHITE_KNIGHT, false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), WHITE_BISHOP, false, false, false));
        }

        while (rightAttackNormal) {
            Square to = popLSB(rightAttackNormal);
            Square from = shift(to, -9);
            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), EMPTY, false, false, false));
        }

        while (rightAttackPromotion) {
            Square to = popLSB(rightAttackPromotion);
            Square from = shift(to, -9);

            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), WHITE_QUEEN,  false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), WHITE_ROOK,   false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), WHITE_KNIGHT, false, false, false));
            moves.push_back(Move(from, to, WHITE_PAWN, board.pieceAt(to), WHITE_BISHOP, false, false, false));
        }

        // EN PASSANT

        if (board.enPassantSquare != Square::NO_SQUARE) {
            uint64_t epBit = 1ULL << toIndex(board.enPassantSquare);
            uint64_t epLeft = (pawns & notAFile) << 7 & epBit;
            uint64_t epRight = (pawns & notHFile) << 9 & epBit;

            if (epLeft) {
                Square to = board.enPassantSquare;
                Square from = shift(to, -7);

                moves.push_back(Move(from, to, WHITE_PAWN, BLACK_PAWN, EMPTY, true, false, false));
            }
            if (epRight) {
                Square to = board.enPassantSquare;
                Square from = shift(to, -9);

                moves.push_back(Move(from, to, WHITE_PAWN, BLACK_PAWN, EMPTY, true, false, false));
            }
        }
    }

    void getWhiteKnightMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t knights = board.whiteKnights;
        uint64_t valid = ~board.getWhitePieces();

        while (knights) {
            Square from = popLSB(knights);

            uint64_t knightMoves = knightAttacks[toIndex(from)] & valid; // bitboard

            while (knightMoves) {
                Square to = popLSB(knightMoves);
                moves.push_back(Move(from, to, WHITE_KNIGHT, board.pieceAt(to), EMPTY, false, false, false));
            }
        }
    }

    void getWhiteBishopMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t bishops = board.whiteBishops;
        uint64_t occ = board.getAllPieces();
        uint64_t friendly = board.getWhitePieces();

        // Use precomputed bitmasks for bishop rays.

        while (bishops) {
            Square from = popLSB(bishops);

            // NE ray

            uint64_t rayNE = bishopNE[toIndex(from)];
            uint64_t blockersNE = rayNE & occ;

            if (blockersNE) {
                Square blockerSquare = popLSB(blockersNE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayNE &= ~bishopNE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayNE &= ~blockerBit;
                }
            }

            while (rayNE) {
                Square to = popLSB(rayNE);
                moves.push_back(Move(from, to, WHITE_BISHOP, board.pieceAt(to), EMPTY, false, false, false));
            }

            // SE ray

            uint64_t raySE = bishopSE[toIndex(from)];
            uint64_t blockersSE = raySE & occ;

            if (blockersSE) {
                Square blockerSquare = popMSB(blockersSE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                raySE &= ~bishopSE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    raySE &= ~blockerBit;
                }
            }

            while (raySE) {
                Square to = popLSB(raySE);
                moves.push_back(Move(from, to, WHITE_BISHOP, board.pieceAt(to), EMPTY, false, false, false));
            }

            // NW

            uint64_t rayNW = bishopNW[toIndex(from)];
            uint64_t blockersNW = rayNW & occ;

            if (blockersNW) {
                Square blockerSquare = popLSB(blockersNW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayNW &= ~bishopNW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayNW &= ~blockerBit;
                }
            }

            while (rayNW) {
                Square to = popLSB(rayNW);
                moves.push_back(Move(from, to, WHITE_BISHOP, board.pieceAt(to), EMPTY, false, false, false));
            }

            // SW

            uint64_t raySW = bishopSW[toIndex(from)];
            uint64_t blockersSW = raySW & occ;

            if (blockersSW) {
                Square blockerSquare = popMSB(blockersSW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                raySW &= ~bishopSW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    raySW &= ~blockerBit;
                }
            }

            while (raySW) {
                Square to = popLSB(raySW);
                moves.push_back(Move(from, to, WHITE_BISHOP, board.pieceAt(to), EMPTY, false, false, false));
            }
        }
    }

    void getWhiteRookMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t rooks = board.whiteRooks;
        uint64_t occ = board.getAllPieces();
        uint64_t friendly = board.getWhitePieces();

        while (rooks) {
            Square from = popLSB(rooks);

            // North ray

            uint64_t rayN = rookN[toIndex(from)];
            uint64_t blockersN = rayN & occ;

            if (blockersN) {
                Square blockerSquare = popLSB(blockersN);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayN &= ~rookN[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayN &= ~blockerBit;
                }
            }

            while (rayN) {
                Square to = popLSB(rayN);
                moves.push_back(Move(from, to, WHITE_ROOK, board.pieceAt(to), EMPTY, false, false, false));
            }

            // East ray

            uint64_t rayE = rookE[toIndex(from)];
            uint64_t blockersE = rayE & occ;

            if (blockersE) {
                Square blockerSquare = popLSB(blockersE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayE &= ~rookE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayE &= ~blockerBit;
                }
            }

            while (rayE) {
                Square to = popLSB(rayE);
                moves.push_back(Move(from, to, WHITE_ROOK, board.pieceAt(to), EMPTY, false, false, false));
            }

            // South ray

            uint64_t rayS = rookS[toIndex(from)];
            uint64_t blockersS = rayS & occ;

            if (blockersS) {
                Square blockerSquare = popMSB(blockersS);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayS &= ~rookS[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayS &= ~blockerBit;
                }
            }

            while (rayS) {
                Square to = popLSB(rayS);
                moves.push_back(Move(from, to, WHITE_ROOK, board.pieceAt(to), EMPTY, false, false, false));
            }

            // West ray

            uint64_t rayW = rookW[toIndex(from)];
            uint64_t blockersW = rayW & occ;

            if (blockersW) {
                Square blockerSquare = popMSB(blockersW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayW &= ~rookW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayW &= ~blockerBit;
                }
            }

            while (rayW) {
                Square to = popLSB(rayW);
                moves.push_back(Move(from, to, WHITE_ROOK, board.pieceAt(to), EMPTY, false, false, false));
            }
        }
    }

    void getWhiteQueenMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t queens = board.whiteQueens;
        uint64_t occ = board.getAllPieces();
        uint64_t friendly = board.getWhitePieces();

        while (queens) {
            Square from = popLSB(queens);

            // North ray

            uint64_t rayN = rookN[toIndex(from)];
            uint64_t blockersN = rayN & occ;

            if (blockersN) {
                Square blockerSquare = popLSB(blockersN);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayN &= ~rookN[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayN &= ~blockerBit;
                }
            }

            while (rayN) {
                Square to = popLSB(rayN);
                moves.push_back(Move(from, to, WHITE_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // East ray

            uint64_t rayE = rookE[toIndex(from)];
            uint64_t blockersE = rayE & occ;

            if (blockersE) {
                Square blockerSquare = popLSB(blockersE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayE &= ~rookE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayE &= ~blockerBit;
                }
            }

            while (rayE) {
                Square to = popLSB(rayE);
                moves.push_back(Move(from, to, WHITE_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // South ray

            uint64_t rayS = rookS[toIndex(from)];
            uint64_t blockersS = rayS & occ;

            if (blockersS) {
                Square blockerSquare = popMSB(blockersS);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayS &= ~rookS[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayS &= ~blockerBit;
                }
            }

            while (rayS) {
                Square to = popLSB(rayS);
                moves.push_back(Move(from, to, WHITE_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // West ray

            uint64_t rayW = rookW[toIndex(from)];
            uint64_t blockersW = rayW & occ;

            if (blockersW) {
                Square blockerSquare = popMSB(blockersW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayW &= ~rookW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayW &= ~blockerBit;
                }
            }

            while (rayW) {
                Square to = popLSB(rayW);
                moves.push_back(Move(from, to, WHITE_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // NE ray

            uint64_t rayNE = bishopNE[toIndex(from)];
            uint64_t blockersNE = rayNE & occ;

            if (blockersNE) {
                Square blockerSquare = popLSB(blockersNE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayNE &= ~bishopNE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayNE &= ~blockerBit;
                }
            }

            while (rayNE) {
                Square to = popLSB(rayNE);
                moves.push_back(Move(from, to, WHITE_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // SE ray

            uint64_t raySE = bishopSE[toIndex(from)];
            uint64_t blockersSE = raySE & occ;

            if (blockersSE) {
                Square blockerSquare = popMSB(blockersSE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                raySE &= ~bishopSE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    raySE &= ~blockerBit;
                }
            }

            while (raySE) {
                Square to = popLSB(raySE);
                moves.push_back(Move(from, to, WHITE_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // NW

            uint64_t rayNW = bishopNW[toIndex(from)];
            uint64_t blockersNW = rayNW & occ;

            if (blockersNW) {
                Square blockerSquare = popLSB(blockersNW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayNW &= ~bishopNW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayNW &= ~blockerBit;
                }
            }

            while (rayNW) {
                Square to = popLSB(rayNW);
                moves.push_back(Move(from, to, WHITE_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // SW

            uint64_t raySW = bishopSW[toIndex(from)];
            uint64_t blockersSW = raySW & occ;

            if (blockersSW) {
                Square blockerSquare = popMSB(blockersSW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                raySW &= ~bishopSW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    raySW &= ~blockerBit;
                }
            }

            while (raySW) {
                Square to = popLSB(raySW);
                moves.push_back(Move(from, to, WHITE_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }
        }
    }

    void getWhiteKingMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t kings = board.whiteKing;
        Square kingSquare = popLSB(kings);  // Exactly one king should be present
        uint64_t valid = ~board.getWhitePieces();

        uint64_t kingMoves = kingAttacks[toIndex(kingSquare)] & valid;

        while (kingMoves) {
            Square to = popLSB(kingMoves);
            moves.push_back(Move(kingSquare, to, WHITE_KING, board.pieceAt(to), EMPTY, false, false, false));
        }

        uint64_t occ = board.getAllPieces();

        // Kingside castle

        if (board.castlingRights & WK) {
            // Check King and Rook see each other
            bool empty = !(occ & ((1ULL << toIndex(Square::F1)) | (1ULL << toIndex(Square::G1))));

            bool safe =
                !(board.isSquareAttacked(Square::E1, Colour::BLACK)) &&
                !board.isSquareAttacked(Square::F1, Colour::BLACK) &&
                !board.isSquareAttacked(Square::G1, Colour::BLACK);

            if (empty && safe) {
                moves.push_back(Move(Square::E1, Square::G1, WHITE_KING, EMPTY, EMPTY, false, true, false));
            }
        }

        // Queenside castle
        if (board.castlingRights & WQ) {

            bool empty = !(occ & ((1ULL << toIndex(Square::B1)) |
                                  (1ULL << toIndex(Square::C1)) |
                                  (1ULL << toIndex(Square::D1))));

            bool safe =
                !board.isSquareAttacked(Square::E1, Colour::BLACK) &&
                !board.isSquareAttacked(Square::D1, Colour::BLACK) &&
                !board.isSquareAttacked(Square::C1, Colour::BLACK);

            if (empty && safe) {
                moves.push_back(Move(Square::E1, Square::C1, WHITE_KING, EMPTY, EMPTY, false, true, false));
            }
        }
    }

    void getBlackPawnMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t pawns = board.blackPawns;
        uint64_t empty = board.getEmpty();
        uint64_t enemies = board.getWhitePieces();

        uint64_t singlePush = pawns >> 8 & empty;

        uint64_t normalPush = singlePush & ~rank1;
        uint64_t promotionPush = singlePush & rank1;

        uint64_t leftAttack = (pawns & notAFile) >> 9 & enemies;
        uint64_t rightAttack = (pawns & notHFile) >> 7 & enemies;

        uint64_t leftAttackNormal = leftAttack & ~rank1;
        uint64_t leftAttackPromotion = leftAttack & rank1;

        uint64_t rightAttackNormal = rightAttack & ~rank1;
        uint64_t rightAttackPromotion = rightAttack & rank1;

        // For double pushes, we want the squares for one push and the square for two push to be empty, AND the pawn to be on the 2nd rank

        uint64_t rank7Pawns = pawns & rank7;
        uint64_t doublePush = ((rank7Pawns >> 8) & empty) >> 8 & empty;

        // Now add to moves

        while (normalPush) {
            Square to = popLSB(normalPush);
            Square from = shift(to, 8);
            moves.push_back(Move(from, to, BLACK_PAWN, EMPTY, EMPTY, false, false, false));
        }

        while (promotionPush) {
            Square to = popLSB(promotionPush);
            Square from = shift(to, 8);

            moves.push_back(Move(from, to, BLACK_PAWN, EMPTY, BLACK_QUEEN,  false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, EMPTY, BLACK_ROOK,   false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, EMPTY, BLACK_KNIGHT, false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, EMPTY, BLACK_BISHOP, false, false, false));
        }

        while (doublePush) {
            Square to = popLSB(doublePush);
            Square from = shift(to, 16);
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), EMPTY, false, false, true));
        }

        while (leftAttackNormal) {
            Square to = popLSB(leftAttackNormal);
            Square from = shift(to, 9);
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), EMPTY, false, false, false));
        }

        while (leftAttackPromotion) {
            Square to = popLSB(leftAttackPromotion);
            Square from = shift(to, 9);

            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), BLACK_QUEEN,  false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), BLACK_ROOK,   false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), BLACK_KNIGHT, false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), BLACK_BISHOP, false, false, false));
        }

        while (rightAttackNormal) {
            Square to = popLSB(rightAttackNormal);
            Square from = shift(to, 7);
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), EMPTY, false, false, false));
        }

        while (rightAttackPromotion) {
            Square to = popLSB(rightAttackPromotion);
            Square from = shift(to, 7);

            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), BLACK_QUEEN,  false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), BLACK_ROOK,   false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), BLACK_KNIGHT, false, false, false));
            moves.push_back(Move(from, to, BLACK_PAWN, board.pieceAt(to), BLACK_BISHOP, false, false, false));
        }

        if (board.enPassantSquare != Square::NO_SQUARE) {
            uint64_t epBit = 1ULL << toIndex(board.enPassantSquare);
            uint64_t epLeft = (pawns & notAFile) >> 9 & epBit;
            uint64_t epRight = (pawns & notHFile) >> 7 & epBit;

            if (epLeft) {
                Square to = board.enPassantSquare;
                Square from = shift(to, 9);

                moves.push_back(Move(from, to, BLACK_PAWN, WHITE_PAWN, EMPTY, true, false, false));
            }
            if (epRight) {
                Square to = board.enPassantSquare;
                Square from = shift(to, 7);

                moves.push_back(Move(from, to, BLACK_PAWN, WHITE_PAWN, EMPTY, true, false, false));
            }
        }
    }

    void getBlackKnightMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t knights = board.blackKnights;
        uint64_t valid = ~board.getBlackPieces();

        while (knights) {
            Square from = popLSB(knights);

            uint64_t knightMoves = knightAttacks[toIndex(from)] & valid; // bitboard

            while (knightMoves) {
                Square to = popLSB(knightMoves);
                moves.push_back(Move(from, to, BLACK_KNIGHT, board.pieceAt(to), EMPTY, false, false, false));
            }
        }
    }

    void getBlackBishopMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t bishops = board.blackBishops;
        uint64_t occ = board.getAllPieces();
        uint64_t friendly = board.getBlackPieces();

        // Use precomputed bitmasks for bishop rays.

        while (bishops) {
            Square from = popLSB(bishops);

            // NE ray

            uint64_t rayNE = bishopNE[toIndex(from)];
            uint64_t blockersNE = rayNE & occ;

            if (blockersNE) {
                Square blockerSquare = popLSB(blockersNE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayNE &= ~bishopNE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayNE &= ~blockerBit;
                }
            }

            while (rayNE) {
                Square to = popLSB(rayNE);
                moves.push_back(Move(from, to, BLACK_BISHOP, board.pieceAt(to), EMPTY, false, false, false));
            }

            // SE ray

            uint64_t raySE = bishopSE[toIndex(from)];
            uint64_t blockersSE = raySE & occ;

            if (blockersSE) {
                Square blockerSquare = popMSB(blockersSE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                raySE &= ~bishopSE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    raySE &= ~blockerBit;
                }
            }

            while (raySE) {
                Square to = popLSB(raySE);
                moves.push_back(Move(from, to, BLACK_BISHOP, board.pieceAt(to), EMPTY, false, false, false));
            }

            // NW

            uint64_t rayNW = bishopNW[toIndex(from)];
            uint64_t blockersNW = rayNW & occ;

            if (blockersNW) {
                Square blockerSquare = popLSB(blockersNW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayNW &= ~bishopNW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayNW &= ~blockerBit;
                }
            }

            while (rayNW) {
                Square to = popLSB(rayNW);
                moves.push_back(Move(from, to, BLACK_BISHOP, board.pieceAt(to), EMPTY, false, false, false));
            }

            // SW

            uint64_t raySW = bishopSW[toIndex(from)];
            uint64_t blockersSW = raySW & occ;

            if (blockersSW) {
                Square blockerSquare = popMSB(blockersSW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                raySW &= ~bishopSW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    raySW &= ~blockerBit;
                }
            }

            while (raySW) {
                Square to = popLSB(raySW);
                moves.push_back(Move(from, to, BLACK_BISHOP, board.pieceAt(to), EMPTY, false, false, false));
            }
        }
    }

    void getBlackRookMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t rooks = board.blackRooks;
        uint64_t occ = board.getAllPieces();
        uint64_t friendly = board.getBlackPieces();

        while (rooks) {
            Square from = popLSB(rooks);

            // North ray

            uint64_t rayN = rookN[toIndex(from)];
            uint64_t blockersN = rayN & occ;

            if (blockersN) {
                Square blockerSquare = popLSB(blockersN);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayN &= ~rookN[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayN &= ~blockerBit;
                }
            }

            while (rayN) {
                Square to = popLSB(rayN);
                moves.push_back(Move(from, to, BLACK_ROOK, board.pieceAt(to), EMPTY, false, false, false));
            }

            // East ray

            uint64_t rayE = rookE[toIndex(from)];
            uint64_t blockersE = rayE & occ;

            if (blockersE) {
                Square blockerSquare = popLSB(blockersE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayE &= ~rookE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayE &= ~blockerBit;
                }
            }

            while (rayE) {
                Square to = popLSB(rayE);
                moves.push_back(Move(from, to, BLACK_ROOK, board.pieceAt(to), EMPTY, false, false, false));
            }

            // South ray

            uint64_t rayS = rookS[toIndex(from)];
            uint64_t blockersS = rayS & occ;

            if (blockersS) {
                Square blockerSquare = popMSB(blockersS);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayS &= ~rookS[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayS &= ~blockerBit;
                }
            }

            while (rayS) {
                Square to = popLSB(rayS);
                moves.push_back(Move(from, to, BLACK_ROOK, board.pieceAt(to), EMPTY, false, false, false));
            }

            // West ray

            uint64_t rayW = rookW[toIndex(from)];
            uint64_t blockersW = rayW & occ;

            if (blockersW) {
                Square blockerSquare = popMSB(blockersW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayW &= ~rookW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayW &= ~blockerBit;
                }
            }

            while (rayW) {
                Square to = popLSB(rayW);
                moves.push_back(Move(from, to, BLACK_ROOK, board.pieceAt(to), EMPTY, false, false, false));
            }
        }
    }

    void getBlackQueenMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t queens = board.blackQueens;
        uint64_t occ = board.getAllPieces();
        uint64_t friendly = board.getBlackPieces();

        while (queens) {
            Square from = popLSB(queens);

            // North ray

            uint64_t rayN = rookN[toIndex(from)];
            uint64_t blockersN = rayN & occ;

            if (blockersN) {
                Square blockerSquare = popLSB(blockersN);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayN &= ~rookN[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayN &= ~blockerBit;
                }
            }

            while (rayN) {
                Square to = popLSB(rayN);
                moves.push_back(Move(from, to, BLACK_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // East ray

            uint64_t rayE = rookE[toIndex(from)];
            uint64_t blockersE = rayE & occ;

            if (blockersE) {
                Square blockerSquare = popLSB(blockersE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayE &= ~rookE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayE &= ~blockerBit;
                }
            }

            while (rayE) {
                Square to = popLSB(rayE);
                moves.push_back(Move(from, to, BLACK_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // South ray

            uint64_t rayS = rookS[toIndex(from)];
            uint64_t blockersS = rayS & occ;

            if (blockersS) {
                Square blockerSquare = popMSB(blockersS);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayS &= ~rookS[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayS &= ~blockerBit;
                }
            }

            while (rayS) {
                Square to = popLSB(rayS);
                moves.push_back(Move(from, to, BLACK_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // West ray

            uint64_t rayW = rookW[toIndex(from)];
            uint64_t blockersW = rayW & occ;

            if (blockersW) {
                Square blockerSquare = popMSB(blockersW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayW &= ~rookW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayW &= ~blockerBit;
                }
            }

            while (rayW) {
                Square to = popLSB(rayW);
                moves.push_back(Move(from, to, BLACK_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // NE ray

            uint64_t rayNE = bishopNE[toIndex(from)];
            uint64_t blockersNE = rayNE & occ;

            if (blockersNE) {
                Square blockerSquare = popLSB(blockersNE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayNE &= ~bishopNE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayNE &= ~blockerBit;
                }
            }

            while (rayNE) {
                Square to = popLSB(rayNE);
                moves.push_back(Move(from, to, BLACK_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // SE ray

            uint64_t raySE = bishopSE[toIndex(from)];
            uint64_t blockersSE = raySE & occ;

            if (blockersSE) {
                Square blockerSquare = popMSB(blockersSE);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                raySE &= ~bishopSE[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    raySE &= ~blockerBit;
                }
            }

            while (raySE) {
                Square to = popLSB(raySE);
                moves.push_back(Move(from, to, BLACK_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // NW

            uint64_t rayNW = bishopNW[toIndex(from)];
            uint64_t blockersNW = rayNW & occ;

            if (blockersNW) {
                Square blockerSquare = popLSB(blockersNW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                rayNW &= ~bishopNW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    rayNW &= ~blockerBit;
                }
            }

            while (rayNW) {
                Square to = popLSB(rayNW);
                moves.push_back(Move(from, to, BLACK_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }

            // SW

            uint64_t raySW = bishopSW[toIndex(from)];
            uint64_t blockersSW = raySW & occ;

            if (blockersSW) {
                Square blockerSquare = popMSB(blockersSW);
                uint64_t blockerBit = 1ULL << toIndex(blockerSquare);

                raySW &= ~bishopSW[toIndex(blockerSquare)];

                if (friendly & blockerBit) {
                    raySW &= ~blockerBit;
                }
            }

            while (raySW) {
                Square to = popLSB(raySW);
                moves.push_back(Move(from, to, BLACK_QUEEN, board.pieceAt(to), EMPTY, false, false, false));
            }
        }
    }

    void getBlackKingMoves(const Board& board, std::vector<Move>& moves) {
        uint64_t kings = board.blackKing;
        Square kingSquare = popLSB(kings);  // Exactly one king should be present
        uint64_t valid = ~board.getBlackPieces();

        uint64_t kingMoves = kingAttacks[toIndex(kingSquare)] & valid;

        while (kingMoves) {
            Square to = popLSB(kingMoves);
            moves.push_back(Move(kingSquare, to, BLACK_KING, board.pieceAt(to), EMPTY, false, false, false));
        }

        uint64_t occ = board.getAllPieces();

        // Kingside castle
        if (board.castlingRights & BK) {

            bool empty = !(occ & ((1ULL << toIndex(Square::F8)) | (1ULL << toIndex(Square::G8))));

            bool safe =
                !(board.isSquareAttacked(Square::E8, Colour::WHITE)) &&
                !board.isSquareAttacked(Square::F8, Colour::WHITE) &&
                !board.isSquareAttacked(Square::G8, Colour::WHITE);

            if (empty && safe) {
                moves.push_back(Move(Square::E8, Square::G8, BLACK_KING, EMPTY, EMPTY, false, true, false));
            }
        }

        // Queenside castle
        if (board.castlingRights & BQ) {
            bool empty = !(occ & ((1ULL << toIndex(Square::B8)) |
                                  (1ULL << toIndex(Square::C8)) |
                                  (1ULL << toIndex(Square::D8))));

            bool safe =
                !board.isSquareAttacked(Square::E8, Colour::WHITE) &&
                !board.isSquareAttacked(Square::D8, Colour::WHITE) &&
                !board.isSquareAttacked(Square::C8, Colour::WHITE);

            if (empty && safe) {
                moves.push_back(Move(Square::E8, Square::C8, BLACK_KING, EMPTY, EMPTY, false, true, false));
            }
        }
    }
}
