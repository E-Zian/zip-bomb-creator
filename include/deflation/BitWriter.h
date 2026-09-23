//
// Created by LeeEeZian on 23/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_BITWRITER_H
#define ZIP_BOMB_CREATOR_BITWRITER_H

#include <cstdint>
#include <vector>
class BitWriter {
    public:
    BitWriter() = default;

    void add(uint32_t value,int numOfBits);

    std::vector<uint8_t> release();

private:
    std::vector<uint8_t> bits_{};
    uint8_t bitBuffer_{};
    int bitCount_{};
};
#endif //ZIP_BOMB_CREATOR_BITWRITER_H
