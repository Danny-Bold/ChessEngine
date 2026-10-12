//
// Created by Danny on 11/10/2026.
//

#include <iostream>
#include <optional>
#include <sstream>
#include <chrono>

#include "chess/Move.h"
#include "chess/Board.h"
#include "chess/UCI.h"

#include <limits>

#include "chess/Search.h"


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
            stopSearch();
            board_ = Board{};
        }
        else if (command == "position") {
            handlePosition(line);
        }
        else if (command == "go") {
            handleGo(line);
        }
        else if (command == "stop") {
            stopRequested_.store(true, std::memory_order_relaxed);
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

    void Uci::handleGo(const std::string& line) {

        std::istringstream input(line);
        std::string command;
        input >> command;

        std::string firstArg;
        if (!(input >> firstArg)) {
            std::cout << "info string Missing go parameters\n";
            return;
        }

        if (firstArg == "perft") {
            int depth;
            std::string extra;

            if (!(input >> depth) || depth < 1 || (input >> extra)) {
                std::cout << "info string Invalid perft command\n";
                return;
            }

            stopSearch();
            stopRequested_.store(false, std::memory_order_relaxed);

            divide(board_, depth);
            return;
        }

        const auto parsed = parseGoParameters(line);

        if (!parsed) {
            return;
        }

        // Prepare for the new search.
        stopSearch();
        stopRequested_.store(false);

        const GoParameters& searchParams = *parsed;

        if (searchParams.ponder) {
            std::cout << "info string Pondering not implemented\n";
        } else if (searchParams.infinite) {
            const auto moveTime = searchParams.moveTime;

            const auto deadline = moveTime.has_value() ? std::optional(Clock::now() + std::chrono::milliseconds(*moveTime)) : std::nullopt;

            searchThread_ = std::thread([this, deadline]() {
                Move result = findBestMoveInfinite(
                    board_,
                    stopRequested_,
                    deadline
                );
                std::cout << "bestmove " << moveToString(result) << '\n' << std::flush;
        });
        } else if (searchParams.depth.has_value()) {
            const int depth = *searchParams.depth;
            const auto moveTime = searchParams.moveTime;

            const auto deadline = moveTime.has_value() ? std::optional(Clock::now() + std::chrono::milliseconds(*moveTime)) : std::nullopt;

            searchThread_ = std::thread([this, depth, deadline]() {
                BestMoveResult result = findBestMoveDepth(
                    board_,
                    depth,
                    stopRequested_,
                    std::nullopt,
                    deadline
                    );
                std::cout << "bestmove " << moveToString(result.move) << '\n' << std::flush;
            });

        } else if (searchParams.moveTime.has_value()) {
            using Clock = std::chrono::steady_clock;

            const auto start = Clock::now();
            const auto deadline = start + std::chrono::milliseconds(*searchParams.moveTime);

            searchThread_ = std::thread([this, deadline]() {
                Move result = findBestMoveInfinite(board_, stopRequested_, deadline);
                std::cout << "bestmove " << moveToString(result) << '\n' << std::flush;
            });

        } else {
            std::cout << "info string Unsupported go command\n";
        }
    }

    std::optional<GoParameters> Uci::parseGoParameters(const std::string& line) {
        std::istringstream input(line);
        std::vector<std::string> tokens;
        std::string token;

        input >> token; // "go"

        while (input >> token) {
            tokens.push_back(token);
        }

        GoParameters params;

        auto isKeyword = [](const std::string& s) {
            return s == "infinite" ||
               s == "ponder" ||
               s == "depth" ||
               s == "mate" ||
               s == "nodes" ||
               s == "movetime" ||
               s == "wtime" ||
               s == "btime" ||
               s == "winc" ||
               s == "binc" ||
               s == "movestogo" ||
               s == "searchmoves" ||
               s == "perft";
        };

        auto parseNumber = [](const std::string& s) -> uint64_t {
            std::size_t pos = 0;
            unsigned long long value = std::stoull(s, &pos);

            if (pos != s.size() || s.empty() || s[0] == '-') {
                throw std::invalid_argument("Invalid number");
            }
            return value;
        };

        try {
            for (std::size_t i = 0; i < tokens.size(); ++i) {
                const std::string& key = tokens[i];

                if (key == "infinite") {
                    params.infinite = true;
                }
                else if (key == "ponder") {
                    params.ponder = true;
                }
                else if (key == "searchmoves") {
                    // Read moves until the next recognised keyword.
                    while (i + 1 < tokens.size() &&
                           !isKeyword(tokens[i + 1])) {
                        params.searchMoves.push_back(tokens[++i]);
                    }

                    if (params.searchMoves.empty()) {
                        throw std::invalid_argument(
                            "searchmoves requires at least one move"
                        );
                    }
                }
                else if (key == "depth" ||
                         key == "mate" ||
                         key == "nodes" ||
                         key == "movetime" ||
                         key == "wtime" ||
                         key == "btime" ||
                         key == "winc" ||
                         key == "binc" ||
                         key == "movestogo" ||
                         key == "perft") {

                    if (i + 1 >= tokens.size() ||
                        isKeyword(tokens[i + 1])) {
                        throw std::invalid_argument(
                            key + " requires a value"
                        );
                    }

                    uint64_t value = parseNumber(tokens[++i]);

                    if (key == "depth") {
                        if (value == 0 ||
                            value > std::numeric_limits<int>::max()) {
                            throw std::invalid_argument("Invalid depth");
                        }
                        params.depth = static_cast<int>(value);
                    }
                    else if (key == "mate") {
                        if (value == 0 ||
                            value > std::numeric_limits<int>::max()) {
                            throw std::invalid_argument("Invalid mate limit");
                        }
                        params.mate = static_cast<int>(value);
                    }
                    else if (key == "nodes") {
                        params.nodes = value;
                    }
                    else if (key == "movetime") {
                        if (value > std::numeric_limits<int>::max()) {
                            throw std::invalid_argument("Invalid movetime");
                        }
                        params.moveTime = static_cast<int>(value);
                    }
                    else if (key == "wtime") {
                        if (value > std::numeric_limits<int>::max()) {
                            throw std::invalid_argument("Invalid wtime");
                        }
                        params.wtime = static_cast<int>(value);
                    }
                    else if (key == "btime") {
                        if (value > std::numeric_limits<int>::max()) {
                            throw std::invalid_argument("Invalid btime");
                        }
                        params.btime = static_cast<int>(value);
                    }
                    else if (key == "winc") {
                        if (value > std::numeric_limits<int>::max()) {
                            throw std::invalid_argument("Invalid winc");
                        }
                        params.winc = static_cast<int>(value);
                    }
                    else if (key == "binc") {
                        if (value > std::numeric_limits<int>::max()) {
                            throw std::invalid_argument("Invalid binc");
                        }
                        params.binc = static_cast<int>(value);
                    }
                    else if (key == "movestogo") {
                        if (value == 0 ||
                            value > std::numeric_limits<int>::max()) {
                            throw std::invalid_argument("Invalid movestogo");
                        }
                        params.movesToGo = static_cast<int>(value);
                    }
                    else if (key == "perft") {
                        divide(board_, static_cast<int>(value));
                        return std::nullopt;
                    }
                }
                else {
                    throw std::invalid_argument(
                        "Unknown go parameter: " + key
                    );
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << "info string Invalid go command: "
                      << e.what() << '\n';
            return std::nullopt;
        }

        // Capture the parameters by value.
        GoParameters searchParams = params;

        return searchParams;
    }

    void Uci::stopSearch() {
        if (searchThread_.joinable()) {
            stopRequested_.store(true, std::memory_order_relaxed);
            searchThread_.join();
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
