//
// Created by LeeEeZian on 10/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_ZIPFILE_H
#define ZIP_BOMB_CREATOR_ZIPFILE_H

#include <optional>

#include "CentralDirectory.h"
#include "LocalFileHeader.h"
#include "Eocd.h"
#include "Helper.h"
#include <iostream>
#include <utility>
#include <vector>

class ZipFile {
    public:
    explicit ZipFile(std::string  name):filename_{std::move(name)},outStream_{filename_,std::ios::binary} {
        if (!outStream_) {
            std::cout << "Output file failed to open \n";
            throw std::runtime_error("Output file failed to open");
        }
    };
    void addFile(LocalFileHeader& fileHeader);

    void close() ;


    private:
    std::string filename_;

    std::vector<LocalFileHeader> localFileHeaders_;
    std::vector<CentralDirectoryHeader> centralDirectories_;

    uint64_t fileOffset_{};
    size_t localFileHeadersCount_{};
    std::ofstream outStream_;

};
#endif //ZIP_BOMB_CREATOR_ZIPFILE_H
