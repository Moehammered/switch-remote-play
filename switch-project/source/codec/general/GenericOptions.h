#ifndef __GENERICOPTIONS_H__
#define __GENERICOPTIONS_H__

#include "../../dataHelpers/VsyncMode.h"
#include "../../dataHelpers/HWAccel.h"
#include "../../dataHelpers/Resolution.h"
#include "../../dataHelpers/VideoCodec.h"
#include <vector>
#include <unordered_map>
#include <string>

int16_t constexpr MinMonitorNumber = 0;
int16_t constexpr MaxMonitorNumber = 100;
int16_t constexpr DefaultMonitorNumber = 0;

int16_t constexpr MinDesiredFramerate = 1;
int16_t constexpr MaxDesiredFramerate = 60;
int16_t constexpr DefaultDesiredFramerate = 30;

int16_t constexpr mbToKb(double const mb)
{
    return mb * 1024;
}
static std::vector<int16_t> const Bitrates
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
    Resolution      desktopResolution;
    Resolution      switchResolution;
    int16_t         desiredFrameRate;
    int16_t         bitrateKB;
    VsyncMode       vsyncMode;
    HWAccelMode     hwaccelMode;
    VideoCodec      videoCodec;
    int16_t         monitorNumber;
};

uint32_t constexpr VideoDataSize = sizeof(VideoData);

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

std::unordered_map<VideoParameters, std::string> VideoParamsToStr(VideoData const data);
VideoData VideoParamsFromStr(std::unordered_map<VideoParameters, std::string> const & map);

static std::vector<VideoParameters> const VideoParametersList
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

static std::unordered_map<VideoParameters, std::string> const VideoParamToDesc
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

#endif
