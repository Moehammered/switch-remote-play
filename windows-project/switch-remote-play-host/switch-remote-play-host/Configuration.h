#pragma once

#include <string>
#include <vector>

class Configuration
{
public:
    Configuration(std::string const file);

    bool SaveConfigFile();
    bool ExtractVariable(std::string const variable, std::string& result) const;
    bool ReplaceVariable(std::string const variable, std::string const value, std::string& result);
    void CreateVariable(std::string const variable, std::string const value);

private:
    std::string const filepath;
    std::vector<std::string> lines;
    char const delimiter;
    char const endOfLine;

    std::vector<std::string> LoadFile(std::string const filepath) const;
};

