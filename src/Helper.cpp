//
// Created by LeeEeZian on 9/9/2026.
//
#include "Helper.h"

void helper::appendBytes16Small(std::vector<uint8_t> &buffer,const uint16_t data) {
    buffer.push_back(data & 0xff);

    buffer.push_back(data >> 8);
}

void helper::appendBytes32Small(std::vector<uint8_t> &buffer,const uint32_t data) {
    buffer.push_back(data & 0xff);
    buffer.push_back(data >> 8);
    buffer.push_back(data >> 16);
    buffer.push_back(data >> 24);

}
