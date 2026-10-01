//
// Created by LeeEeZian on 30/9/2026.
//
#include "zip_file_construction/Crc32.h"

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

    crc32Value = advanceValue(getCrc32(),8*b.length) ^ b.getCrc32();

    length += b.length;

    return *this;
}

// apply a 32x32 GF(2) matrix to a 32-bit vector
static uint32_t gf2Times(const uint32_t mat[32], uint32_t vec) {
    uint32_t sum = 0;
    for (int i = 0; vec; vec >>= 1, ++i)
        if (vec & 1) sum ^= mat[i];
    return sum;
}
static void gf2Square(uint32_t sq[32], const uint32_t mat[32]) {
    for (int n = 0; n < 32; ++n) sq[n] = gf2Times(mat, mat[n]);
}

// advance `crc` forward by `bytes` bytes, in O(log bytes)
uint32_t Crc32::advanceValue(uint32_t crc, size_t bytes) const {
    if (bytes == 0) return crc;
    uint32_t even[32], odd[32];
    odd[0] = polynomial;                 // operator for 1 zero bit
    uint32_t row = 1;
    for (int n = 1; n < 32; ++n) { odd[n] = row; row <<= 1; }
    gf2Square(even, odd);                // 2 bits
    gf2Square(odd, even);                // 4 bits
    do {
        gf2Square(even, odd);            // 8 bits (1 byte) on first pass, then doubling
        if (bytes & 1) crc = gf2Times(even, crc);
        bytes >>= 1;
        if (bytes == 0) break;
        gf2Square(odd, even);
        if (bytes & 1) crc = gf2Times(odd, crc);
        bytes >>= 1;
    } while (bytes);
    return crc;
}