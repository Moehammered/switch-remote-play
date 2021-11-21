#include "GeneralProgramOptions.h"

namespace
{
    std::string const boolToYes(bool val)
    {
        return val == true ? "yes" : "no";
    }

    bool const boolFromYes(std::string const str)
    {
        return str == "yes";
    }
}

namespace general
{
    std::unordered_map<ProgramParameters, std::string> const general::ProgramParamsToStr(GeneralProgramConfig const config)
    {
        auto values = std::unordered_map<ProgramParameters, std::string>{};

        values[ProgramParameters::MinimiseOnConnect] = boolToYes(config.minimiseOnConnect);
        values[ProgramParameters::MuteOnConnect] = boolToYes(config.muteOnConnect);
        values[ProgramParameters::RestoreOnDisconnect] = boolToYes(config.restoreOnDisconnect);
        values[ProgramParameters::RestoreMuteOnDisconnect] = boolToYes(config.restoreMuteOnDisconnect);
        values[ProgramParameters::LogToFile] = boolToYes(config.logToFile);

        return values;
    }

    GeneralProgramConfig const general::ProgramParamsFromStr(std::unordered_map<ProgramParameters, std::string> const& map)
    {
        auto config = GeneralProgramConfig{};

        auto parseBool = [&](auto const param, auto& member, auto const defaultValue)
        {
            auto entry = map.find(param);
            if (entry != map.end())
                member = boolFromYes(entry->second);
            else
                member = defaultValue;
        };

        parseBool(ProgramParameters::MinimiseOnConnect, config.minimiseOnConnect, DefaultMinimiseOnConnect);
        parseBool(ProgramParameters::MuteOnConnect, config.muteOnConnect, DefaultMuteOnConnect);
        parseBool(ProgramParameters::RestoreOnDisconnect, config.restoreOnDisconnect, DefaultRestoreOnDisconnect);
        parseBool(ProgramParameters::RestoreMuteOnDisconnect, config.restoreMuteOnDisconnect, DefaultRestoreMuteOnDisconnect);
        parseBool(ProgramParameters::LogToFile, config.logToFile, DefaultLogToFile);

        return config;
    }
}
