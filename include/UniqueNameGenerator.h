//
// Created by LeeEeZian on 16/11/2026.
//

#ifndef ZIP_BOMB_CREATOR_NAMEGENERATOR_H
#define ZIP_BOMB_CREATOR_NAMEGENERATOR_H
#include <cstdint>
#include <string>

class UniqueNameGenerator {
    static constexpr std::string_view ALPHABETS{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

public:
    UniqueNameGenerator(size_t maxVariations,std::string&& folderName);

    std::string createName();

    std::string getName();

    [[nodiscard]] size_t getSize() const;

    std::string getFolderName();

private:
    size_t maxVariations_{};
    size_t maxNumberIterations_{};
    size_t currentNumIteration_{};
    size_t alphaIteration_{};

    std::string folderName_{};
};
#endif //ZIP_BOMB_CREATOR_NAMEGENERATOR_H
