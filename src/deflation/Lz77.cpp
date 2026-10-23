//
// Created by LeeEeZian on 29/9/2026.
//

#include "deflation\Lz77.h"

#include <algorithm>

#include "deflation/BitWriter.h"

lz77::MatchCode lz77::getLengthCode(const int length) {
    constexpr std::array base{
        3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
        35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258
    };

    constexpr std::array extra{
        0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
    };

    size_t i{base.size() - 1};
    while (base[i] > length) --i;
    return {.symbol = 257 + static_cast<int>(i), .extraBit = extra[i], .extraValue = length - base[i]};
}

lz77::MatchCode lz77::getDistanceCode(const int distance) {
    constexpr std::array base{
        1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385,
        513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577
    };
    constexpr std::array extra{
        0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7,
        8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
    };

    size_t i{base.size() - 1};
    while (base[i] > distance) --i;

    return {.symbol = static_cast<int>(i), .extraBit = extra[i], .extraValue = distance - base[i]};
}

LocalFileHeader lz77::fixedCompress(const std::string_view fileName,std::span<uint8_t> byteBuffer) {
    constexpr size_t minimumRepeatSize{3};
    constexpr size_t maxWindowSize{static_cast<size_t>(helper::ByteSize::KB) * 32};

    BitWriter writer;

    writer.add(HuffmanTable::BFINAL::YES, 1);
    writer.add(HuffmanTable::BTYPE::FIXED_HUFFMAN, 2);

    size_t windowStart{};
    size_t windowEnd{};

    while (windowEnd + minimumRepeatSize < byteBuffer.size()) {
        const size_t windowSize{std::min(maxWindowSize, windowEnd - windowStart)};

        auto currentWindowStart{byteBuffer.begin() + static_cast<long long>(windowStart)};
        auto currentWindowEnd{currentWindowStart + static_cast<long long>(windowSize)};

        const auto recurringCheckStart{currentWindowEnd};
        const auto recurringCheckEnd{currentWindowEnd + minimumRepeatSize};

        auto recurringPoint{
            std::search(currentWindowStart,
                        currentWindowEnd,
                        recurringCheckStart,
                        recurringCheckEnd)
        };

        if (recurringPoint == currentWindowEnd) {
            writer.add(HuffmanTable::fixedSymbolTable(*recurringCheckStart));
            ++windowEnd;
            if (windowEnd - windowStart > maxWindowSize) windowStart += 1;

        } else {
            size_t remaining{byteBuffer.size() - windowEnd};
            const size_t maxLength{std::min<size_t>(258, remaining)};
            auto [ia,ib]{
                std::mismatch(recurringPoint, recurringPoint + static_cast<long long>(maxLength), recurringCheckStart)
            };

            const int length{static_cast<int>(ia - recurringPoint)};
            const int distance{static_cast<int>(recurringCheckStart - recurringPoint)};


            const MatchCode lengthCode{getLengthCode(length)};
            const MatchCode distanceCode{getDistanceCode(distance)};
            auto [lCode, lLength]{HuffmanTable::fixedSymbolTable(lengthCode.symbol)};
            auto [dCode, dLength]{HuffmanTable::fixedDistanceTable(distanceCode.symbol)};

            writer.add(lCode, lLength);
            writer.add(lengthCode.extraValue, lengthCode.extraBit);

            writer.add(dCode, dLength);
            writer.add(distanceCode.extraValue, distanceCode.extraBit);

            windowEnd += length;
            if (windowEnd - windowStart > maxWindowSize) windowStart = windowEnd - maxWindowSize;
        }
    }
    if (windowEnd < byteBuffer.size()) {
        const size_t remaining{byteBuffer.size() - windowEnd};
        for (size_t i{}; i < remaining; ++i) {
            auto [code, length]{HuffmanTable::fixedSymbolTable(byteBuffer[windowEnd + i])};
            writer.add(code, length);
        }
    }

    auto [endCode, endLength]{HuffmanTable::fixedSymbolTable(HuffmanTable::END_OF_BLOCK)};
    writer.add(endCode, endLength);

    std::vector<uint8_t> compressedBytes{writer.release()};
    const size_t compressedSize {compressedBytes.size()};

    const size_t uncompressedSize {byteBuffer.size()};


    Crc32 crc{};
    for (const uint8_t &byte: byteBuffer) {
        crc.update(byte);
    }
    crc.computeCrc32();

    return LocalFileHeader::createBasic(std::string(fileName),std::move(compressedBytes),compressedSize,uncompressedSize,crc.getCrc32());
}
