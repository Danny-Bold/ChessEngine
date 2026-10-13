//
// Created by Danny on 11/10/2026.
//

#pragma once

#include <optional>
#include <atomic>
#include <thread>

#include "chess/GoParameters.h"
#include "chess/Search.h"
#include "chess/Move.h"
#include "chess/Board.h"


namespace Chess {


    class Uci {
    public:
        explicit Uci(Board& board);
        void run();

        void stopSearch();

    private:
        Board& board_;

        std::atomic<bool> stopRequested_{false};
        std::thread searchThread_;
        SearchTiming timing_;

        std::optional<GoParameters> activeSearchParams_;

        bool handleCommand(const std::string& line);
        void handlePosition(const std::string& line);
        void handleGo(const std::string& line);
        void handlePonderHit();
    };

    std::optional<GoParameters> parseGoParameters(const std::string &line);

    void printBestMove(const BestMoveResult& result);

    std::optional<Move> parseUCIMove(const Board& board, std::string_view notation);

}
