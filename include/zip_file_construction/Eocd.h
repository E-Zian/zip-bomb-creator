//
// Created by LeeEeZian on 9/9/2026.
//
#pragma once

#ifndef ZIP_BOMB_CREATOR_EOCD_H
#define ZIP_BOMB_CREATOR_EOCD_H

#include "CentralDirectory.h"
#include <cstdint>
#include <vector>

class Eocd {
public:
    struct EocdConstructConfig {
        const std::vector<CentralDirectoryHeader>& entries;
        std::vector<uint8_t> comment;
    };

    explicit Eocd(const EocdConstructConfig& config);

    std::vector<uint8_t> serialize();

    void setTotalEntries(const uint64_t totalEntries) {
        totalEntries_ = totalEntries;
        entriesOnThisDisk_ = totalEntries;
    }

    void setCentralDirOffset(const uint64_t centralDirOffset) {
        centralDirOffset_ = centralDirOffset;
    }

    void setCentralDirSize(const uint64_t size) {
        centralDirSize_ = size;
    }

    static constexpr uint32_t signature{0x06054b50};

    static constexpr uint32_t recordSignature{0x06064b50};
    static constexpr uint32_t locatorSignature{0x07064b50};

    void setOffsetOfEocdRecord(const uint64_t offsetOfEocdRecord) {
        offsetOfEocdRecord_ = offsetOfEocdRecord;
    }

private:
    uint32_t diskNumber_{}; // 16
    uint32_t centralDirStartDisk_{}; // 16
    uint64_t entriesOnThisDisk_{}; // 16
    uint64_t totalEntries_{}; // 16
    uint64_t centralDirSize_{}; //32
    uint64_t centralDirOffset_{}; //32
    std::vector<uint8_t> comment_{};

    // Zip64 record section
    uint16_t versionMadeBy_{};
    uint16_t versionNeeded_{};

    // Zip64 locator section
    uint32_t diskWithEocdRecord_{}; // Should be just 0
    uint64_t offsetOfEocdRecord_{};
    uint32_t totalDisks_{1}; // Should be just 1

};

#endif //ZIP_BOMB_CREATOR_EOCD_H
