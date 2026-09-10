//
// Created by LeeEeZian on 9/9/2026.
//
#include "Helper.h"

#include <iomanip>
#include <iostream>

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

void helper::displayBytes(const std::span<const uint8_t> buffer) {
   for (size_t i{};i<buffer.size();i++) {
       std::cout << std::hex << std::setw(2) << std::setfill('0')<<static_cast<int>(buffer[i]) << " ";
       if ((i + 1) % 16 == 0) std::cout << '\n';
   }
}