#ifndef __GENERICOPTIONS_H__
#define __GENERICOPTIONS_H__

#include "../../ffmpegHelpers/VsyncMode.h"
#include "../../ffmpegHelpers/HWAccel.h"
#include "../../ffmpegHelpers/Resolution.h"
#include "../../ffmpegHelpers/VideoCodec.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace codec
{
    int16_t constexpr minMonitorNumber = 0;
    int16_t constexpr maxMonitorNumber = 100;
    int16_t constexpr defaultMonitorNumber = 0;

    int16_t constexpr minDesiredFramerate = 1;
    int16_t constexpr maxDesiredFramerate = 60;
    int16_t constexpr defaultDesiredFramerate = 30;

    int16_t constexpr mbToKb(double const mb)
    {
        return mb * 1024;
    }

    static std::vector<int16_t> const bitrates
    {
        mbToKb(3), mbToKb(3.5), mbToKb(4), mbToKb(4.5),
        mbToKb(5), mbToKb(5.5), mbToKb(6), mbToKb(6.5),
        mbToKb(7), mbToKb(7.5), mbToKb(8), mbToKb(8.5),
        mbToKb(9), mbToKb(9.5), mbToKb(10), mbToKb(10.5),
        mbToKb(11), mbToKb(11.5), mbToKb(12), mbToKb(12.5),
        mbToKb(13), mbToKb(13.5), mbToKb(14), mbToKb(15)
    };

    struct VideoData
    {
        Resolution          desktopResolution;
        Resolution          switchResolution;
        int16_t             desiredFrameRate;
        int16_t             bitrateKB;
        ffmpeg::VsyncMode   vsyncMode;
        ffmpeg::HWAccelMode hwaccelMode;
        ffmpeg::VideoCodec  videoCodec;
        int16_t             monitorNumber;
    };

    uint32_t constexpr videoDataSize = sizeof(VideoData);

    enum class VideoParameters
    {
        DesktopResolution,
        SwitchResolution,
        DesiredFramerate,
        BitrateKB,
        VsyncMode,
        HWAccelMode,
        VideoCodec,
        MonitorNumber
    };

    std::unordered_map<VideoParameters, std::string> videoParamsToStr(VideoData const data);
    VideoData videoParamsFromStr(std::unordered_map<VideoParameters, std::string> const & map);

    static std::vector<VideoParameters> const videoParametersList
    {
        VideoParameters::DesktopResolution, 
        VideoParameters::SwitchResolution,
        VideoParameters::DesiredFramerate,
        VideoParameters::BitrateKB,
        VideoParameters::VsyncMode,
        VideoParameters::HWAccelMode,
        VideoParameters::VideoCodec,
        VideoParameters::MonitorNumber
    };

    static std::unordered_map<VideoParameters, std::string> const videoParamToDesc
    {
        { VideoParameters::DesktopResolution, "Desktop Resolution" },
        { VideoParameters::SwitchResolution, "Switch Resolution" },
        { VideoParameters::DesiredFramerate, "Desired Framerate" },
        { VideoParameters::BitrateKB, "Bitrate(KB/s)" },
        { VideoParameters::VsyncMode, "Vsync Mode" },
        { VideoParameters::HWAccelMode, "HW Accel Mode" },
        { VideoParameters::VideoCodec, "Codec" },
        { VideoParameters::MonitorNumber, "Monitor Number" }
    };
}

#endif
