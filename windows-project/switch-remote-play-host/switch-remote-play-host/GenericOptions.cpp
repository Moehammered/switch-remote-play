#include "GenericOptions.h"

std::unordered_map<VideoParameters, std::string> VideoParamsToStr(VideoData const data)
{
    auto values = std::unordered_map<VideoParameters, std::string>{};

    values[VideoParameters::DesktopResolution] = ResolutionToString(data.desktopResolution);
    values[VideoParameters::SwitchResolution] = ResolutionToString(data.switchResolution);
    values[VideoParameters::DesiredFramerate] = std::to_string(data.desiredFrameRate);
    values[VideoParameters::BitrateKB] = std::to_string(data.bitrateKB);
    values[VideoParameters::VsyncMode] = VsyncModeToStr(data.vsyncMode);
    values[VideoParameters::HWAccelMode] = HWAccelModeToStr(data.hwaccelMode);
    values[VideoParameters::VideoCodec] = VideoCodecToStr(data.videoCodec);
    values[VideoParameters::MonitorNumber] = std::to_string(data.monitorNumber);

    return values;
}

VideoData VideoParamsFromStr(std::unordered_map<VideoParameters, std::string> const& map)
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
    if (fpsEntry != map.end())
        data.desiredFrameRate = std::max(std::stoi(fpsEntry->second), (int)Framerates.front());
    else
        data.desiredFrameRate = Framerates.front();

    auto bitrateEntry = map.find(VideoParameters::BitrateKB);
    if (bitrateEntry != map.end())
    {
        auto highest = Bitrates.back();
        auto lowest = Bitrates.front();
        auto entry = std::stoi(bitrateEntry->second);
        auto value = std::max(entry, (int)lowest);
        value = std::min(value, (int)highest);
        data.bitrateKB = (int16_t)value;
    }
    else
        data.bitrateKB = Bitrates.front();

    auto monitorNumberEntry = map.find(VideoParameters::MonitorNumber);
    if (monitorNumberEntry != map.end())
        data.monitorNumber = std::max(std::min(std::stoi(monitorNumberEntry->second), 0), 10);
    else
        data.monitorNumber = 0;

    return data;
}
