//
// Created by Danny on 05/06/2026.
//

#pragma once

#include <bit>
#include <cstdint>
#include "constants.h"

namespace Chess {
    Square popLSB(uint64_t& bb);
    Square popMSB(uint64_t& bb);

    Square getLSB(const uint64_t& bb);
    Square getMSB(const uint64_t& bb);

    void printBitboard(uint64_t bb);
}