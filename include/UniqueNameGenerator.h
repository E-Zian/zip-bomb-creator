//
// Created by LeeEeZian on 16/11/2026.
//

#ifndef ZIP_BOMB_CREATOR_NAMEGENERATOR_H
#define ZIP_BOMB_CREATOR_NAMEGENERATOR_H
#include <cstdint>
#include <string>

class UniqueNameGenerator {
    static constexpr std::string_view ALPHABETS {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

    public:
    explicit UniqueNameGenerator(size_t maxVariations);
    std::string getName();
    [[nodiscard]] size_t getSize() const;

private:
    size_t maxVariations_{};
    size_t maxNumberIterations_{};
    size_t currentNumIteration_{};
    uint8_t alphaIteration_{};

};
#endif //ZIP_BOMB_CREATOR_NAMEGENERATOR_H
