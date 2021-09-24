#ifndef __MAPPEDDATACONFIGURATION_H__
#define __MAPPEDDATACONFIGURATION_H__

#include <string>
#include <vector>
#include <unordered_map>
#include "Configuration.h"
#include "StringUtils.h"

template<typename configType, typename enumType>
class MappedDataConfiguration
{
public:
    MappedDataConfiguration(std::string const filepath, std::unordered_map<enumType, std::string> const & memberDescriptions)
        : configFile{filepath}, descriptionMap{memberDescriptions}
    {
    }

    template<typename membersToStr>
    bool Save(configType const data, membersToStr toStrings)
    {
        auto parsedMembers = toStrings(data);
        auto fileVariables = ToFileVariables(parsedMembers);
        return SaveFileVariables(fileVariables, configFile);
    }

    template<typename membersParseFunc>
    configType Data(membersParseFunc parseStrMembers) const
    {
        auto variablesMap = ParseFileVariables(configFile);
        return parseStrMembers(variablesMap);
    }

private:
    Configuration configFile;
    std::unordered_map<enumType, std::string> const& descriptionMap;

    bool SaveFileVariables(std::vector<std::pair<std::string, std::string>> const & variables, Configuration& config)
    {
        auto buffer = std::string{};
        for(auto& var : variables)
        {
            if(!config.ReplaceVariable(var.first, var.second, buffer))
                config.CreateVariable(var.first, var.second);
        }

        return config.SaveConfigFile();
    }

    std::vector<std::pair<std::string, std::string>> ToFileVariables(std::unordered_map<enumType, std::string> const & members) const
    {
        auto fileLines = std::vector<std::pair<std::string, std::string>>{};

        for (auto& line : members)
        {
            auto paramItr = descriptionMap.find(line.first);
            if (paramItr != descriptionMap.end())
            {
                auto paramStr = paramItr->second;
                Trim(paramStr);

                auto paramVal = line.second;
                Trim(paramVal);

                fileLines.emplace_back(paramStr, paramVal);
            }
        }

        return fileLines;
    }

    std::unordered_map<enumType, std::string> const ParseFileVariables(Configuration const& config) const
    {
        auto entryMap = std::unordered_map<enumType, std::string>{};

        for(auto const& param : descriptionMap)
        {
            auto varVal = std::string{};
            if(config.ExtractVariable(param.second, varVal))
                entryMap[param.first] = varVal;
        }

        return entryMap;
    }
};

#endif