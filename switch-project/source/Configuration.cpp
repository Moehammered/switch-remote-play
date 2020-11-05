#include "Configuration.h"
#include "FileOperations.h"

Configuration::Configuration() : data{}
{
    data = ReadConfigFile(absolutePath);
}

std::string const Configuration::FoundIP() const
{
    auto ip = std::string{};
    if(ExtractVariable(data, "found_ip", ip))
        return ip;
    else
        return std::string{};
    // auto location = data.find("found_ip=", 0);
    // if(location != std::string::npos)
    // {
    //     auto ipStart = data.find('=', location);
    //     auto end = data.find(';', location);

    //     if(ipStart == std::string::npos || end == std::string::npos)
    //         return std::string{};
    //     else
    //     {
    //         return data.substr(ipStart+1, end-ipStart-1);
    //     }
    // }
    // else
    //     return std::string{};
}

FFMPEG_Config const Configuration::FFMPEGData() const
{
    auto temp = FFMPEG_Config{};

    auto fpsText = std::string{};
    if(ExtractVariable(data, "desired_framerate", fpsText))
        temp.desiredFrameRate = atoi(fpsText.c_str());
    else
        temp.desiredFrameRate = 30;

    auto videoResText = std::string{};
    if(ExtractVariable(data, "video_resolution", videoResText))
    {
        auto widthStart = 0;
        auto widthEnd = videoResText.find('x', 0);
        auto heightStart = widthEnd+1;
        auto heightEnd = videoResText.find(';', widthEnd);

        if(widthEnd == std::string::npos || heightEnd == std::string::npos)
        {
            temp.videoX = 1920;
            temp.videoY = 1080;
        }
        else
        {
            auto width = videoResText.substr(widthStart, widthEnd-widthStart-1);
            auto height = videoResText.substr(heightStart, heightEnd-heightStart-1);
            temp.videoX = atoi(width.c_str());
            temp.videoY = atoi(height.c_str());
        }
    }
    else
    {
        temp.videoX = 1920;
        temp.videoY = 1080;
    }

    auto scaleResText = std::string{};
    if(ExtractVariable(data, "video_scale", scaleResText))
    {
        auto widthStart = 0;
        auto widthEnd = scaleResText.find('x', 0);
        auto heightStart = widthEnd+1;
        auto heightEnd = scaleResText.find(';', widthEnd);

        if(widthEnd == std::string::npos || heightEnd == std::string::npos)
        {
            temp.scaleX = 1280;
            temp.scaleY = 720;
        }
        else
        {
            auto width = scaleResText.substr(widthStart, widthEnd-widthStart-1);
            auto height = scaleResText.substr(heightStart, heightEnd-heightStart-1);
            temp.scaleX = atoi(width.c_str());
            temp.scaleY = atoi(height.c_str());
        }
    }
    else
    {
        temp.scaleX = 1280;
        temp.scaleY = 720;
    }

    auto bitrateText = std::string{};
    if(ExtractVariable(data, "bitrate_kb", bitrateText))
        temp.bitrateKB = atoi(bitrateText.c_str());
    else
        temp.bitrateKB = 5120;

    auto vsyncText = std::string{};
    if(ExtractVariable(data, "vsync_mode", vsyncText))
        temp.vsyncMode = atoi(vsyncText.c_str());
    else
        temp.vsyncMode = 2;

    return temp;
}

std::string Configuration::ReadConfigFile(std::string const path)
{
    auto temp = std::string{};
    readFileAsText(path, temp);
    return temp;
}

bool Configuration::SaveConfigFile(std::string const & data)
{
    if(saveTextToFile(absolutePath, data))
    {
        this->data = data;
        return true;
    }

    return false;
}

bool Configuration::ExtractVariable(std::string const data, std::string const variable, std::string& result) const
{
    auto location = data.find(variable+"=", 0);
    result = std::string{};

    if(location != std::string::npos)
    {
        auto varStart = data.find('=', location);
        auto varEnd = data.find(';', location);

        if(varStart == std::string::npos || varEnd == std::string::npos)
            return false;
        else
        {
            auto varLength = varEnd-varStart-1;
            result = data.substr(varStart+1, varLength);
            return true;
        }
    }
    else
        return false;
}
