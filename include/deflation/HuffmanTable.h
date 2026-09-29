//
// Created by User on 9/24/2026.
//

#ifndef ZIP_BOMB_CREATOR_HUFFMANTABLE_H
#define ZIP_BOMB_CREATOR_HUFFMANTABLE_H
#include <cstdint>
#include <span>
#include <vector>

struct DeflateResult {
    std::vector<uint8_t> data;
    uint64_t uncompressedSize;
    uint32_t crc32;
};

struct HuffmanCode {
    uint16_t code;
    uint8_t length;
};

class HuffmanTable {
public:
    enum BFINAL {
        NO = 0b0,
        YES = 0b1,
    };
    enum BTYPE {
        STORED = 0b00,
        FIXED_HUFFMAN = 0b01,
        DYNAMIC_HUFFMAN = 0b10,
        ERROR = 0b11
    };


    static HuffmanCode fixedSymbolTable(int symbol);

    static std::vector<uint8_t> encodeFixed(std::span<uint8_t> data);

    static DeflateResult createEncodedFixedBomb(size_t size);

    static HuffmanCode fixedDistanceTable(int symbol);

private:
};
#endif //ZIP_BOMB_CREATOR_HUFFMANTABLE_H
