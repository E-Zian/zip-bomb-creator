//
// Created by LeeEeZian on 10/9/2026.
//

#ifndef ZIP_BOMB_CREATOR_ZIPFILE_H
#define ZIP_BOMB_CREATOR_ZIPFILE_H

#include <optional>

#include "CentralDirectory.h"
#include "LocalFileHeader.h"
#include "Eocd.h"

#include <vector>

class ZipFile {
    public:
    void addFile(LocalFileHeader&& fileHeader);

    [[nodiscard]] std::optional<std::vector<uint8_t>>  serialize() ;

    private:
    std::vector<LocalFileHeader> localFileHeaders_;
    std::vector<CentralDirectoryHeader> centralDirectories_;

};
#endif //ZIP_BOMB_CREATOR_ZIPFILE_H
