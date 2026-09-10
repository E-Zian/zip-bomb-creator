//
// Created by LeeEeZian on 9/9/2026.
//
#pragma once

#ifndef ZIP_BOMB_CREATOR_CENTRALDIRECTORY_H
#define ZIP_BOMB_CREATOR_CENTRALDIRECTORY_H

#include <cstdint>
#include <vector>
#include "LocalFileHeader.h"

class CentralDirectoryHeader {
public:
    static constexpr uint32_t signature{0x02014b50};

    static constexpr size_t minSize{46};

    enum class Host : uint8_t {
        DOS = 0,
        WINDOWS = 0,
        UNIX = 3
    };

    struct CentralDirectoryConstructConfig {
        const LocalFileHeader &localHeader;
        uint16_t versionMadeBy;
        std::vector<uint8_t> extraField;
        std::vector<uint8_t> fileComment;
        uint16_t diskNumberStart;
        uint16_t internalAttributes;
        uint32_t externalAttributes;
    };

    static CentralDirectoryHeader createBasic(const LocalFileHeader &localHeader);

    explicit CentralDirectoryHeader(const CentralDirectoryConstructConfig &config);

    std::vector<uint8_t> serialize();

    [[nodiscard]] size_t size() const;

    void setLocalHeaderOffset(const uint32_t offset) {
        localHeaderOffset_ = offset;
    }

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
