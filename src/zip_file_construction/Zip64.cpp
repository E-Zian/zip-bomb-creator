//
// Created by LeeEeZian on 8/10/2026.
//
#include "zip_file_construction/Zip64.h"
#include "Helper.h"

std::vector<uint8_t> Zip64ExtraBlock::serialize() const
{
    using namespace helper;
    std::vector<uint8_t> serialized{};

    if (data.empty()) {
        return serialized;
    }

    serialized.reserve(size());

    appendBytes16Small(serialized,ZIP64_EXTENDED_INFO);
    appendBytes16Small(serialized,data.size());

    serialized.insert(serialized.end(),data.begin(),data.end());

    return serialized;
}

uint8_t Zip64ExtraBlock::size() const {
    if (data.empty()) {
        return 0;
    }
    return data.size()+4;
}
