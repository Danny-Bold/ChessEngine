//
// Created by Danny on 06/06/2026.
//

#pragma once
#include <cstdint>

namespace Chess {
    extern uint64_t whitePawnAttacks[64];
    extern uint64_t blackPawnAttacks[64];

    extern uint64_t knightAttacks[64];

    extern uint64_t bishopNE[64];
    extern uint64_t bishopSE[64];
    extern uint64_t bishopNW[64];
    extern uint64_t bishopSW[64];

    extern uint64_t rookN[64];
    extern uint64_t rookE[64];
    extern uint64_t rookS[64];
    extern uint64_t rookW[64];

    extern uint64_t kingAttacks[64];

    void initialiseAttackTables();
    void initialise();
    void initialisePawnAttacks();
    void initialiseKnightAttacks();
    void initialiseBishopAttacks();
    void initialiseRookAttacks();
    void initialiseKingAttacks();
}
