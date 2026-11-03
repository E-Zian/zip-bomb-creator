//
// Created by LeeEeZian on 10/9/2026.
//

#include "../../include/zip_file_construction/ZipFile.h"

#include "Timer.h"

void ZipFile::addFile(LocalFileHeader &fileHeader) {
    const std::string fileName{fileHeader.getFileName().begin(), fileHeader.getFileName().end()};
    Timer singleFileWriteTimer{"Writing " + fileName + " to zip file"};

    centralDirectories_.push_back(CentralDirectoryHeader::createBasic(fileHeader));
    centralDirectories_.back().setLocalHeaderOffset(fileOffset_);
    fileOffset_ += fileHeader.serializeTo(outStream_);
}

void ZipFile::serialize() {
    try {
        if (centralDirectories_.empty()) return;

        Timer zipFileWriteTimer{"Writing to zipFile"};

        Eocd eocd{{.entries = centralDirectories_, .comment = {}}};

        const uint64_t centralDirStart{fileOffset_};

        eocd.setCentralDirOffset(fileOffset_);

        for (auto& centralDirectory : centralDirectories_) {
            fileOffset_ += centralDirectory.serializeTo(outStream_);
        }
        eocd.setCentralDirSize(fileOffset_-centralDirStart);

        eocd.setOffsetOfEocdRecord(fileOffset_);
        fileOffset_ += eocd.serializeTo(outStream_);
    } catch (std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}
