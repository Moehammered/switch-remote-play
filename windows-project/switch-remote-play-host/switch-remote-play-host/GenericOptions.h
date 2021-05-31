#ifndef __GENERICOPTIONS_H__
#define __GENERICOPTIONS_H__

#include "VsyncMode.h"
#include "HWAccel.h"
#include "Resolution.h"
#include "VideoCodec.h"
#include <vector>
#include <unordered_map>
#include <string>

static std::vector<int16_t> const Framerates { 30, 60 };

struct VideoData
{
    Resolution      desktopResolution;
    Resolution      switchResolution;
    int16_t         desiredFrameRate;
    VsyncMode       vsyncMode;
    HWAccelMode     hwaccelMode;
    VideoCodec      videoCodec;
};

uint32_t constexpr VideoDataSize = sizeof(VideoData);

enum class VideoParameters
{
    DesktopResolution,
    SwitchResolution,
    DesiredFramerate,
    VsyncMode,
    HWAccelMode,
    VideoCodec
};

std::unordered_map<VideoParameters, std::string> VideoParamsToStr(VideoData const data);
VideoData VideoParamsFromStr(std::unordered_map<VideoParameters, std::string> const & map);

static std::vector<VideoParameters> const VideoParametersList
{
    VideoParameters::DesktopResolution, 
    VideoParameters::SwitchResolution,
    VideoParameters::DesiredFramerate,
    VideoParameters::VsyncMode,
    VideoParameters::HWAccelMode,
    VideoParameters::VideoCodec
};

static std::unordered_map<VideoParameters, std::string> const VideoParamToDesc
{
    { VideoParameters::DesktopResolution, "Desktop Resolution" },
    { VideoParameters::SwitchResolution, "Switch Resolution" },
    { VideoParameters::DesiredFramerate, "Desired Framerate" },
    { VideoParameters::VsyncMode, "Vsync Mode" },
    { VideoParameters::HWAccelMode, "HW Accel Mode" },
    { VideoParameters::VideoCodec, "Codec" },
};

#endif
