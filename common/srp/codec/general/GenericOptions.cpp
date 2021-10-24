#include "GenericOptions.h"
#include <algorithm>

namespace codec
{
    std::unordered_map<VideoParameters, std::string> videoParamsToStr(VideoData const data)
    {
        auto values = std::unordered_map<VideoParameters, std::string>{};

        values[VideoParameters::DesktopResolution] = resolutionToString(data.desktopResolution);
        values[VideoParameters::SwitchResolution] = resolutionToString(data.switchResolution);
        values[VideoParameters::DesiredFramerate] = std::to_string(data.desiredFrameRate);
        values[VideoParameters::BitrateKB] = std::to_string(data.bitrateKB);
        values[VideoParameters::VsyncMode] = ffmpeg::vsyncModeToStr(data.vsyncMode);
        values[VideoParameters::HWAccelMode] = ffmpeg::hwAccelModeToStr(data.hwaccelMode);
        values[VideoParameters::VideoCodec] = ffmpeg::videoCodecToStr(data.videoCodec);
        values[VideoParameters::MonitorNumber] = std::to_string(data.monitorNumber);

        return values;
    }

    VideoData videoParamsFromStr(std::unordered_map<VideoParameters, std::string> const & map)
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

        parse(VideoParameters::DesktopResolution, data.desktopResolution, hdRes, parseResolutionString);
        parse(VideoParameters::SwitchResolution, data.switchResolution, hdRes, parseResolutionString);
        parse(VideoParameters::VsyncMode, data.vsyncMode, ffmpeg::defaultVsyncMode, ffmpeg::vsyncModeStrToEnum);
        parse(VideoParameters::HWAccelMode, data.hwaccelMode, ffmpeg::defaultHWAccelMode, ffmpeg::hwAccelFromStr);
        parse(VideoParameters::VideoCodec, data.videoCodec, ffmpeg::defaultVideoCodec, ffmpeg::videoCodecStrToEnum);
        
        auto fpsEntry = map.find(VideoParameters::DesiredFramerate);
        if(fpsEntry != map.end())
        {
            auto fps = (int16_t)std::stoi(fpsEntry->second);
            data.desiredFrameRate = std::clamp(fps, minDesiredFramerate, maxDesiredFramerate);
        }
        else
            data.desiredFrameRate = defaultDesiredFramerate;

        auto bitrateEntry = map.find(VideoParameters::BitrateKB);
        if(bitrateEntry != map.end())
        {
            auto highest = bitrates.back();
            auto lowest = bitrates.front();
            auto entry = std::stoi(bitrateEntry->second);
            auto value = std::max(entry, (int)lowest);
            value = std::min(value, (int)highest);
            data.bitrateKB = (int16_t)value;
        }
        else
            data.bitrateKB = bitrates.front();

        auto monitorNumberEntry = map.find(VideoParameters::MonitorNumber);
        if(monitorNumberEntry != map.end())
        {
            auto const maxMonitorIndex = 10;
            auto const minMonitorIndex = 0;
            auto number = std::stoi(monitorNumberEntry->second);
            auto upperLimited = std::min(number, maxMonitorIndex);
            auto lowerLimited = std::max(upperLimited, minMonitorIndex);
            data.monitorNumber = lowerLimited;
        }
        else
            data.monitorNumber = 0;

        return data;
    }
}