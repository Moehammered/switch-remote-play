#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "NetworkData.h"
#include <string>
/* Configuration File Layout

this line gets ignored because there's no match

found_ip=192.168.0.19;
desired_framerate=60; comment goes here
video_resolution=1920x1080; another one here
video_scale=1280x720;
bitrate_kb=5120;
vsync_mode=2;

*/

class Configuration
{
public:
    Configuration();
    std::string const FoundIP() const;
    // bool SaveFoundIP(std::string const & ip);

    FFMPEG_Config const FFMPEGData() const;
    // bool SaveFFMPEG(FFMPEG_Config const & data);

private:
    auto static constexpr parentFolder = "sdmc:/switch/switch-remote-play";
    auto static constexpr filename = "config.ini";
    auto static constexpr absolutePath = "sdmc:/switch/switch-remote-play/config.ini";
    std::string data;

    std::string ReadConfigFile(std::string const path);
    bool SaveConfigFile(std::string const & data);
    bool ExtractVariable(std::string const data, std::string const variable, std::string& result) const;
};

#endif
