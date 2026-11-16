//
// Created by LeeEeZian on 16/11/2026.
//
#include "UniqueNameGenerator.h"
#include <format>
int UniqueNameGenerator::getVariationDecimalPlaces() const {
    size_t tempMax{maxVariations_};
    int count{};

    while (tempMax > 0) {
        ++count;
        tempMax >>= 1;
    }

    return count;
}

std::string UniqueNameGenerator::getName() {
    ++currentVariation_;
    if (currentVariation_ > maxVariations_) {
        throw std::runtime_error("NameGenerator::getName(): Maximum number of variables reached.");
    }
    std::string name{name_+std::format("{:0{}}",currentVariation_,getVariationDecimalPlaces())};
    return name;
};
