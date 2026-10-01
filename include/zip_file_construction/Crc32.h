//
// Created by LeeEeZian on 30/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_CRC32_H
#define ZIP_BOMB_CREATOR_CRC32_H

#include <cstdint>
struct Crc32 {
    uint32_t state{0xFFFFFFFF};
    const uint32_t polynomial{0xEDB88320};

    uint32_t crc32Value{};

    size_t length{};

    void update(uint8_t byte);

    void advance(size_t bits);
    [[nodiscard]] uint32_t getCrc32() const;

    void computeCrc32();


    Crc32& combine(const Crc32 &b);
    [[nodiscard]] uint32_t advanceValue(uint32_t crc,  size_t bits) const ;
};
#endif //ZIP_BOMB_CREATOR_CRC32_H
