#include "GenericOptions.h"

std::unordered_map<VideoParameters, std::string> VideoParamsToStr(VideoData const data)
{
    auto values = std::unordered_map<VideoParameters, std::string>{};

    values[VideoParameters::DesktopResolution] = ResolutionToString(data.desktopResolution);
    values[VideoParameters::SwitchResolution] = ResolutionToString(data.switchResolution);
    values[VideoParameters::DesiredFramerate] = std::to_string(data.desiredFrameRate);
    values[VideoParameters::VsyncMode] = VsyncModeToStr(data.vsyncMode);
    values[VideoParameters::HWAccelMode] = HWAccelModeToStr(data.hwaccelMode);
    values[VideoParameters::VideoCodec] = VideoCodecToStr(data.videoCodec);

    return values;
}

VideoData VideoParamsFromStr(std::unordered_map<VideoParameters, std::string> const & map)
{
    auto data = VideoData();

    auto parse = [&](VideoParameters p, auto& member, auto def, auto convert)
    {
        auto itr = map.find(p);
        if (itr != map.end())
            member = convert(itr->second);
        else
            member = def;
    };

    parse(VideoParameters::DesktopResolution, data.desktopResolution, hdRes, ParseResolutionString);
    parse(VideoParameters::SwitchResolution, data.switchResolution, hdRes, ParseResolutionString);
    parse(VideoParameters::VsyncMode, data.vsyncMode, DefaultVsyncMode, VsyncModeStrToEnum);
    parse(VideoParameters::HWAccelMode, data.hwaccelMode, DefaultHWAccelMode, HWAccelStrToEnum);
    parse(VideoParameters::VideoCodec, data.videoCodec, DefaultVideoCodec, VideoCodecStrToEnum);
    
    auto fpsEntry = map.find(VideoParameters::DesiredFramerate);
    if(fpsEntry != map.end())
        data.desiredFrameRate = std::max(std::stoi(fpsEntry->second), (int)Framerates.front());
    else
        data.desiredFrameRate = Framerates.front();

    return data;
}
