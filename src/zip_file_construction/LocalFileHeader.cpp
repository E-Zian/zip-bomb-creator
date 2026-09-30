//
// Created by LeeEeZian on 9/9/2026.
//
#include "zip_file_construction/LocalFileHeader.h"
#include "factory/PayloadFactory.h"
#include "Helper.h"
#include "deflation/BitWriter.h"
#include <span>

LocalFileHeader::LocalFileHeader(const FileHeaderConstructConfig &config) : version_{config.version},
                                                                            flags_{config.flags},
                                                                            compressionMethod_{
                                                                                config.compressionMethod
                                                                            },
                                                                            modTime_{config.modTime},
                                                                            modDate_{config.modDate},
                                                                            compressedSize_{config.compressedSize},
                                                                            uncompressedSize_{config.uncompressedSize},
                                                                            fileName_{config.fileName},
                                                                            extraField_{config.extraField},
                                                                            data_{config.data} {
    if (config.crc32.has_value()) {
        crc32_ = config.crc32.value();
    } else {
        Crc32 crc{};
        for (const uint8_t byte : data_) {
            crc.update(byte);
        }
        crc.finalize();
        crc32_ = crc.getCrc32();
    }
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
    config.version = static_cast<uint16_t>(Version::ORIGINAL);
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
    auto [data, uncompressedSize, crc32]{payloadFactory::createFixedBombPayload(size)};
    FileHeaderConstructConfig config{};
    config.version = static_cast<uint16_t>(Version::DEFLATE);
    config.flags = 0;
    config.compressionMethod = static_cast<uint16_t>(Compression::DEFLATE);
    config.modTime = 0;
    config.modDate = 0;

    config.compressedSize = static_cast<uint32_t>(data.size());
    config.uncompressedSize = static_cast<uint32_t>(uncompressedSize);
    config.fileName.assign(fileName.begin(), fileName.end());
    config.extraField = {};

    config.data.assign(data.begin(), data.end());
    config.crc32 = crc32;
    return LocalFileHeader{config};
}
