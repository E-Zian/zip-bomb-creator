//
// Created by LeeEeZian on 23/9/2026.
//

#include "deflation/BitWriter.h"

#include <format>

void BitWriter::add(uint32_t value, int numOfBits) {
    if (numOfBits <= 0) {
        return;
    }

    while (numOfBits > 0) {
        const int remainingBits { 8 - bitCount_};
        const int take { std::min(numOfBits, remainingBits)};

        bitBuffer_ |= (value & (1u << take)-1) << bitCount_;
        value >>= take;
        numOfBits -= take;

        bitCount_ += take;

        if (bitCount_ == 8) {
            bits_.push_back(bitBuffer_);
            bitCount_ = 0;
            bitBuffer_ = 0;
        }
    }
}

std::vector<uint8_t> BitWriter::release() {
    if (bitCount_ > 0) {
    bits_.push_back(bitBuffer_);
        bitBuffer_ = 0;
        bitCount_  = 0;
    }
    return std::move(bits_);
}
