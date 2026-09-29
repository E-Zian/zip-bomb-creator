//
// Created by LeeEeZian on 29/9/2026.
//

#include "deflation\Lz77.h""

lz77::MatchCode lz77::getLengthCode(const int length){
    constexpr std::array base {
        3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,
        35,43,51,59,67,83,99,115,131,163,195,227,258
    };

    constexpr std::array extra {
        0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,
        3,3,3,3,4,4,4,4,5,5,5,5,0
    };

    size_t i { base.size()-1 };
    while (base[i] > length) --i;
    return { .symbol = 257 +  static_cast<int>(i), .extraBit = extra[i], .extraValue = length - base[i] };
}
lz77::MatchCode lz77::getDistanceCode(const int distance) {
    constexpr std::array base {
        1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,
        513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577
    };
    constexpr std::array extra {
        0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,
        8,8,9,9,10,10,11,11,12,12,13,13
    };

    size_t i { base.size()-1 };
    while (base[i] > distance) --i;

    return {.symbol = static_cast<int>(i),.extraBit = extra[i],.extraValue = distance-base[i]};
}