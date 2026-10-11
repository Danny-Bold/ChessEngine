//
// Created by Danny on 05/06/2026.
//

#include <iostream>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <stdexcept>
#include <string>

#include "chess/Board.h"
#include "chess/AttackTables.h"
#include "chess/Bitboard.h"
#include "chess/constants.h"
#include "chess/MoveGen.h"
#include "chess/UCI.h"


namespace Chess{
    Board::Board()
    : board {
            WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING, WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK,
            WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN, WHITE_PAWN,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
            BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN, BLACK_PAWN,
            BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING, BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK
    }
    {
        whitePawns = 0x000000000000FF00ULL;
        whiteRooks = 0x0000000000000081ULL;
        whiteKnights = 0x0000000000000042ULL;
        whiteBishops = 0x0000000000000024ULL;
        whiteQueens = 0x0000000000000008ULL;
        whiteKing = 0x0000000000000010ULL;

        blackPawns = 0x00FF000000000000ULL;
        blackRooks = 0x8100000000000000ULL;
        blackKnights = 0x4200000000000000ULL;
        blackBishops = 0x2400000000000000ULL;
        blackQueens = 0x0800000000000000ULL;
        blackKing = 0x1000000000000000ULL;

        enPassantSquare = Square::NO_SQUARE;  // If a pawn pushes from e7 to e5, we set enPassantSquare as e6 (destination for a successful en passant)
        castlingRights = WK | WQ | BK | BQ;

        sideToMove = Colour::WHITE;

        halfMoveClock = 0;
    }

    void Board::print() const {
        for (int rank = 7; rank >= 0; rank--) {

            std::cout << (rank + 1) << "  ";

            for (int file = 0; file < 8; file++) {
                const int sq = rank * 8 + file;
                const uint64_t mask = 1ULL << sq;

                if (whitePawns & mask) std::cout << "P ";
                else if (whiteRooks & mask) std::cout << "R ";
                else if (whiteKnights & mask) std::cout << "N ";
                else if (whiteBishops & mask) std::cout << "B ";
                else if (whiteQueens & mask) std::cout << "Q ";
                else if (whiteKing & mask) std::cout << "K ";

                else if (blackPawns & mask) std::cout << "p ";
                else if (blackRooks & mask) std::cout << "r ";
                else if (blackKnights & mask) std::cout << "n ";
                else if (blackBishops & mask) std::cout << "b ";
                else if (blackQueens & mask) std::cout << "q ";
                else if (blackKing & mask) std::cout << "k ";
                else std::cout << ". ";
            }
            std::cout << "\n";
        }
        std::cout << "\n   a b c d e f g h\n";
    }

    void Board::printMailbox() const {
        for (int rank = 7; rank >= 0; rank--) {

            std::cout << (rank + 1) << "  ";

            for (int file = 0; file < 8; file++) {
                const int sq = rank * 8 + file;
                std::cout << pieceToChar(board[sq]) << ' ';
            }

            std::cout << '\n';
        }

        std::cout << "\n   a b c d e f g h\n";
    }

    uint64_t Board::getWhitePieces() const {
        return whitePawns | whiteRooks | whiteKnights | whiteBishops | whiteQueens | whiteKing;
    }

    uint64_t Board::getBlackPieces() const {
        return blackPawns | blackRooks | blackKnights | blackBishops | blackQueens | blackKing;
    }

    uint64_t Board::getAllPieces() const {
        return getWhitePieces() | getBlackPieces();
    }

    uint64_t Board::getPieces(Piece piece) const {
        switch (piece) {
            case WHITE_PAWN: return whitePawns;
            case WHITE_KNIGHT: return whiteKnights;
            case WHITE_BISHOP: return whiteBishops;
            case WHITE_ROOK: return whiteRooks;
            case WHITE_QUEEN: return whiteQueens;
            case WHITE_KING: return whiteKing;

            case BLACK_PAWN: return blackPawns;
            case BLACK_KNIGHT: return blackKnights;
            case BLACK_BISHOP: return blackBishops;
            case BLACK_ROOK: return blackRooks;
            case BLACK_QUEEN: return blackQueens;
            case BLACK_KING: return blackKing;
            default: throw std::invalid_argument("Invalid piece");
        }
    }

