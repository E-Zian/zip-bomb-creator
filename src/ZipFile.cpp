//
// Created by LeeEeZian on 10/9/2026.
//

#include "ZipFile.h"

void ZipFile::addFile(LocalFileHeader &&fileHeader) {
    centralDirectories_.push_back(CentralDirectoryHeader::createBasic(fileHeader));

    localFileHeaders_.push_back(std::move(fileHeader));
}

std::optional<std::vector<uint8_t> > ZipFile::serialize() {
    if (centralDirectories_.empty() || localFileHeaders_.empty() || centralDirectories_.size() != localFileHeaders_.
        size()) { return std::nullopt; }

    std::vector<uint8_t> serialized;

    Eocd eocd{{.entries = centralDirectories_, .comment = {}}};

    for (size_t i{}; i < localFileHeaders_.size(); ++i) {
        centralDirectories_[i].setLocalHeaderOffset(serialized.size());
        std::vector<uint8_t> tempLocalFileHeaders{localFileHeaders_[i].serialize()};
        serialized.insert(serialized.end(), tempLocalFileHeaders.begin(), tempLocalFileHeaders.end());
    }

    eocd.setCentralDirOffset(serialized.size());

    for (size_t i{}; i < centralDirectories_.size(); ++i) {
        std::vector<uint8_t> tempCentralDirHeaders{centralDirectories_[i].serialize()};
        serialized.insert(serialized.end(), tempCentralDirHeaders.begin(), tempCentralDirHeaders.end());
    }
    const std::vector<uint8_t> eocdSerialized{eocd.serialize()};

    serialized.insert(serialized.end(), eocdSerialized.begin(), eocdSerialized.end());
    return serialized;
}
