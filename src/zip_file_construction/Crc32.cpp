//
// Created by LeeEeZian on 30/9/2026.
//
#include "zip_file_construction/Crc32.h"

#include <array>

void Crc32::update(const uint8_t byte) {
    state ^= byte;
    advance(8);
    length += 1;
}


void Crc32::advance(const size_t bits) {
    for (size_t i = 0; i < bits; ++i) {
        state = (state & 1) ? (state >> 1) ^ polynomial : (state >> 1);
    }
}

uint32_t Crc32::getCrc32() const {
    return crc32Value;
}

void Crc32::computeCrc32() {
    constexpr uint32_t max4Byte{0xFFFFFFFF};
    crc32Value = state ^ max4Byte;
}

Crc32 &Crc32::combine(const Crc32 &b) {
    crc32Value = advanceValue(getCrc32(), b.length) ^ b.getCrc32();

    length += b.length;

    return *this;
}

namespace {
    uint32_t matrixDotVector(const std::array<uint32_t,32> &mat, uint32_t vec) {
        uint32_t sum{};
        for (int i{}; vec; vec >>= 1, ++i) {
            if (vec & 1) {
                sum ^= mat[i];
            }
        }
        return sum;
    }

    void squareMatrix(std::array<uint32_t,32> &mat) {
        const auto tempMatrix{mat};
        for (int i{}; i<32; ++i) {
            mat[i] = matrixDotVector(tempMatrix, tempMatrix[i]);
        }
    }}

// advance `crc` forward by `bytes` bytes, in O(log bytes)
uint32_t Crc32::advanceValue(uint32_t crc, size_t bytes) const {
    if (bytes == 0) return crc;
    size_t bits { bytes * 8};
    uint32_t even[32], odd[32];
    odd[0] = polynomial; // operator for 1 zero bit
    uint32_t row = 1;
    for (int n = 1; n < 32; ++n) {
        odd[n] = row;
        row <<= 1;
    }
    squareMatrix(even, odd); // 2 bits
    squareMatrix(odd, even); // 4 bits
    do {
        squareMatrix(even, odd); // 8 bits (1 byte) on first pass, then doubling
        if (bits & 1) crc = matrixDotVector(even, crc);
        bits >>= 1;
        if (bits == 0) break;
        squareMatrix(odd, even);
        if (bits & 1) crc = matrixDotVector(odd, crc);
        bits >>= 1;
    } while (bits);
    return crc;
}
