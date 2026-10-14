//
// Created by LeeEeZian on 9/9/2026.
//

#include "../../include/zip_file_construction/Eocd.h"
#include "Helper.h"

Eocd::Eocd(const EocdConstructConfig &config) : entriesOnThisDisk_{static_cast<uint16_t>(config.entries.size())},
                                                totalEntries_{entriesOnThisDisk_}, centralDirSize_{},
                                                comment_{config.comment},
                                                versionMadeBy_{
                                                    config.entries.at(0).getVersionMadeBy()
                                                },
                                                versionNeeded_{config.entries.at(0).getVersionNeeded()} {
}

size_t Eocd::serializeTo(std::ofstream& out) {
    using namespace helper;

    std::vector<uint8_t> serialized;
    const bool totalEntriesOverflow{totalEntries_ >= 0xFFFF};
    const bool centralDirSizeOverflow{centralDirSize_ >= 0xFFFFFFFF};
    const bool centralDirOffsetOverflow{centralDirOffset_ >= 0xFFFFFFFF};

    const bool useZip64{totalEntriesOverflow || centralDirSizeOverflow || centralDirOffsetOverflow};

    if (useZip64) {
        // Zip 64 Records
        appendBytes32Small(serialized, recordSignature);
        // Is 44 now, but might need to dynamically calculate it in future if using extensible data section
        constexpr uint64_t sizeOfRecord{44};
        appendBytes64Small(serialized, sizeOfRecord);
        appendBytes16Small(serialized, versionMadeBy_);
        appendBytes16Small(serialized, versionNeeded_);
        appendBytes32Small(serialized, diskNumber_);
        appendBytes32Small(serialized, centralDirStartDisk_);
        appendBytes64Small(serialized, entriesOnThisDisk_);
        appendBytes64Small(serialized, totalEntries_);
        appendBytes64Small(serialized, centralDirSize_);
        appendBytes64Small(serialized, centralDirOffset_);

        // Zip 64 Locator

        appendBytes32Small(serialized, locatorSignature);
        appendBytes32Small(serialized, diskWithEocdRecord_);
        appendBytes64Small(serialized, offsetOfEocdRecord_);
        appendBytes32Small(serialized, totalDisks_);
    }

    appendBytes32Small(serialized, signature);
    appendBytes16Small(serialized, diskNumber_);
    appendBytes16Small(serialized, centralDirStartDisk_);

    if (totalEntriesOverflow) {
        appendBytes16Small(serialized, 0xFFFF);
        appendBytes16Small(serialized, 0xFFFF);
    } else {
        appendBytes16Small(serialized, entriesOnThisDisk_);
        appendBytes16Small(serialized, totalEntries_);
    }

    if (centralDirSizeOverflow) {
        appendBytes32Small(serialized, 0xFFFFFFFF);
    } else {
        appendBytes32Small(serialized, centralDirSize_);
    }

    if (centralDirOffsetOverflow) {
        appendBytes32Small(serialized, 0xFFFFFFFF);
    } else {
        appendBytes32Small(serialized, centralDirOffset_);
    }

    appendBytes16Small(serialized, static_cast<uint16_t>(comment_.size()));


    serialized.insert(serialized.end(), comment_.begin(), comment_.end());

    writeFile(out, serialized);

    return serialized.size();
}
