//
// Created by User on 9/24/2026.
//
#include "deflation/BitWriter.h"
#include "deflation/huffmanTable.h"
#include "deflation/Lz77.h"
#include "zip_file_construction/LocalFileHeader.h"

namespace {
    uint16_t reverseBits(uint16_t value, const int bitCount) {
        uint16_t result{};

        for (int i{}; i < bitCount; ++i) {
            result <<= 1;
            result |= (value & 1);
            value >>= 1;
        }
        return result;
    }
}

HuffmanCode HuffmanTable::fixedSymbolTable(const int symbol) {
    if (symbol <= 143) {
        return {.code = reverseBits(0x30 + symbol, 8), .length = 8};
    }

    if (symbol <= 255) {
        return {.code = reverseBits(0x190 + symbol - 144, 9), .length = 9};
    }

    if (symbol <= 279) {
        return {.code = reverseBits(0x00 + symbol - 256, 7), .length = 7};
    }

    return {.code = reverseBits(0xC0 + symbol - 280, 8), .length = 8};
}

std::vector<uint8_t> HuffmanTable::encodeFixed(const std::span<uint8_t> data) {
    BitWriter writer;
    writer.add(BFINAL::YES, 1);
    writer.add(BTYPE::FIXED_HUFFMAN, 2);

    for (const auto &byte: data) {
        auto [code, length] {HuffmanTable::fixedSymbolTable(byte)};
        writer.add(code, length);
    }

    auto [endCode, endLength] { HuffmanTable::fixedSymbolTable(256)};
    writer.add(endCode, endLength);

    return writer.release();
}



HuffmanCode HuffmanTable::fixedDistanceTable(const int symbol) {
    return { .code = reverseBits(symbol, 5), .length = 5 };
}
