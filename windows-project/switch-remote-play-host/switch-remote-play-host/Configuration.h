#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

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
    Configuration(std::string parentFolder);

    bool SaveFfmpegEncoderOutputOption(bool enabled);
    bool ShowFfmpegEncoderOutput();

    bool SaveFfmpegAudioEncoderOutputOption(bool enabled);
    bool ShowFfmpegAudioEncoderOutputWindow();

private:
    auto static constexpr filename = "config.ini";
    std::string parentFolder;
    std::string absolutePath;
    std::string data;

    std::string ReadConfigFile(std::string const path);
    bool SaveConfigFile(std::string const & data);
    bool ExtractVariable(std::string const data, std::string const variable, std::string& result) const;
    bool ReplaceVariable(std::string const data, std::string const variable, std::string const value, std::string& newData) const;
};

#endif
