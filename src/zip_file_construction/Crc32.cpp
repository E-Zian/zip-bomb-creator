//
// Created by LeeEeZian on 30/9/2026.
//
#include "zip_file_construction/Crc32.h"

void Crc32::update(const uint8_t byte) {
    state^= byte;
    for (int i{}; i < 8; ++i) {
        if (state & 1) {
            state = (state >> 1) ^ polynomial;
        } else {
            state >>= 1;
        }
    }
}

void Crc32::finalize() {
    constexpr uint32_t max4Byte{0xFFFFFFFF};
    finalValue = state ^ max4Byte;
}

uint32_t Crc32::getCrc32() const {
    return finalValue;
}
