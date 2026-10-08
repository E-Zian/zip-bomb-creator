//
// Created by LeeEeZian on 8/10/2026.
//

#ifndef ZIP_BOMB_CREATOR_ZIP64_H
#define ZIP_BOMB_CREATOR_ZIP64_H
#include <cstdint>
#include <vector>

struct Zip64ExtraBlock {
    enum ID_NAME : uint16_t {
        ZIP64_EXTENDED_INFO = 0x0001,
        NTFS = 0x000A,
        UT = 0x5455,
        UX = 0x7875,
        WINZIP_AES = 0x9901,
    };

    std::vector<uint8_t> serialize() const;

    uint8_t size() const;

    uint16_t id{};
    std::vector<uint8_t> data{};
};
#endif //ZIP_BOMB_CREATOR_ZIP64_H
