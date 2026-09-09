//
// Created by LeeEeZian on 9/9/2026.
//
#pragma once

#ifndef ZIP_BOMB_CREATOR_CENTRALDIRECTORY_H
#define ZIP_BOMB_CREATOR_CENTRALDIRECTORY_H

#include <cstdint>
#include <vector>

class CentralDirectoryHeader {
public:
    static constexpr uint32_t kSignature{0x02014b50};
private:
    uint16_t versionMadeBy_{};
    uint16_t versionNeeded_{};
    uint16_t flags_{};
    uint16_t compressionMethod_{};
    uint16_t modTime_{};
    uint16_t modDate_{};
    uint32_t crc32_{};
    uint32_t compressedSize_{};
    uint32_t uncompressedSize_{};
    // name/extra/comment lengths computed from the vectors at serialize time
    uint16_t diskNumberStart_{};
    uint16_t internalAttributes_{};
    uint32_t externalAttributes_{};
    uint32_t localHeaderOffset_{};
    std::vector<uint8_t> fileName_{};
    std::vector<uint8_t> extraField_{};
    std::vector<uint8_t> fileComment_{};
};
#endif //ZIP_BOMB_CREATOR_CENTRALDIRECTORY_H
