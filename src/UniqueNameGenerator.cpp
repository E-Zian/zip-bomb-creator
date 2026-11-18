//
// Created by LeeEeZian on 16/11/2026.
//
#include "UniqueNameGenerator.h"
#include <format>
#include <iostream>

namespace {
    int getDecimalPlaces(size_t num) {
        int count{};
        while (num > 0) {
            ++count;
            num /= 10;
        }
        return count;
    }
}

UniqueNameGenerator::UniqueNameGenerator(const size_t maxVariations, std::string &&folderName)
    : maxVariations_{maxVariations},
      folderName_{std::move(folderName)} {
    const size_t decimalsPerAlpha{ std::max<size_t>(1, maxVariations_ / ALPHABETS.size())};

    for (size_t i{}; i < getDecimalPlaces(decimalsPerAlpha); ++i) {
        maxNumberIterations_ = (maxNumberIterations_ * 10) + 9;
    }
    if (!folderName_.empty() && folderName_.back() != '/') {
        folderName_ += '/';
    }
}

std::string UniqueNameGenerator::createName() {
    ++currentNumIteration_;
    if (currentNumIteration_ >= maxNumberIterations_) {
        alphaIteration_ += 1;
        currentNumIteration_ = 0;
    }
    return getName();
}

std::string UniqueNameGenerator::getName() {
    if (alphaIteration_ >= ALPHABETS.size()) {

        throw std::runtime_error("Exceeded unique name variation");
    }
    std::string name{
        folderName_ + ALPHABETS[alphaIteration_] + std::format("{:0{}}", currentNumIteration_,
                                                               getDecimalPlaces(maxNumberIterations_))
    };
    return name;
}

size_t UniqueNameGenerator::getSize() const {
    return 1 + getDecimalPlaces(maxNumberIterations_);
};
