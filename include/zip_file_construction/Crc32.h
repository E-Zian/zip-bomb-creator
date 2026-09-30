//
// Created by LeeEeZian on 30/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_CRC32_H
#define ZIP_BOMB_CREATOR_CRC32_H

#include <cstdint>
struct Crc32 {
    uint32_t state{0xFFFFFFFF};
    const uint32_t polynomial{0xEDB88320};

    uint32_t finalValue{};

    void update(uint8_t byte);

    void setCrc32(const uint32_t byte) {
        finalValue = byte;
    };

    void finalize();

    [[nodiscard]] uint32_t getCrc32() const ;


};
#endif //ZIP_BOMB_CREATOR_CRC32_H
