#ifndef ZIP_BOMB_CREATOR_INPUTPARSER_H
#define ZIP_BOMB_CREATOR_INPUTPARSER_H
#include <unordered_map>
#include <string>
#include <optional>


class InputParser {
    struct FlagDescription {
        std::string flagDescription{};
        bool hasFlagValue{};
    };

public:
    bool parse(int argc, const char** argv);
    void registerFlag(const std::string& flagName, const std::string &flagDescription, bool hasFlagValue = false);
    void displayHelp() const;

    [[nodiscard]] std::optional<std::string> getFlagValue(const std::string& flagName) const;

    [[nodiscard]] bool containsInputFlag(const std::string& flagName) const;

private:
    std::unordered_map<std::string, FlagDescription> registeredFlags_;
    std::unordered_map<std::string, std::string> parsedFlagValues_;

    [[nodiscard]] bool checkFlagExists(const std::string& flagName) const;
    [[nodiscard]] bool checkFlagRequiresValue(const std::string& flagName) const;

    void setFlagValue(const std::string& flagName, const std::string& flagValue);

};


#endif //ZIP_BOMB_CREATOR_INPUTPARSER_H
