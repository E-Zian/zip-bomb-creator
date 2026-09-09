//
// Created by LeeEeZian on 9/9/2026.
//
#pragma once

#ifndef ZIP_BOMB_CREATOR_EOCD_H
#define ZIP_BOMB_CREATOR_EOCD_H

#include <cstdint>
#include <vector>

class Eocd {
public:
    static constexpr uint32_t signature{0x06054b50};
private:
    uint16_t diskNumber_{};
    uint16_t centralDirStartDisk_{};
    uint16_t entriesOnThisDisk_{};
    uint16_t totalEntries_{};
    uint32_t centralDirSize_{};
    uint32_t centralDirOffset_{};
    std::vector<uint8_t> comment_{};

};

#endif //ZIP_BOMB_CREATOR_EOCD_H
