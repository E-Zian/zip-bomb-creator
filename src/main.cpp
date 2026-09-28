#include <fstream>

#include "../include/zip_file_construction/LocalFileHeader.h"
#include "Helper.h"
#include "../include/zip_file_construction/CentralDirectory.h"
#include "zip_file_construction/ZipFile.h"
#include "deflation/BitWriter.h"
#include <iostream>

namespace {
    bool writeFile(const std::string& fileName, const std::span<uint8_t> data) {
        std::ofstream out(fileName, std::ios::binary);
        out.write(reinterpret_cast<const char*>(data.data()), static_cast<long>(data.size()));

        return out.good();
    }
}

int main() {
    BitWriter bitWriterTest;
    bitWriterTest.add(0b11,  2);
    bitWriterTest.add(0b101, 3);
    helper::displayBytes(bitWriterTest.release());

    // LocalFileHeader lfh { LocalFileHeader::createStored("a.txt","Hello")};
    //
    // ZipFile zipFile;
    // zipFile.addFile(std::move(lfh));
    // auto file {zipFile.serialize()};
    // if (file) {
    //     if (writeFile("test.zip",*file)) {
    //         std::cout << "zip file written\n";
    //     }else {
    //         std::cout << "zip file failed write\n";
    //     }
    // }

}