    uint64_t Board::getEmpty() const {
        return ~getAllPieces();
    }

    bool Board::whiteToMove() const {
        return sideToMove == Colour::WHITE;
    }

    bool Board::isSquareAttacked(Square square, Colour colour) const {
        // We're not checking through all legal moves - we're checking specific combinations
        // Assume colour = BLACK, so we're checking if a white piece is being attacked by a black one
        // First, pawns: if there is a black pawn in squares 7 and 9 away from the piece, attacked
        // Knights: if any black knights are on knightAttacks[toIndex(square)] then attacked
        // Kings: same thing, kingAttacks[toIndex(square)]
        // Bishops, rooks etc: process in a similar way to move generation.
        // For each direction, get ray, e.g. bishopNE[square].
        // Get occupancy mask, blockers = occ & ray
        // Find nearest blocker via popLSB or popMSB, then check if bishop or queen. If so, attacked.

        initialiseAttackTables();

        uint64_t squareIndex = toIndex(square);
        uint64_t occ = getAllPieces();

        if (colour == Colour::BLACK) {
            // Black Pawns

            uint64_t attackersBP = whitePawnAttacks[squareIndex] & blackPawns;

            if (attackersBP != 0) {
                return true;
            }

            // Black Knights

            uint64_t attackersBN = knightAttacks[squareIndex] & blackKnights;

            if (attackersBN != 0) {
                return true;
            }

            // Black Bishops + Queens

            uint64_t blockersNE = bishopNE[squareIndex] & occ;

            if (blockersNE) {
                Square blockerSquare = popLSB(blockersNE);
                Piece piece = pieceAt(blockerSquare);

                if (piece == BLACK_BISHOP || piece == BLACK_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersSE = bishopSE[squareIndex] & occ;

            if (blockersSE) {
                Square blockerSquare = popMSB(blockersSE);
                Piece piece = pieceAt(blockerSquare);

                if (piece == BLACK_BISHOP || piece == BLACK_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersSW = bishopSW[squareIndex] & occ;

            if (blockersSW) {
                Square blockerSquare = popMSB(blockersSW);
                Piece piece = pieceAt(blockerSquare);

                if (piece == BLACK_BISHOP || piece == BLACK_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersNW = bishopNW[squareIndex] & occ;

            if (blockersNW) {
                Square blockerSquare = popLSB(blockersNW);
                Piece piece = pieceAt(blockerSquare);

                if (piece == BLACK_BISHOP || piece == BLACK_QUEEN) {
                    return true;
                }
            }

            // Black Rooks + Queens

            uint64_t blockersN = rookN[squareIndex] & occ;

            if (blockersN) {
                Square blockerSquare = popLSB(blockersN);
                Piece piece = pieceAt(blockerSquare);

                if (piece == BLACK_ROOK || piece == BLACK_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersE = rookE[squareIndex] & occ;

            if (blockersE) {
                Square blockerSquare = popLSB(blockersE);
                Piece piece = pieceAt(blockerSquare);

                if (piece == BLACK_ROOK || piece == BLACK_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersS = rookS[squareIndex] & occ;

            if (blockersS) {
                Square blockerSquare = popMSB(blockersS);
                Piece piece = pieceAt(blockerSquare);

                if (piece == BLACK_ROOK || piece == BLACK_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersW = rookW[squareIndex] & occ;

            if (blockersW) {
                Square blockerSquare = popMSB(blockersW);
                Piece piece = pieceAt(blockerSquare);

                if (piece == BLACK_ROOK || piece == BLACK_QUEEN) {
                    return true;
                }
            }

            // Black King

            uint64_t attackersBK = kingAttacks[squareIndex] & blackKing;

            if (attackersBK != 0) {
                return true;
            }
        }

        else if (colour == Colour::WHITE) {
            // White Pawns

            uint64_t attackersWP = blackPawnAttacks[squareIndex] & whitePawns;

            if (attackersWP != 0) {
                return true;
            }

            // White Knights

            uint64_t attackersWN = knightAttacks[squareIndex] & whiteKnights;

            if (attackersWN != 0) {
                return true;
            }

            // White Bishops + Queens

            uint64_t blockersNE = bishopNE[squareIndex] & occ;

            if (blockersNE) {
                Square blockerSquare = popLSB(blockersNE);
                Piece piece = pieceAt(blockerSquare);

                if (piece == WHITE_BISHOP || piece == WHITE_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersSE = bishopSE[squareIndex] & occ;

            if (blockersSE) {
                Square blockerSquare = popMSB(blockersSE);
                Piece piece = pieceAt(blockerSquare);

                if (piece == WHITE_BISHOP || piece == WHITE_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersSW = bishopSW[squareIndex] & occ;

            if (blockersSW) {
                Square blockerSquare = popMSB(blockersSW);
                Piece piece = pieceAt(blockerSquare);

                if (piece == WHITE_BISHOP || piece == WHITE_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersNW = bishopNW[squareIndex] & occ;

            if (blockersNW) {
                Square blockerSquare = popLSB(blockersNW);
                Piece piece = pieceAt(blockerSquare);

                if (piece == WHITE_BISHOP || piece == WHITE_QUEEN) {
                    return true;
                }
            }

            // White Rooks + Queens

            uint64_t blockersN = rookN[squareIndex] & occ;

            if (blockersN) {
                Square blockerSquare = popLSB(blockersN);
                Piece piece = pieceAt(blockerSquare);

                if (piece == WHITE_ROOK || piece == WHITE_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersE = rookE[squareIndex] & occ;

            if (blockersE) {
                Square blockerSquare = popLSB(blockersE);
                Piece piece = pieceAt(blockerSquare);

                if (piece == WHITE_ROOK || piece == WHITE_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersS = rookS[squareIndex] & occ;

            if (blockersS) {
                Square blockerSquare = popMSB(blockersS);
                Piece piece = pieceAt(blockerSquare);

                if (piece == WHITE_ROOK || piece == WHITE_QUEEN) {
                    return true;
                }
            }

            uint64_t blockersW = rookW[squareIndex] & occ;

            if (blockersW) {
                Square blockerSquare = popMSB(blockersW);
                Piece piece = pieceAt(blockerSquare);

                if (piece == WHITE_ROOK || piece == WHITE_QUEEN) {
                    return true;
                }
            }

            // White King

            uint64_t attackersWK = kingAttacks[squareIndex] & whiteKing;

            if (attackersWK != 0) {
                return true;
            }
        }

        return false;
    }

    bool Board::isInsufficientMaterial() const {
        if (getPieces(WHITE_PAWN) ||
            getPieces(BLACK_PAWN) ||
            getPieces(WHITE_ROOK) ||
            getPieces(BLACK_ROOK) ||
            getPieces(WHITE_QUEEN) ||
            getPieces(BLACK_QUEEN)) {
            return false;
            }

        uint64_t knights =
            getPieces(WHITE_KNIGHT) |
            getPieces(BLACK_KNIGHT);

        uint64_t bishops =
            getPieces(WHITE_BISHOP) |
            getPieces(BLACK_BISHOP);

        int knightCount = std::popcount(knights);
        int bishopCount = std::popcount(bishops);


        if (knightCount + bishopCount <= 1) {
            return true;
        }

        if (knightCount == 0) {
            constexpr uint64_t LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;
            constexpr uint64_t DARK_SQUARES = ~LIGHT_SQUARES;

            bool allLight = (bishops & DARK_SQUARES) == 0;
            bool allDark = (bishops & LIGHT_SQUARES) == 0;

            if (allLight || allDark) {
                return true;
            }
        }
        return false;
    }

    void Board::removePieceAt(Square square) {
        uint64_t bb = 1ULL << toIndex(square);
        Piece piece = pieceAt(square);

        assert(piece != EMPTY);

        switch (piece) {
            case WHITE_PAWN:
                whitePawns &= ~bb; break;
            case WHITE_ROOK:
                whiteRooks &= ~bb; break;
            case WHITE_KNIGHT:
                whiteKnights &= ~bb; break;
            case WHITE_BISHOP:
                whiteBishops &= ~bb; break;
            case WHITE_QUEEN:
                whiteQueens &= ~bb; break;
            case WHITE_KING:
                whiteKing &= ~bb; break;
            case BLACK_PAWN:
                blackPawns &= ~bb; break;
            case BLACK_ROOK:
                blackRooks &= ~bb; break;
            case BLACK_KNIGHT:
                blackKnights &= ~bb; break;
            case BLACK_BISHOP:
                blackBishops &= ~bb; break;
            case BLACK_QUEEN:
                blackQueens &= ~bb; break;
            case BLACK_KING:
                blackKing &= ~bb; break;
            case EMPTY:  // Never reached due to assertion, included for completeness
                break;
        }

        board[toIndex(square)] = EMPTY;
    }

    void Board::insertPieceAt(Square square, Piece piece) {
        // assert(piece != EMPTY);
        if (piece == EMPTY) {
            std::cerr << "\n=== INSERT EMPTY PIECE ===\n";
            std::cerr << "Square: " << static_cast<int>(square) << '\n';
            std::cerr << "Existing piece: "
                      << static_cast<int>(pieceAt(square)) << '\n';
            std::cerr << "Side to move: "
                      << static_cast<int>(sideToMove) << '\n';

            std::cerr << "Board before insertion:\n";
            print();

            std::cerr << "Mailbox board:\n";
            printMailbox();

            std::cerr << "==========================\n";
            std::abort();
        }
        assert(pieceAt(square) == EMPTY);

        uint64_t bb = 1ULL << toIndex(square);

        switch (piece) {
            case WHITE_PAWN:
                whitePawns |= bb; break;
            case WHITE_ROOK:
                whiteRooks |= bb; break;
            case WHITE_KNIGHT:
                whiteKnights |= bb; break;
            case WHITE_BISHOP:
                whiteBishops |= bb; break;
            case WHITE_QUEEN:
                whiteQueens |= bb; break;
            case WHITE_KING:
                whiteKing |= bb; break;
            case BLACK_PAWN:
                blackPawns |= bb; break;
            case BLACK_ROOK:
                blackRooks |= bb; break;
            case BLACK_KNIGHT:
                blackKnights |= bb; break;
            case BLACK_BISHOP:
                blackBishops |= bb; break;
            case BLACK_QUEEN:
                blackQueens |= bb; break;
            case BLACK_KING:
                blackKing |= bb; break;
            case EMPTY:  // Never reached due to assertion, included for completeness
                break;
        }

        board[toIndex(square)] = piece;
    }

    void Board::makeMove(const Move& move) {

        // if (move.movingPiece == EMPTY) {
        //     std::cerr << "\n=== SUSPICIOUS MOVE ===\n";
        //     std::cerr << "from: " << static_cast<int>(move.from) << '\n';
        //     std::cerr << "to:   " << static_cast<int>(move.to) << '\n';
        //     std::cerr << "movingPiece:   " << static_cast<int>(move.movingPiece) << '\n';
        //     std::cerr << "capturedPiece: " << static_cast<int>(move.capturedPiece) << '\n';
        //     std::cerr << "promotionPiece:" << static_cast<int>(move.promotionPiece) << '\n';
        //     std::cerr << "enPassant:     " << move.isEnPassant << '\n';
        //     std::cerr << "castle:        " << move.isCastle << '\n';
        //     std::cerr << "doublePush:    " << move.isDoublePawnPush << '\n';
        //     std::abort();
        // }

        Square from = move.from;
        Square to = move.to;

        Piece piece = move.movingPiece;

        // Push state onto history
        history.push_back({enPassantSquare, castlingRights, halfMoveClock});

        // Remove the piece being moved
        removePieceAt(from);

        // Handle captures
        if (move.capturedPiece != EMPTY && !move.isEnPassant) {
            removePieceAt(to);
        }

        // En passant
        if (move.isEnPassant) {
            if (piece == WHITE_PAWN) {
                Square capSq = shift(to, -8);
                removePieceAt(capSq);
            }
            else {
                Square capSq = shift(to, 8);
                removePieceAt(capSq);
            }
        }

        // Castling
        if (move.isCastle) {
            if (piece == WHITE_KING && from == Square::E1 && to == Square::G1) {  // White Kingside
                removePieceAt(Square::H1);
                insertPieceAt(Square::F1, WHITE_ROOK);
            }
            if (piece == WHITE_KING && from == Square::E1 && to == Square::C1) {  // White Queenside
                removePieceAt(Square::A1);
                insertPieceAt(Square::D1, WHITE_ROOK);
            }
            if (piece == BLACK_KING && from == Square::E8 && to == Square::G8) {  // Black Kingside
                removePieceAt(Square::H8);
                insertPieceAt(Square::F8, BLACK_ROOK);
            }

            if (piece == BLACK_KING && from == Square::E8 && to == Square::C8) {  // Black Queenside
                removePieceAt(Square::A8);
                insertPieceAt(Square::D8, BLACK_ROOK);
            }
        }

        // Promotions
        Piece finalPiece = piece;

        if (move.promotionPiece != EMPTY) {
            finalPiece = move.promotionPiece;
        }

        // Insert piece on destination
        insertPieceAt(to, finalPiece);

        // Update En Passant Square
        if (move.isDoublePawnPush) {
            enPassantSquare = (piece == WHITE_PAWN) ? shift(from, +8) : shift(from, -8);
        }
        else {
            enPassantSquare = Square::NO_SQUARE;
        }

        // Update castling rights
        if (piece == WHITE_KING) {
            castlingRights &= ~(WK | WQ);
        }
        if (piece == BLACK_KING) {
            castlingRights &= ~(BK | BQ);
        }

        if (piece == WHITE_ROOK) {
            if (from == Square::A1) castlingRights &= ~WQ;
            if (from == Square::H1) castlingRights &= ~WK;
        }

        if (piece == BLACK_ROOK) {
            if (from == Square::A8) castlingRights &= ~BQ;
            if (from == Square::H8) castlingRights &= ~BK;
        }

        // Also handle rook captures affecting castling rights
        if (move.capturedPiece == WHITE_ROOK) {
            if (to == Square::A1) castlingRights &= ~WQ;
            if (to == Square::H1) castlingRights &= ~WK;
        }

        if (move.capturedPiece == BLACK_ROOK) {
            if (to == Square::A8) castlingRights &= ~BQ;
            if (to == Square::H8) castlingRights &= ~BK;
        }

        sideToMove = opposite(sideToMove);

        // Handling halfmove clock

        if (move.movingPiece == WHITE_PAWN || move.movingPiece == BLACK_PAWN || move.capturedPiece != EMPTY) {
            halfMoveClock = 0;
        }
        else {
            halfMoveClock++;
        }
    }

    void Board::makeMove(std::string_view notation) {
        auto move = parseUCIMove(*this, notation);

        if (!move.has_value()) {
            throw std::invalid_argument("Invalid or illegal UCI move: " + std::string(notation));
        }

        makeMove(*move);
    }

    void Board::undoMove(const Move& move) {
        Square from = move.from;
        Square to = move.to;

        // Get old history back
        assert (!history.empty());
        History oldState = history.back();

        if (move.isCastle) {
            assert(to == Square::G1 || to == Square::C1 || to == Square::G8 || to == Square::C8);
            // All we need is the 'to' square: *1 -> White, *8 -> Black, C* -> Queenside, G* -> Kingside
            if (to == Square:: G1) {
                // White Kingside
                removePieceAt(to);  // Moving King back
                insertPieceAt(from, WHITE_KING);

                removePieceAt(Square::F1);  // Moving Rook back
                insertPieceAt(Square::H1, WHITE_ROOK);
            }

            else if (to == Square::C1) {
                // White Queenside
                removePieceAt(to);
                insertPieceAt(from, WHITE_KING);

                removePieceAt(Square::D1);
                insertPieceAt(Square::A1, WHITE_ROOK);
            }

            else if (to == Square::G8) {
                // Black Kingside
                removePieceAt(to);  // Moving King back
                insertPieceAt(from, BLACK_KING);

                removePieceAt(Square::F8);  // Moving Rook back
                insertPieceAt(Square::H8, BLACK_ROOK);
            }

            else if (to == Square::C8) {
                // Black Queenside
                removePieceAt(to);
                insertPieceAt(from, BLACK_KING);

                removePieceAt(Square::D8);
                insertPieceAt(Square::A8, BLACK_ROOK);
            }
        }

        else if (move.isEnPassant) {
            assert(move.movingPiece == WHITE_PAWN || move.movingPiece == BLACK_PAWN);

            removePieceAt(to);  // Move the pawn back
            insertPieceAt(from, move.movingPiece);

            // Calculate which square the piece was taken at, insert the piece back
            if (move.movingPiece == WHITE_PAWN) {
                Square captureSquare = shift(to, -8);
                insertPieceAt(captureSquare, BLACK_PAWN);
            }
            else {
                Square captureSquare = shift(to, +8);
                insertPieceAt(captureSquare, WHITE_PAWN);
            }
        }

        else {  // Promotions follow the same logic as the normal process
            removePieceAt(to);
            insertPieceAt(from, move.movingPiece);

            if (move.capturedPiece != EMPTY){
                insertPieceAt(to, move.capturedPiece);  // Default to EMPTY when there's no capture
                }
        }

        // Restore the correct en passant and castling state
        enPassantSquare = oldState.enPassantSquare;
        castlingRights = oldState.castlingRights;
        halfMoveClock = oldState.halfMoveClock;

        // Remove the history that doesn't exist anymore
        history.pop_back();

        sideToMove = opposite(sideToMove);
    }

    Piece Board::pieceAt(Square sq) const {
        return board[toIndex(sq)];
    }

    Square Board::whiteKingPosition() const {
        return getMSB(whiteKing);
    }

    Square Board::blackKingPosition() const {
        return getMSB(blackKing);
    }

    CastlingRights Board::getCastlingRights() const {
        return castlingRights;
    }

    int Board::getHalfMoveClock() const {
        return halfMoveClock;
    }

    std::vector<Move> Board::getLegalMoves() const {
        initialiseAttackTables();

        Board copy = *this;

        if (sideToMove == Colour::WHITE) {
            return generateWhiteMoves(copy);
        }
        return generateBlackMoves(copy);
    }

    void Board::assertConsistent() const {
        for (int sq = 0; sq < 64; sq++) {
            uint64_t mask = 1ULL << sq;
            Piece mailboxPiece = board[sq];
            Piece bitboardPiece = EMPTY;

            if (whitePawns & mask) bitboardPiece = WHITE_PAWN;
            else if (whiteRooks & mask) bitboardPiece = WHITE_ROOK;
            else if (whiteKnights & mask) bitboardPiece = WHITE_KNIGHT;
            else if (whiteBishops & mask) bitboardPiece = WHITE_BISHOP;
            else if (whiteQueens & mask) bitboardPiece = WHITE_QUEEN;
            else if (whiteKing & mask) bitboardPiece = WHITE_KING;
            else if (blackPawns & mask) bitboardPiece = BLACK_PAWN;
            else if (blackRooks & mask) bitboardPiece = BLACK_ROOK;
            else if (blackKnights & mask) bitboardPiece = BLACK_KNIGHT;
            else if (blackBishops & mask) bitboardPiece = BLACK_BISHOP;
            else if (blackQueens & mask) bitboardPiece = BLACK_QUEEN;
            else if (blackKing & mask) bitboardPiece = BLACK_KING;

            assert(mailboxPiece == bitboardPiece);
        }
    }

    void Board::setPosition(const std::array<Piece, 64>& position) {
        // Clear board representations
        board.fill(EMPTY);

        whitePawns = 0;
        whiteRooks = 0;
        whiteKnights = 0;
        whiteBishops = 0;
        whiteQueens = 0;
        whiteKing = 0;

        blackPawns = 0;
        blackRooks = 0;
        blackKnights = 0;
        blackBishops = 0;
        blackQueens = 0;
        blackKing = 0;

        // Rebuild mailbox + bitboards
        for (int square = 0; square < 64; ++square) {
            Piece piece = position[square];

            if (piece == EMPTY) {
                continue;
            }

            board[square] = piece;

            uint64_t mask = 1ULL << square;

            switch (piece) {
                case WHITE_PAWN:
                    whitePawns |= mask;
                    break;

                case WHITE_ROOK:
                    whiteRooks |= mask;
                    break;

                case WHITE_KNIGHT:
                    whiteKnights |= mask;
                    break;

                case WHITE_BISHOP:
                    whiteBishops |= mask;
                    break;

                case WHITE_QUEEN:
                    whiteQueens |= mask;
                    break;

                case WHITE_KING:
                    whiteKing |= mask;
                    break;

                case BLACK_PAWN:
                    blackPawns |= mask;
                    break;

                case BLACK_ROOK:
                    blackRooks |= mask;
                    break;

                case BLACK_KNIGHT:
                    blackKnights |= mask;
                    break;

                case BLACK_BISHOP:
                    blackBishops |= mask;
                    break;

                case BLACK_QUEEN:
                    blackQueens |= mask;
                    break;

                case BLACK_KING:
                    blackKing |= mask;
                    break;

                case EMPTY:
                    break;
            }
        }

        enPassantSquare = Square::NO_SQUARE;
        castlingRights = 0;
        history.clear();

        // Set this appropriately for the position you're loading.
        sideToMove = Colour::WHITE;

        assertConsistent();
    }

    void Board::setPosition(const std::array<std::string, 8>& ranks) {
        std::array<Piece, 64> position{};
        position.fill(EMPTY);

        for (int rank = 0; rank < 8; ++rank) {
            assert(ranks[rank].size() == 8);

            for (int file = 0; file < 8; ++file) {
                char c = ranks[rank][file];

                // ranks[0] = rank 8, ranks[7] = rank 1
                int square = (7 - rank) * 8 + file;

                switch (c) {
                    case 'P': position[square] = WHITE_PAWN;   break;
                    case 'R': position[square] = WHITE_ROOK;   break;
                    case 'N': position[square] = WHITE_KNIGHT; break;
                    case 'B': position[square] = WHITE_BISHOP; break;
                    case 'Q': position[square] = WHITE_QUEEN;  break;
                    case 'K': position[square] = WHITE_KING;   break;

                    case 'p': position[square] = BLACK_PAWN;   break;
                    case 'r': position[square] = BLACK_ROOK;   break;
                    case 'n': position[square] = BLACK_KNIGHT; break;
                    case 'b': position[square] = BLACK_BISHOP; break;
                    case 'q': position[square] = BLACK_QUEEN;  break;
                    case 'k': position[square] = BLACK_KING;   break;

                    case '.': position[square] = EMPTY;       break;

                    default:
                        assert(false && "Invalid piece character");
                }
            }
        }

        setPosition(position);
    }

    void Board::setEnPassantSquare(Square square) {
        enPassantSquare = square;
    }

    void Board::loadFEN(const std::string& fen) {
        std::istringstream input(fen);

        std::string placement;
        std::string activeColour;
        std::string castling;
        std::string enPassant;
        std::string halfmoveClock;
        std::string fullmoveNumber;

        if (!(input >> placement
                    >> activeColour
                    >> castling
                    >> enPassant
                    >> halfmoveClock
                    >> fullmoveNumber)) {
            throw std::invalid_argument("FEN must contain six fields");
        }

        // Reset the mailbox board and every piece bitboard.
        board.fill(EMPTY);

        whitePawns = whiteRooks = whiteKnights = 0;
        whiteBishops = whiteQueens = whiteKing = 0;

        blackPawns = blackRooks = blackKnights = 0;
        blackBishops = blackQueens = blackKing = 0;

        // Parse piece placement, starting at a8 (index 56).
        int rank = 7;
        int file = 0;

        for (char c : placement) {
            if (c == '/') {
                if (file != 8 || rank == 0) {
                    throw std::invalid_argument(
                        "Invalid FEN rank structure");
                }

                --rank;
                file = 0;
                continue;
            }

            if (c >= '1' && c <= '8') {
                file += c - '0';

                if (file > 8) {
                    throw std::invalid_argument(
                        "Invalid FEN rank width");
                }

                continue;
            }

            if (file >= 8 || rank < 0) {
                throw std::invalid_argument(
                    "Invalid FEN piece placement");
            }

            const int sq = rank * 8 + file;
            const std::uint64_t mask = 1ULL << sq;

            switch (c) {
                case 'P':
                    board[sq] = WHITE_PAWN;
                    whitePawns |= mask;
                    break;
                case 'R':
                    board[sq] = WHITE_ROOK;
                    whiteRooks |= mask;
                    break;
                case 'N':
                    board[sq] = WHITE_KNIGHT;
                    whiteKnights |= mask;
                    break;
                case 'B':
                    board[sq] = WHITE_BISHOP;
                    whiteBishops |= mask;
                    break;
                case 'Q':
                    board[sq] = WHITE_QUEEN;
                    whiteQueens |= mask;
                    break;
                case 'K':
                    board[sq] = WHITE_KING;
                    whiteKing |= mask;
                    break;

                case 'p':
                    board[sq] = BLACK_PAWN;
                    blackPawns |= mask;
                    break;
                case 'r':
                    board[sq] = BLACK_ROOK;
                    blackRooks |= mask;
                    break;
                case 'n':
                    board[sq] = BLACK_KNIGHT;
                    blackKnights |= mask;
                    break;
                case 'b':
                    board[sq] = BLACK_BISHOP;
                    blackBishops |= mask;
                    break;
                case 'q':
                    board[sq] = BLACK_QUEEN;
                    blackQueens |= mask;
                    break;
                case 'k':
                    board[sq] = BLACK_KING;
                    blackKing |= mask;
                    break;

                default:
                    throw std::invalid_argument(
                        "Invalid FEN piece character");
            }

            ++file;
        }

        if (rank != 0 || file != 8) {
            throw std::invalid_argument(
                "FEN must describe exactly eight ranks");
        }

        // Side to move.
        if (activeColour == "w") {
            sideToMove = Colour::WHITE;
        } else if (activeColour == "b") {
            sideToMove = Colour::BLACK;
        } else {
            throw std::invalid_argument(
                "Invalid FEN active colour");
        }

        // Castling rights.
        castlingRights = 0;

        if (castling != "-") {
            for (char c : castling) {
                switch (c) {
                    case 'K': castlingRights |= WK; break;
                    case 'Q': castlingRights |= WQ; break;
                    case 'k': castlingRights |= BK; break;
                    case 'q': castlingRights |= BQ; break;
                    default:
                        throw std::invalid_argument(
                            "Invalid FEN castling rights");
                }
            }
        }

        // En passant target square.
        if (enPassant == "-") {
            enPassantSquare = Square::NO_SQUARE;
        } else {
            if (enPassant.size() != 2 ||
                enPassant[0] < 'a' || enPassant[0] > 'h' ||
                enPassant[1] < '1' || enPassant[1] > '8') {
                throw std::invalid_argument(
                    "Invalid FEN en passant square");
            }

            const int epFile = enPassant[0] - 'a';
            const int epRank = enPassant[1] - '1';
            const int epIndex = epRank * 8 + epFile;

            enPassantSquare = static_cast<Square>(epIndex);
        }
    }

    bool Board::operator==(const Board& other) const {
        return
            board == other.board &&

            whitePawns == other.whitePawns &&
            whiteRooks == other.whiteRooks &&
            whiteKnights == other.whiteKnights &&
            whiteBishops == other.whiteBishops &&
            whiteQueens == other.whiteQueens &&
            whiteKing == other.whiteKing &&

            blackPawns == other.blackPawns &&
            blackRooks == other.blackRooks &&
            blackKnights == other.blackKnights &&
            blackBishops == other.blackBishops &&
            blackQueens == other.blackQueens &&
            blackKing == other.blackKing &&

            enPassantSquare == other.enPassantSquare &&
            castlingRights == other.castlingRights &&
            sideToMove == other.sideToMove &&
            halfMoveClock == other.halfMoveClock;

    }

    std::uint64_t perft(Board& board, int depth) {
        if (depth == 0) {
            return 1;
        }

        const auto moves = board.getLegalMoves();
        std::uint64_t nodes = 0;

        for (auto move : moves) {
            board.makeMove(move);
            nodes += perft(board, depth - 1);
            board.undoMove(move);
        }

        return nodes;
    }

    void divide(Board& board, int depth) {
        auto moves = board.getLegalMoves();

        uint64_t total = 0;

        for (const Chess::Move& move : moves) {
            board.makeMove(move);
            uint64_t nodes = perft(board, depth - 1);
            board.undoMove(move);

            std::cout << moveToString(move) << ": " << nodes << '\n';
            total += nodes;
        }

        std::cout << "Total: " << total << '\n';
    }

}
