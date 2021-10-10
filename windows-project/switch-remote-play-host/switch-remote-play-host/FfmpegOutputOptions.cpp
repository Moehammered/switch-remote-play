#include "FfmpegOutputOptions.h"

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

namespace ffmpeg
{
    std::unordered_map<OutputParameters, std::string> const OutputParamsToStr(FfmpegOutputConfig const config)
    {
        auto values = std::unordered_map<OutputParameters, std::string>{};

        values[OutputParameters::ShowEncoderOutput] = boolToYes(config.showEncoderOutput);
        values[OutputParameters::ShowAudioOutputWindow] = boolToYes(config.showAudioOutputWindow);

        return values;
    }

    FfmpegOutputConfig const OutputParamsFromStr(std::unordered_map<OutputParameters, std::string> const& map)
    {
        auto config = FfmpegOutputConfig{};

        auto parseBool = [&](auto const param, auto& member, auto const defaultValue)
        {
            auto entry = map.find(param);
            if (entry != map.end())
                member = boolFromYes(entry->second);
            else
                member = defaultValue;
        };

        parseBool(OutputParameters::ShowEncoderOutput, config.showEncoderOutput, DefaultShowEncoderOutput);
        parseBool(OutputParameters::ShowAudioOutputWindow, config.showAudioOutputWindow, DefaultShowAudioOutputWindow);

        return config;
    }
}