//
// Created by LeeEeZian on 20/10/2026.
//

#ifndef ZIP_BOMB_CREATOR_RINGBUFFER_H
#define ZIP_BOMB_CREATOR_RINGBUFFER_H
#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <optional>
#include <ostream>
#include <span>

template<int length>
class RingBuffer {
public:
    void add(const std::span<uint8_t> data) {
        if (data.size() > length) {
            throw std::length_error("RingBuffer::add(): data size is too big!");
        }

        size_t firstChunk{std::min(data.size(), length - posEntry_)};

        std::copy_n(data.begin(), firstChunk, buffer_.begin() + posEntry_);

        if (const size_t secondChunk{data.size() - firstChunk}; secondChunk > 0) {
            std::copy_n(data.begin() + firstChunk, secondChunk, buffer_.begin());
        }

        posEntry_ = (posEntry_ + data.size()) % length;
        totalWritten_+=data.size();
    }

    uint8_t operator[](const size_t index) const {
        return buffer_[index % length];
    }

    bool isIndexValid(const size_t index) const {
        return index < totalWritten_ && index >= totalWritten_ - std::min(totalWritten_, static_cast<size_t>(length));
    }

    size_t getTotalWritten() const {
        return totalWritten_;
    }

private:
    std::array<std::uint8_t, length> buffer_;
    size_t posEntry_{};
    size_t totalWritten_{};
};


#endif //ZIP_BOMB_CREATOR_RINGBUFFER_H
