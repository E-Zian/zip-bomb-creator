//
// Created by LeeEeZian on 29/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_LZ77_H
#define ZIP_BOMB_CREATOR_LZ77_H

#include <array>
#include <cstdint>
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


#endif //ZIP_BOMB_CREATOR_LZ77_H
