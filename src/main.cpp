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

#include "deflation/Lz77.h"

namespace {
    enum class ByteSizeBaseKB:size_t {
        MB = 1024,
        GB = MB * 1024,
        TB = GB * 1024,
        PB = TB * 1024
    };
}

std::vector<uint8_t> readFile(const std::string &fileName) {
    std::ifstream in(fileName, std::ios::binary | std::ios::ate);
    if (!in) {
        std::cout << "Input file failed to open \n";
        return {};
    }

    const long long fileSize { in.tellg()};

    std::vector<uint8_t> fileDate(fileSize);
    in.seekg(0, std::ios::beg);

    in.read(reinterpret_cast<char *>(fileDate.data()), fileSize);

    return fileDate;
}

int main() {
    try {
        const std::string readFileName("test.txt");
        std::vector<uint8_t> readFileData{readFile(readFileName)};
        LocalFileHeader readFileHeader { lz77::fixedCompress(readFileName,readFileData)};


        Timer totalRuntime{"Total Runtime"};
        std::string zipFileName{"Test.zip"};

        std::ofstream out(zipFileName, std::ios::binary);
        if (!out) {
            std::cout << "Output file failed to open \n";
            return 1;
        }
        // LocalFileHeader bombLfh{};
        // {
        //     Timer localHeaderBomb{"Creating local file header bomb"};
        //     bombLfh = LocalFileHeader::createBomb("Your_Mum.ppt", (static_cast<size_t>(ByteSizeBaseKB::GB) * 11));
        // }
        //
        ZipFile zipFile{zipFileName};
        // zipFile.addFile(std::move(bombLfh));
        zipFile.addFile(std::move(readFileHeader));

        {
            Timer zipFileSerialization{"Serializing zip file"};
            zipFile.serializeTo(out);
        }


    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}
;