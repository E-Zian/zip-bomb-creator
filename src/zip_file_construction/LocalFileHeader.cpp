//
// Created by LeeEeZian on 9/9/2026.
//
#include "../../include/zip_file_construction/LocalFileHeader.h"
#include "Helper.h"
#include "deflation/BitWriter.h"
#include <span>


uint32_t LocalFileHeader::calculateCRC32(const std::span<const uint8_t> data) {
    constexpr uint32_t max4Byte{0xFFFFFFFF};

    uint32_t crc{max4Byte};

    for (const uint8_t byte: data) {
        crc ^= byte;
        for (int i{}; i < 8; ++i) {
            if (crc & 1) {
                constexpr uint32_t polynomial{0xEDB88320};
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    crc = crc ^ max4Byte;

    return crc;
}

LocalFileHeader::LocalFileHeader(const FileHeaderConstructConfig &config) : version_{config.version},
                                                                            flags_{config.flags},
                                                                            compressionMethod_{
                                                                                config.compressionMethod
                                                                            }, modTime_{config.modTime},
                                                                            modDate_{config.modDate},
                                                                            crc32_{calculateCRC32(data_)},
                                                                            compressedSize_{config.compressedSize},
                                                                            uncompressedSize_{config.uncompressedSize},
                                                                            fileName_{config.fileName},
                                                                            extraField_{config.extraField},
                                                                            data_{config.data} {
    crc32_ = calculateCRC32(data_);
}

std::vector<uint8_t> LocalFileHeader::serialize() {
    using namespace helper;

    std::vector<uint8_t> serialized;

    appendBytes32Small(serialized, signature);
    appendBytes16Small(serialized, version_);
    appendBytes16Small(serialized, flags_);
    appendBytes16Small(serialized, compressionMethod_);
    appendBytes16Small(serialized, modTime_);
    appendBytes16Small(serialized, modDate_);

    crc32_ = calculateCRC32(data_);
    appendBytes32Small(serialized, crc32_);

    appendBytes32Small(serialized, compressedSize_);
    appendBytes32Small(serialized, uncompressedSize_);
    appendBytes16Small(serialized, static_cast<uint16_t>(fileName_.size()));
    appendBytes16Small(serialized, static_cast<uint16_t>(extraField_.size()));

    serialized.insert(serialized.end(), fileName_.begin(), fileName_.end());
    serialized.insert(serialized.end(), extraField_.begin(), extraField_.end());

    serialized.insert(serialized.end(), data_.begin(), data_.end());

    return serialized;
}

LocalFileHeader LocalFileHeader::createStored(std::string fileName, std::string data) {
    FileHeaderConstructConfig config{};
    config.version = static_cast<uint16_t>(Version::DEFLATE);
    config.flags = 0;
    config.compressionMethod = static_cast<uint16_t>(Compression::STORED);
    config.modTime = 0;
    config.modDate = 0;

    config.compressedSize = static_cast<uint32_t>(data.size());
    config.uncompressedSize = static_cast<uint32_t>(data.size());
    config.fileName.assign(fileName.begin(), fileName.end());
    config.extraField = {};

    config.data.assign(data.begin(), data.end());

    return LocalFileHeader{config};
}

LocalFileHeader LocalFileHeader::createBomb(std::string fileName, const size_t size) {
    DeflateResult bomb { HuffmanTable::createEncodedFixedBomb(size)};
    BitWriter writer{};
    FileHeaderConstructConfig config{};
    config.version = static_cast<uint16_t>(Version::DEFLATE);
    config.flags = 0;
    config.compressionMethod = static_cast<uint16_t>(Compression::DEFLATE);
    config.modTime = 0;
    config.modDate = 0;

    config.compressedSize = static_cast<uint32_t>(bomb.size());
    config.uncompressedSize = static_cast<uint32_t>(1+repeatedData );
    config.fileName.assign(fileName.begin(), fileName.end());
    config.extraField = {};

    config.data.assign(bomb.begin(), bomb.end());

    return LocalFileHeader{config};
}
