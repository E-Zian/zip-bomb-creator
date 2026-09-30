#include <fstream>

#include "../include/zip_file_construction/LocalFileHeader.h"
#include "Helper.h"
#include "../include/zip_file_construction/CentralDirectory.h"
#include "zip_file_construction/ZipFile.h"
#include "deflation/BitWriter.h"
#include "deflation/HuffmanTable.h"
#include <iostream>

namespace {
    bool writeFile(const std::string& fileName, const std::span<uint8_t> data) {
        std::ofstream out(fileName, std::ios::binary);
        out.write(reinterpret_cast<const char*>(data.data()), static_cast<long>(data.size()));

        return out.good();
    }
}

namespace {
    enum class ByteSize:size_t {
        MB = 1024,
        GB = 1024*1024,
        TB = 1024*1024*1024,
    };
}

int main() {

    LocalFileHeader lfh { LocalFileHeader::createBomb("a.txt",static_cast<size_t>(ByteSize::MB)*1000) };

    ZipFile zipFile;
    zipFile.addFile(std::move(lfh));
    auto file {zipFile.serialize()};
    if (file) {
        if (writeFile("test.zip",*file)) {
            std::cout << "zip file written\n";
        }else {
            std::cout << "zip file failed write\n";
        }
    }

}