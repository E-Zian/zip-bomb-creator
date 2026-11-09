//
// Created by User on 11/9/2026.
//
#include "tui/InputParser.h"
#include <iostream>
#include <format>

void InputParser::parse(const int argc,const char **argv) {

    for (int i = 1; i < argc; i++) {
        if (std::string_view argcView{argv[i]}; argcView.starts_with('-') && checkFlagExists(std::string(argcView.substr(1)))) {
            if (std::string flagName{argcView.substr(1)}; checkFlagRequiresValue(flagName)) {
                if (i+1 < argc) {
                    setFlagValue(flagName, argv[i+1]);
                    ++i;
                }else {
                    std::cout <<  "Required value for "<< flagName <<" not found" << '\n'<< '\n';
                    return;
                }
            }else {
                setFlagValue(flagName, "true");
            }
        }else {
            std::cout <<  "Invalid flag given : " << argv[i] << '\n'<< '\n';
            return;
        }
    }

    state_ = true;
}

void InputParser::registerFlag(const std::string& flagName, const std::string &flagDescription, const bool hasFlagValue) {
    registeredFlags_.emplace(flagName, FlagDescription{flagDescription,hasFlagValue});
}

std::optional<std::string> InputParser::getFlagValue(const std::string &flagName) const {
    if (parsedFlagValues_.contains(flagName)) {
        return parsedFlagValues_.at(flagName);
    }
    return std::nullopt;
}

bool InputParser::containsInputFlag(const std::string &flagName) const {
    return parsedFlagValues_.contains(flagName);
}

bool InputParser::checkFlagExists(const std::string& flagName) const {
    return registeredFlags_.contains(flagName);
}

bool InputParser::checkFlagRequiresValue(const std::string &flagName) const {
    return registeredFlags_.at(flagName).hasFlagValue;
}

void InputParser::setFlagValue(const std::string &flagName, const std::string &flagValue) {
    parsedFlagValues_.insert_or_assign(flagName, flagValue);
}

void InputParser::displayHelp() const {
    for (const auto& [flagName, flagDescription] : registeredFlags_) {
        std::string label { std::format("-{}", flagName)};
        std::cout << std::format("  {:<12}{:<50}{}\n",
            label,
            flagDescription.flagDescription,
            flagDescription.hasFlagValue ? "[requires value]" : "");
    }
}
