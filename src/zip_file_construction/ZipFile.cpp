//
// Created by LeeEeZian on 10/9/2026.
//

#include "../../include/zip_file_construction/ZipFile.h"

#include "Timer.h"

void ZipFile::addFile(LocalFileHeader &&fileHeader) {
    centralDirectories_.push_back(CentralDirectoryHeader::createBasic(fileHeader));
    localFileHeaders_.push_back(std::move(fileHeader));
}

bool ZipFile::serializeTo(std::ofstream &out) {
    try {
        if (centralDirectories_.empty() || localFileHeaders_.empty() || centralDirectories_.size() != localFileHeaders_.
        size()) { return false; }

        Timer zipFileWriteTimer{"Writing to zipFile"};
        size_t fileOffset{};

        Eocd eocd{{.entries = centralDirectories_, .comment = {}}};

        uint32_t eocdCentralDirSize{};

        for (size_t i{}; i < localFileHeaders_.size(); ++i) {
            centralDirectories_[i].setLocalHeaderOffset(fileOffset);
            fileOffset += localFileHeaders_[i].serializeTo(out);

            eocdCentralDirSize += centralDirectories_[i].size();
        }

        eocd.setCentralDirOffset(fileOffset);

        eocd.setCentralDirSize(eocdCentralDirSize);

        for (size_t i{}; i < centralDirectories_.size(); ++i) {
            fileOffset += centralDirectories_[i].serializeTo(out);
        }

        eocd.setOffsetOfEocdRecord(fileOffset);
        fileOffset += eocd.serializeTo(out);

        return true;
    }catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return false;
    }

}
