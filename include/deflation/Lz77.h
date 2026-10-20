//
// Created by LeeEeZian on 29/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_LZ77_H
#define ZIP_BOMB_CREATOR_LZ77_H

#include <array>
#include <cstdint>

#include "Helper.h"

namespace lz77 {
    struct MatchCode {
        int symbol;
        int extraBit;
        int extraValue;
    };
    enum class LengthValue :int{
        MAX = 258
    };
    MatchCode getLengthCode(int length);

    MatchCode getDistanceCode(int distance);
}

class Lz77 {
    public:
private:
    std::array<uint8_t, static_cast<int>(helper::ByteSize::KB)*32> ringBuffer {};
};


#endif //ZIP_BOMB_CREATOR_LZ77_H
