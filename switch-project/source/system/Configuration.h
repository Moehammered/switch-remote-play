#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "../network/NetworkData.h"
#include "../dataHelpers/DecoderFlags.h"
#include <string>
/* Configuration File Layout

this line gets ignored because there's no match

manual_ip=192.168.0.19; <-- semicolon marks the end of the variable
desired_framerate=60; comment goes here
desktop_resolution=1920x1080; another one here
switch_resolution=1280x720;
bitrate_kb=5120;
vsync_mode=variable frame rate;
hwaccel_mode=auto;
video_codec=h264;
quality_control_factor=16;
*/

class Configuration
{
public:
    Configuration();

    bool ManualIPEnabled() const;
    bool SaveManualIPEnabled(bool enabled);

    std::string const ManualIP() const;
    bool SaveManualIP(std::string const ip);

    std::string BroadcastAddress() const;

    EncoderConfig const FFMPEGData() const;
    bool SaveFFMPEG(EncoderConfig const data);

    ControllerConfig const ControllerData() const;
    bool SaveController(ControllerConfig const data);

    DecoderConfiguration const DecoderData() const;
    bool SaveDecoderConfig(DecoderConfiguration const data);

private:
    auto static constexpr parentFolder = "sdmc:/switch/switch-remote-play";
    auto static constexpr filename = "config.ini";
    auto static constexpr absolutePath = "sdmc:/switch/switch-remote-play/config.ini";
    std::string data;

    std::string ReadConfigFile(std::string const path);
    bool SaveConfigFile(std::string const & data);
    bool ExtractVariable(std::string const data, std::string const variable, std::string& result) const;
    bool ReplaceVariable(std::string const data, std::string const variable, std::string const value, std::string& newData) const;
};

#endif
