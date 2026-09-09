//
// Created by LeeEeZian on 9/9/2026.
//
#pragma once

#ifndef ZIP_BOMB_CREATOR_HELPER_H
#define ZIP_BOMB_CREATOR_HELPER_H

#include <cstdint>
#include <vector>

namespace helper {
    void appendBytes16Small(std::vector<uint8_t> &buffer, uint16_t data);
    void appendBytes32Small(std::vector<uint8_t> &buffer, uint32_t data);
}


#endif //ZIP_BOMB_CREATOR_HELPER_H
