//
// Created by LeeEeZian on 30/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_PAYLOADFACTORY_H
#define ZIP_BOMB_CREATOR_PAYLOADFACTORY_H

#include "deflation/HuffmanTable.h"
#include <vector>

#include "zip_file_construction/Crc32.h"


namespace payloadFactory {

    struct BombPayloadRecipe {
        std::vector<uint8_t> start;
        std::vector<uint8_t> period;
        std::vector<uint8_t> end;

        size_t kbSize{};
        size_t numberOfPeriods{};
    };

    struct CreateBombPayloadResult{
        BombPayloadRecipe bombRecipe;
        size_t uncompressedSize{};
        size_t compressedSize{};
        Crc32 crc32{};
    };

    CreateBombPayloadResult createFixedBombPayload(size_t size);

    CreateBombPayloadResult createDynamicBombPayload(size_t size);
}


#endif //ZIP_BOMB_CREATOR_PAYLOADFACTORY_H
