//
// Created by LeeEeZian on 16/11/2026.
//

#ifndef ZIP_BOMB_CREATOR_NAMEGENERATOR_H
#define ZIP_BOMB_CREATOR_NAMEGENERATOR_H
#include <string>
#include <utility>

class UniqueNameGenerator {
    public:

    UniqueNameGenerator(std::string&& name, const size_t maxVariations):name_{std::move( name )}, maxVariations_{ maxVariations } {}

private:
    std::string name_;
    size_t maxVariations_{};
    size_t currentVariation_{};

    [[nodiscard]] int getVariationDecimalPlaces() const;
    std::string getName();
};
#endif //ZIP_BOMB_CREATOR_NAMEGENERATOR_H
