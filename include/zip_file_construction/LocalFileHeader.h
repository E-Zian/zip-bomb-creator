//
// Created by LeeEeZian on 9/9/2026.
//
#pragma once
#ifndef ZIP_BOMB_CREATOR_LOCALFILEHEADER_H
#define ZIP_BOMB_CREATOR_LOCALFILEHEADER_H

#include "Crc32.h"
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <vector>


class LocalFileHeader {
public:
    struct FileHeaderConstructConfig {
        uint16_t version;
        uint16_t flags;
        uint16_t compressionMethod;
        uint16_t modTime;
        uint16_t modDate;
        uint32_t compressedSize;
        uint32_t uncompressedSize;
        std::vector<uint8_t> fileName;
        std::vector<uint8_t> extraField;
        std::vector<uint8_t> data;
        std::optional<uint32_t> crc32;
    };

    enum class Version :uint8_t {
        ORIGINAL = 10,
        DEFLATE = 20,
        ZIP64 = 45
    };

    enum class Compression:uint8_t {
        STORED = 0,
        DEFLATE = 8,
        DEFLATE64 = 9,
    };

    // static  uint32_t calculateCRC32(std::span<const uint8_t> data);

    explicit LocalFileHeader(const FileHeaderConstructConfig &config);

    static constexpr uint32_t signature{0x04034b50};

    std::vector<uint8_t> serialize();

    static LocalFileHeader createStored(std::string fileName, std::string data);

    static LocalFileHeader createBomb(std::string fileName,size_t size);

    [[nodiscard]] uint16_t getVersion() const {
        return version_;
    };

    [[nodiscard]] uint16_t getFlags() const {
        return flags_;
    }

    [[nodiscard]] uint16_t getCompressionMethod() const {
        return compressionMethod_;
    }

    [[nodiscard]] uint16_t getModTime() const {
        return modTime_;
    }

    [[nodiscard]] uint16_t getModDate() const {
        return modDate_;
    }

    [[nodiscard]] uint32_t getCrc32() const {
        return crc32_;
    }

    [[nodiscard]] uint32_t getCompressedSize() const {
        return compressedSize_;
    }

    [[nodiscard]] uint32_t getUncompressedSize() const {
        return uncompressedSize_;
    }

    [[nodiscard]] std::vector<uint8_t> getFileName() const {
        return fileName_;
    }

    [[nodiscard]] std::vector<uint8_t> getExtraField() const {
        return extraField_;
    }

private:
    uint16_t version_{};
    uint16_t flags_{};
    uint16_t compressionMethod_{};
    uint16_t modTime_{};
    uint16_t modDate_{};
    uint32_t crc32_{};
    uint32_t compressedSize_{};
    uint32_t uncompressedSize_{};

    std::vector<uint8_t> fileName_{};
    std::vector<uint8_t> extraField_{};

    std::vector<uint8_t> data_{};
};

#endif //ZIP_BOMB_CREATOR_LOCALFILEHEADER_H
