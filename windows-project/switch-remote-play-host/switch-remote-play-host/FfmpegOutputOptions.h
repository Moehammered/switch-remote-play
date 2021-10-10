#ifndef __FFMPEGOUTPUTOPTIONS_H__
#define __FFMPEGOUTPUTOPTIONS_H__

#include <unordered_map>
#include <string>

namespace ffmpeg
{
    enum class OutputParameters
    {
        ShowEncoderOutput,
        ShowAudioOutputWindow
    };

    static std::unordered_map<OutputParameters, std::string> const OutputParamsDesc
    {
        { OutputParameters::ShowEncoderOutput, "Show Encoder Output" },
        { OutputParameters::ShowAudioOutputWindow, "Show Audio Output Window" }
    };

    auto constexpr DefaultShowEncoderOutput = false;
    auto constexpr DefaultShowAudioOutputWindow = false;

    struct FfmpegOutputConfig
    {
        bool showEncoderOutput;
        bool showAudioOutputWindow;
    };

    auto constexpr FfmpegOutputConfigSize = sizeof(FfmpegOutputConfig);

    std::unordered_map<OutputParameters, std::string> const OutputParamsToStr(FfmpegOutputConfig const config);
    FfmpegOutputConfig const OutputParamsFromStr(std::unordered_map<OutputParameters, std::string> const& map);
}

#endif
