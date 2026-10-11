//
// Created by Danny on 11/10/2026.
//

#include <iostream>
#include <optional>
#include <sstream>

#include "chess/Move.h"
#include "chess/Board.h"
#include "chess/UCI.h"


namespace Chess {
    Uci::Uci(Board& board) : board_(board) {}
    void Uci::run() {
        std::string line;

        while (std::getline(std::cin, line)) {
            try {
                if (!handleCommand(line)) {
                    break;
                }
            } catch (const std::exception& e) {
                std::cout << "ERROR: " << e.what() << '\n';
                std::cout.flush();
            }
        }
    }

    bool Uci::handleCommand(const std::string& line) {
        std::istringstream input(line);
        std::string command;

        if (!(input >> command)) {
            return true; // Ignore empty lines.
        }

        if (command == "uci") {
            std::cout << "uciok\n";
        }
        else if (command == "isready") {
            std::cout << "readyok\n";
        }
        else if (command == "ucinewgame") {
            board_ = Board{};
        }
        else if (command == "position") {
            handlePosition(line);
        }
        else if (command == "go") {
            // TODO: invoke the search algorithm.
        }
        else if (command == "stop") {
            // TODO: stop an active search.
        }
        else if (command == "quit") {
            return false;
        }
        else if (command == "ponderhit") {
            // TODO: the player played the best expected move.
        }

        else if (command == "d") {
            board_.print();
        }

        std::cout.flush();
        return true;
    }

    void Uci::handlePosition(const std::string& line) {
        std::istringstream input(line);
        std::string token;

        input >> token; // "position"
        input >> token; // "startpos" or "fen"

        if (token == "startpos") {
            board_ = Board{};
        }
        else if (token == "fen") {
            std::string fen;

            for (int i = 0; i < 6; ++i) {
                if (!(input >> token)) {
                    throw std::invalid_argument("Incomplete FEN string");
                }

                if (i > 0) {
                    fen += ' ';
                }
                fen += token;
            }

            board_.loadFEN(fen);
        }
        else {
            throw std::invalid_argument("Unknown position format");
        }

        while (input >> token) {
            if (token == "moves") {
                break;
            }
        }

        while (input >> token) {
            board_.makeMove(token);
        }
    }

    std::optional<Move> parseUCIMove(const Board& board, std::string_view notation) {
        // Validate the notation's basic structure.
        if (notation.size() != 4 && notation.size() != 5) {
            return std::nullopt;
        }

        // Validate the coordinates.
        if (notation[0] < 'a' || notation[0] > 'h' ||
            notation[2] < 'a' || notation[2] > 'h' ||
            notation[1] < '1' || notation[1] > '8' ||
            notation[3] < '1' || notation[3] > '8') {
            return std::nullopt;
        }

        const auto squareFromCoords = [](char file, char rank) {
            return static_cast<Square>(
                (rank - '1') * 8 + (file - 'a')
            );
        };

        const Square start = squareFromCoords(notation[0], notation[1]);
        const Square end   = squareFromCoords(notation[2], notation[3]);

        // Parse the optional promotion suffix.
        char promotion = '\0';

        if (notation.size() == 5) {
            promotion = notation[4];

            if (promotion != 'q' && promotion != 'r' &&
                promotion != 'b' && promotion != 'n') {
                return std::nullopt;
            }
        }

        // Resolve the notation against the legal moves.
        for (const Move& move : board.getLegalMoves()) {
            if (move.from != start ||
                move.to != end) {
                continue;
            }

            // Adapt this comparison to your Move representation.
            const Piece promoted = move.promotionPiece;

            if (promotion == '\0') {
                if (promoted == EMPTY) {
                    return move;
                }
            } else {
                const char expected =
                    promoted == WHITE_QUEEN || promoted == BLACK_QUEEN ? 'q' :
                    promoted == WHITE_ROOK  || promoted == BLACK_ROOK  ? 'r' :
                    promoted == WHITE_BISHOP || promoted == BLACK_BISHOP ? 'b' :
                    promoted == WHITE_KNIGHT || promoted == BLACK_KNIGHT ? 'n' :
                    '\0';

                if (expected == promotion) {
                    return move;
                }
            }
        }

        return std::nullopt;
    }
}