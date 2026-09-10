//
// Created by LeeEeZian on 9/9/2026.
//
#include "CentralDirectory.h"
#include "Helper.h"

CentralDirectoryHeader CentralDirectoryHeader::createBasic(const LocalFileHeader &localHeader) {
    CentralDirectoryConstructConfig config{.localHeader = localHeader};
    config.versionMadeBy = (static_cast<uint16_t>(Host::WINDOWS) << 8) | localHeader.getVersion();
    config.extraField = {};
    config.fileComment = {};
    config.diskNumberStart = 0;
    config.internalAttributes = 0;
    config.externalAttributes = 0;

    return CentralDirectoryHeader(config);
}

CentralDirectoryHeader::CentralDirectoryHeader(const CentralDirectoryConstructConfig &config) : versionMadeBy_
    {config.versionMadeBy}, versionNeeded_{config.localHeader.getVersion()},
    flags_{config.localHeader.getFlags()},
    compressionMethod_{config.localHeader.getCompressionMethod()},
    modTime_{config.localHeader.getModTime()},
    modDate_{config.localHeader.getModDate()},
    crc32_{config.localHeader.getCrc32()},
    compressedSize_{config.localHeader.getCompressedSize()},
    uncompressedSize_{config.localHeader.getUncompressedSize()},
    diskNumberStart_{config.diskNumberStart},
    internalAttributes_{config.internalAttributes},
    externalAttributes_{config.externalAttributes},
    fileName_{config.localHeader.getFileName()},
    extraField_{config.extraField},
    fileComment_{config.fileComment} {
}

std::vector<uint8_t> CentralDirectoryHeader::serialize() {
    using namespace helper;

    std::vector<uint8_t> serialized;

    appendBytes32Small(serialized, signature);
    appendBytes16Small(serialized, versionMadeBy_);
    appendBytes16Small(serialized, versionNeeded_);
    appendBytes16Small(serialized, flags_);
    appendBytes16Small(serialized, compressionMethod_);
    appendBytes16Small(serialized, modTime_);
    appendBytes16Small(serialized, modDate_);
    appendBytes32Small(serialized, crc32_);
    appendBytes32Small(serialized, compressedSize_);
    appendBytes32Small(serialized, uncompressedSize_);
    appendBytes16Small(serialized, static_cast<uint16_t>(fileName_.size()));
    appendBytes16Small(serialized, static_cast<uint16_t>(extraField_.size()));
    appendBytes16Small(serialized, static_cast<uint16_t>(fileComment_.size()));
    appendBytes16Small(serialized, diskNumberStart_);
    appendBytes16Small(serialized, internalAttributes_);
    appendBytes32Small(serialized, externalAttributes_);
    appendBytes32Small(serialized, localHeaderOffset_);
    serialized.insert(serialized.end(), fileName_.begin(), fileName_.end());
    serialized.insert(serialized.end(), extraField_.begin(), extraField_.end());
    serialized.insert(serialized.end(), fileComment_.begin(), fileComment_.end());

    return serialized;
}

size_t CentralDirectoryHeader::size() const {
    return minSize + fileName_.size() + extraField_.size() + fileComment_.size();
}
