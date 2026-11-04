#include <chrono>
#include <fstream>

#include "../include/zip_file_construction/LocalFileHeader.h"
#include "Helper.h"
#include "Timer.h"
#include "deflation/Lz77.h"
#include "zip_file_construction/ZipFile.h"

#include <iostream>
#include <filesystem>

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
        std::string zipFileName{"Test.zip"};
        Timer totalRuntime{"Total Runtime"};

        namespace fs = std::filesystem;

        std::string folderName{"test"};
        ZipFile zipFile{zipFileName};

        for (const fs::directory_entry& entry : fs::recursive_directory_iterator(folderName,fs::directory_options::skip_permission_denied)) {
            if (is_regular_file(entry)) {
                std::string fileName{entry.path().filename().string()};
                Timer readFileTimer{"Compressing File - "+fileName};

                std::vector<uint8_t> fileData{readFile(fileName)};
                LocalFileHeader compressedFileHeader { lz77::fixedCompress(fileName,fileData)};
                zipFile.addFile(compressedFileHeader);
            }

        }


        // LocalFileHeader bombLfh{};
        // {
        //     Timer localHeaderBomb{"Creating local file header bomb"};
        //     bombLfh = LocalFileHeader::createBomb("Your_Mum.ppt", (static_cast<size_t>(ByteSizeBaseKB::GB) * 11));
        // }
        //
        // zipFile.addFile(std::move(bombLfh));

        {
            Timer zipFileSerialization{"Closing zip file"};
            zipFile.close();
        }


    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << std::endl;
    }
}
;