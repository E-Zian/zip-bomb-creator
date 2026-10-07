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
    crc32Value = advanceValue(getCrc32(), 8 * b.length) ^ b.getCrc32();

    length += b.length;

    return *this;
}

namespace {
    uint32_t matrixDotVector(const std::array<uint32_t, 32> &mat, uint32_t vec) {
        uint32_t sum{};
        for (int i{}; vec; vec >>= 1, ++i) {
            if (vec & 1) {
                sum ^= mat[i];
            }
        }
        return sum;
    }

    void squareMatrix(std::array<uint32_t, 32> &mat) {
        const auto tempMatrix{mat};
        for (int i{}; i < 32; ++i) {
            mat[i] = matrixDotVector(tempMatrix, mat[i]);
        }
    }

    void matrixDotMatrix(std::array<uint32_t, 32> &mat1, const std::array<uint32_t, 32> &mat2) {
        const auto tempMatrix{mat1};

        for (int i{}; i < 32; ++i) {
            mat1[i] = matrixDotVector(mat2, tempMatrix[i]);
        }
    }
}


uint32_t Crc32::advanceValue(uint32_t crc, size_t bits) const {
    if (bits == 0) return crc;
    std::array<uint32_t, 32> advanceMatrix{};

    advanceMatrix[0] = polynomial;

    // Set as the advance function matrix
    for (size_t i{1}; i < advanceMatrix.size(); ++i) {
        advanceMatrix[i] = 1u << (i-1);
    }

    while (bits > 0) {
        if (bits & 1) {
            crc = matrixDotVector(advanceMatrix, crc);
        }
        bits >>= 1;

        if (bits) {
            squareMatrix(advanceMatrix);
        }
    }

    return crc;
}
