//
// Created by LeeEeZian on 29/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_LZ77_H
#define ZIP_BOMB_CREATOR_LZ77_H

#include <array>
#include <cstdint>
#include "zip_file_construction/LocalFileHeader.h"
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

    LocalFileHeader fixedCompress(std::string_view fileName,std::span<uint8_t> byteBuffer);
}


#endif //ZIP_BOMB_CREATOR_LZ77_H
