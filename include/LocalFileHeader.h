//
// Created by LeeEeZian on 9/9/2026.
//
#pragma once
#ifndef ZIP_BOMB_CREATOR_LOCALFILEHEADER_H
#define ZIP_BOMB_CREATOR_LOCALFILEHEADER_H

#include <cstdint>
#include <vector>


class LocalFileHeader {
    public:
    static constexpr uint32_t signature {0x04034b50};

    std::vector<uint8_t> serialize();

    enum class Version :uint8_t{
        ORIGINAL = 10,
        DEFLATE = 20,
        ZIP64 = 45
    };

    enum class Compression:uint8_t {
        STORED = 0,
        DEFLATE = 8,
        DEFLATE64 = 9,
    };
private:
    uint16_t version_{};
    uint16_t flags_{};
    uint16_t compressionMethod_{};
    uint16_t modTime_{};
    uint16_t modDate_{};
    // Checksum
    uint32_t crc32_{};
    uint32_t compressedSize_{};
    uint32_t uncompressedSize_{};

    std::vector<uint8_t> fileName_{};
    std::vector<uint8_t> extraField_{};

    std::vector<uint8_t> data_{};
};

#endif //ZIP_BOMB_CREATOR_LOCALFILEHEADER_H
