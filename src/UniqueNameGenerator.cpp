//
// Created by LeeEeZian on 16/11/2026.
//
#include "UniqueNameGenerator.h"
#include <format>
namespace {
    int getDecimalPlaces(size_t num)  {
        int count{};
        while (num > 0) {
            ++count;
            num /= 10;
        }
        return count;
    }
}

UniqueNameGenerator::UniqueNameGenerator(const size_t maxVariations)
    : maxVariations_{maxVariations} {
    for (size_t i {} ; i < getDecimalPlaces(maxVariations_/ALPHABETS.size());i++ ) {
        maxNumberIterations_ = (maxNumberIterations_* 10)+9;
    }
}

std::string UniqueNameGenerator::createName() {
    ++currentNumIteration_;
    if (currentNumIteration_ >= maxNumberIterations_) {
        alphaIteration_ += 1;
        currentNumIteration_ = 0;
    }
    if (alphaIteration_ >= ALPHABETS.size()) {
        throw std::runtime_error("Exceeded unique name variation");
    }
    std::string name{ALPHABETS[alphaIteration_] + std::format("{:0{}}",currentNumIteration_ , getDecimalPlaces(maxNumberIterations_))};
    return name;
}

std::string UniqueNameGenerator::getName() {
    if (alphaIteration_ >= ALPHABETS.size()) {
        throw std::runtime_error("Exceeded unique name variation");
    }
    std::string name{ALPHABETS[alphaIteration_] + std::format("{:0{}}",currentNumIteration_ , getDecimalPlaces(maxNumberIterations_))};
    return name;
}

size_t UniqueNameGenerator::getSize() const {
    return 1 + getDecimalPlaces(maxNumberIterations_);
};
