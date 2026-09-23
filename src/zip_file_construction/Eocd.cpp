//
// Created by LeeEeZian on 9/9/2026.
//

#include "../../include/zip_file_construction/Eocd.h"
#include "Helper.h"

Eocd::Eocd(const EocdConstructConfig &config) : entriesOnThisDisk_{static_cast<uint16_t>(config.entries.size())},
                                                totalEntries_{entriesOnThisDisk_}, centralDirSize_{},
                                                comment_{config.comment} {
    for (auto &entry: config.entries) {
        centralDirSize_ += entry.size();
    }
}

std::vector<uint8_t> Eocd::serialize() {
    using namespace helper;

    std::vector<uint8_t> serialized;

    appendBytes32Small(serialized, signature);
    appendBytes16Small(serialized, diskNumber_);
    appendBytes16Small(serialized, centralDirStartDisk_);
    appendBytes16Small(serialized, entriesOnThisDisk_);
    appendBytes16Small(serialized, totalEntries_);
    appendBytes32Small(serialized, centralDirSize_);
    appendBytes32Small(serialized, centralDirOffset_);
    appendBytes16Small(serialized, static_cast<uint16_t>(comment_.size()));

    serialized.insert(serialized.end(), comment_.begin(), comment_.end());

    return serialized;
}
