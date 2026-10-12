//
// Created by Danny on 12/10/2026.
//

#include <random>
#include <array>

#include "chess/Zobrist.h"


namespace Chess::Zobrist {

    using Hash = std::uint64_t;

    std::array<std::array<Hash, 64>, 12> pieceSquare{};
    Hash sideToMove{};
    std::array<Hash, 16> castlingRights{};
    std::array<Hash, 8> enPassantFile{};

    void initialiseZobristHashes() {
        static const bool initialised = [] {
            // Fixed seed makes engine runs reproducible.
            std::mt19937_64 gen{0xC4CEB9FE1A85EC53ULL};

            auto randomHash = [&gen]() {
                return gen();
            };

            for (auto& piece : pieceSquare) {
                for (Hash& key : piece) {
                    key = randomHash();
                }
            }

            sideToMove = randomHash();

            for (Hash& key : castlingRights) {
                key = randomHash();
            }

            for (Hash& key : enPassantFile) {
                key = randomHash();
            }

            return true;
        }();

        (void)initialised;
    }
}
