//
// Created by LeeEeZian on 10/9/2026.
//

#include "../../include/zip_file_construction/ZipFile.h"

#include "Timer.h"
#include <filesystem>

ZipFile::~ZipFile() {
    if (!completed_) {
        outStream_.close();
        std::error_code ec;
        std::filesystem::remove(filename_, ec);
    }
}

void ZipFile::addFile(LocalFileHeader &fileHeader) {
    std::vector<uint8_t> fileNameVector{fileHeader.getFileName()};
    const std::string fileName{fileNameVector.begin(), fileNameVector.end()};

    Timer singleFileWriteTimer{"Writing " + fileName + " to zip file", true};

    centralDirectories_.push_back(CentralDirectoryHeader::createBasic(fileHeader));
    centralDirectories_.back().setLocalHeaderOffset(fileOffset_);
    fileOffset_ += fileHeader.serializeTo(outStream_);
}

void ZipFile::close() {
    if (centralDirectories_.empty()) return;

    Timer zipFileWriteTimer{"Closing zipFile"};

    Eocd eocd{{.entries = centralDirectories_, .comment = {}}};

    const uint64_t centralDirStart{fileOffset_};

    eocd.setCentralDirOffset(fileOffset_);

    for (auto &centralDirectory: centralDirectories_) {
        fileOffset_ += centralDirectory.serializeTo(outStream_);
    }
    eocd.setCentralDirSize(fileOffset_ - centralDirStart);

    eocd.setOffsetOfEocdRecord(fileOffset_);
    fileOffset_ += eocd.serializeTo(outStream_);

    completed_ = true;
}

void ZipFile::cdAddCapacity(const size_t capacity) {
    centralDirectories_.reserve(centralDirectories_.size() + capacity);
}
