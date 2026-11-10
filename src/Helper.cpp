//
// Created by LeeEeZian on 9/9/2026.
//
#include "Helper.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

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

void helper::appendBytes64Small(std::vector<uint8_t> &buffer, const uint64_t data) {
    buffer.push_back(data & 0xff);
    buffer.push_back(data >> 8);
    buffer.push_back(data >> 16);
    buffer.push_back(data >> 24);
    buffer.push_back(data >> 32);
    buffer.push_back(data >> 40);
    buffer.push_back(data >> 48);
    buffer.push_back(data >> 56);
}


void helper::displayBytes(const std::span<const uint8_t> buffer) {
   for (size_t i{};i<buffer.size();i++) {
       std::cout << std::hex << std::setw(2) << std::setfill('0')<<static_cast<int>(buffer[i]) << " ";
       if ((i + 1) % 16 == 0) std::cout << '\n';
   }
}

bool helper::writeFile(std::ofstream& out, const std::span<uint8_t> data) {

    out.write(reinterpret_cast<const char *>(data.data()), static_cast<std::streamsize>(data.size()));

    return out.good();
}

void helper::writeXChunk(std::ofstream &out, const std::span<uint8_t> chunkData, const size_t totalBytes) {
    const size_t chunkSize {chunkData.size()};

    size_t bytesLeft{totalBytes};


    while (bytesLeft > 0) {
        const size_t bytesToWrite{std::min(chunkSize,bytesLeft)};
        out.write(reinterpret_cast<const char *>(chunkData.data()), static_cast<std::streamsize>(bytesToWrite));
        bytesLeft -= bytesToWrite;
    }
}


size_t helper::randomNumber(const size_t max) {
    if (max == 0) return 0;
    std::random_device rd;
    std::mt19937_64 gen(rd());

    return gen()%max;
}

std::vector<uint8_t> helper::readFile(const std::string &filePath) {
    std::ifstream in(filePath, std::ios::binary | std::ios::ate);
    if (!in) {
        std::cout << "failed to open Input file : "<<filePath<<" \n";
        return {};
    }

    const long long fileSize { in.tellg()};

    std::vector<uint8_t> fileDate(fileSize);
    in.seekg(0, std::ios::beg);

    in.read(reinterpret_cast<char *>(fileDate.data()), fileSize);

    return fileDate;
}

std::string helper::ensureZipExtension(std::string fileName) {
    if (!fileName.ends_with(".zip"))
        fileName += ".zip";
    return fileName;
}

