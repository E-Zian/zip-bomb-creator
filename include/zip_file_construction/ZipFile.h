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

#include <utility>
#include <vector>

class ZipFile {
    public:
    explicit ZipFile(std::string  name):filename_{std::move(name)}{};
    std::string filename_;
    void addFile(LocalFileHeader&& fileHeader);

    bool serializeTo(std::ofstream& out) ;

    private:
    std::vector<LocalFileHeader> localFileHeaders_;
    std::vector<CentralDirectoryHeader> centralDirectories_;

};
#endif //ZIP_BOMB_CREATOR_ZIPFILE_H
