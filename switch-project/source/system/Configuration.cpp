#include "Configuration.h"
#include "FileOperations.h"

#include <iostream>

auto constexpr MANUAL_IP_TAG = "manual_ip";
auto constexpr IP_TAG = "found_ip";
auto constexpr FPS_TAG = "desired_framerate";
auto constexpr VIDEO_RES_TAG = "video_resolution";
auto constexpr VIDEO_SCALE_TAG = "video_scale";
auto constexpr BITRATE_TAG = "bitrate_kb";
auto constexpr VSYNC_TAG = "vsync_mode";

Configuration::Configuration() : data{}
{
    data = ReadConfigFile(absolutePath);
}

std::string const Configuration::FoundIP() const
{
    auto ip = std::string{};
    if(ExtractVariable(data, IP_TAG, ip))
        return ip;
    else
        return std::string{};
}

bool Configuration::SaveFoundIP(std::string const ip)
{
    auto newData = std::string{};
    if(ReplaceVariable(data, IP_TAG, ip, newData))
        return SaveConfigFile(newData);
    else
    {
        newData = std::string{IP_TAG} + "=" + ip + ";\n";
        newData += data;
        return SaveConfigFile(newData);
    }
}

std::string const Configuration::ManualIP() const
{
    auto ip = std::string{};
    if(ExtractVariable(data, MANUAL_IP_TAG, ip))
        return ip;
    else
        return std::string{};
}

bool Configuration::SaveManualIP(std::string const ip)
{
    auto newData = std::string{};
    if(ReplaceVariable(data, MANUAL_IP_TAG, ip, newData))
        return SaveConfigFile(newData);
    else
    {
        newData = std::string{MANUAL_IP_TAG} + "=" + ip + ";\n";
        newData += data;
        return SaveConfigFile(newData);
    }
}

FFMPEG_Config const Configuration::FFMPEGData() const
{
    auto temp = FFMPEG_Config{};

    auto fpsText = std::string{};
    if(ExtractVariable(data, FPS_TAG, fpsText))
        temp.desiredFrameRate = atoi(fpsText.c_str());
    else
        temp.desiredFrameRate = 30;

    auto videoResText = std::string{};
    if(ExtractVariable(data, VIDEO_RES_TAG, videoResText))
    {
        auto widthStart = 0;
        auto widthEnd = videoResText.find('x', 0);
        auto heightStart = widthEnd+1;
        auto heightEnd = videoResText.length();

        if(widthEnd == std::string::npos || heightEnd == std::string::npos)
        {
            std::cout << "failed to find delimiters for " << videoResText << ": " << widthEnd << ", " << heightEnd << std::endl;
            temp.videoX = 1920;
            temp.videoY = 1080;
        }
        else
        {
            auto width = videoResText.substr(widthStart, widthEnd-widthStart);
            auto height = videoResText.substr(heightStart, heightEnd-heightStart);
            temp.videoX = atoi(width.c_str());
            temp.videoY = atoi(height.c_str());
        }
    }
    else
    {
        std::cout << "failed to extract video res" << std::endl;
        temp.videoX = 1920;
        temp.videoY = 1080;
    }

    auto scaleResText = std::string{};
    if(ExtractVariable(data, VIDEO_SCALE_TAG, scaleResText))
    {
        auto widthStart = 0;
        auto widthEnd = scaleResText.find('x', 0);
        auto heightStart = widthEnd+1;
        auto heightEnd = scaleResText.length();

        if(widthEnd == std::string::npos || heightEnd == std::string::npos)
        {
            std::cout << "failed to find delimiters for " << scaleResText << ": " << widthEnd << ", " << heightEnd << std::endl;
            temp.scaleX = 1280;
            temp.scaleY = 720;
        }
        else
        {
            auto width = scaleResText.substr(widthStart, widthEnd-widthStart);
            auto height = scaleResText.substr(heightStart, heightEnd-heightStart);
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
    if(ExtractVariable(data, BITRATE_TAG, bitrateText))
        temp.bitrateKB = atoi(bitrateText.c_str());
    else
        temp.bitrateKB = 5120;

    auto vsyncText = std::string{};
    if(ExtractVariable(data, VSYNC_TAG, vsyncText))
        temp.vsyncMode = atoi(vsyncText.c_str());
    else
        temp.vsyncMode = 2;

    return temp;
}

bool Configuration::SaveFFMPEG(FFMPEG_Config const data)
{
    auto newData = std::string{this->data};
    
    auto fps = std::to_string(data.desiredFrameRate);
    if(!ReplaceVariable(newData, FPS_TAG, fps, newData))
    {
        std::cout << "Save error --> couldn't replace " << FPS_TAG << std::endl;
        return false;
    }

    auto vx = std::to_string(data.videoX);
    auto vy = std::to_string(data.videoY);
    auto videoResStr = vx+"x"+vy;
    
    if(!ReplaceVariable(newData, VIDEO_RES_TAG, videoResStr, newData))
    {
        std::cout << "Save error --> couldn't replace " << VIDEO_RES_TAG << std::endl;
        return false;
    }
    
    auto sx = std::to_string(data.scaleX);
    auto sy = std::to_string(data.scaleY);
    auto videoScaleStr = sx+"x"+sy;
    if(!ReplaceVariable(newData, VIDEO_SCALE_TAG, videoScaleStr, newData))
    {
        std::cout << "Save error --> couldn't replace " << VIDEO_SCALE_TAG << std::endl;
        return false;
    }

    auto br = std::to_string(data.bitrateKB);
    if(!ReplaceVariable(newData, "bitrate_kb", br, newData))
    {
        std::cout << "Save error --> couldn't replace " << BITRATE_TAG << std::endl;
        return false;
    }

    auto vsync = std::to_string(data.vsyncMode);
    if(!ReplaceVariable(newData, VSYNC_TAG, vsync, newData))
    {
        std::cout << "Save error --> couldn't replace " << VSYNC_TAG << std::endl;
        return false;
    }
    
    return SaveConfigFile(newData);
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

bool Configuration::ReplaceVariable(std::string const data, std::string const variable, std::string const value, std::string& newData) const
{
    auto variableStart = data.find(variable+"=", 0);

    if(variableStart != std::string::npos)
    {
        auto varEnd = data.find(';', variableStart);

        if(varEnd == std::string::npos)
            return false;
        else
        {
            auto copy = std::string{data};
            copy.replace(variableStart, varEnd-variableStart, variable+"="+value);
            newData = copy;
            return true;
        }
    }
    else
    {
        std::cout << "couldn't find variable: " << variable+"= in " << std::endl << newData << std::endl;
        return false;
    }
}