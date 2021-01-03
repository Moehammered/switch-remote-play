#include "Configuration.h"
#include "FileOperations.h"
#include "../network/NetworkData.h"
#include "../dataHelpers/HWAccel.h"
#include "../dataHelpers/VsyncMode.h"
#include "../dataHelpers/Resolution.h"
#include "../dataHelpers/EncoderPreset.h"
#include "../dataHelpers/VideoCodecMode.h"
#include "../dataHelpers/ControllerMode.h"
#include <iostream>

auto constexpr MANUAL_IP_TAG = "manual_ip";
auto constexpr FPS_TAG = "desired_framerate";
auto constexpr DESKTOP_RES_TAG = "desktop_resolution";
auto constexpr SWITCH_RES_TAG = "switch_resolution";
auto constexpr BITRATE_TAG = "bitrate_kb";
auto constexpr VSYNC_TAG = "vsync_mode";
auto constexpr ENCODER_PRESET_TAG = "encoder_preset";
auto constexpr QUALITY_FACTOR_TAG = "quality_control_factor";
auto constexpr VIDEO_CODEC_TAG = "video_codec";
auto constexpr HWACCEL_TAG = "hwaccel_mode";
auto constexpr MOUSE_SENS_TAG = "mouse_sensitivity";
auto constexpr CONTROLLER_MODE_TAG = "controller_mode";



Configuration::Configuration() : data{}
{
    data = ReadConfigFile(absolutePath);
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
        temp.desiredFrameRate = 60;

    auto videoResText = std::string{};
    if(ExtractVariable(data, DESKTOP_RES_TAG, videoResText))
    {
        auto res = ParseResolutionString(videoResText);
        temp.videoX = res.width;
        temp.videoY = res.height;
    }
    else
    {
        std::cout << "failed to extract video res" << std::endl;
        temp.videoX = 1280;
        temp.videoY = 720;
    }

    auto scaleResText = std::string{};
    if(ExtractVariable(data, SWITCH_RES_TAG, scaleResText))
    {
        auto res = ParseResolutionString(scaleResText);
        temp.scaleX = res.width;
        temp.scaleY = res.height;
    }
    else
    {
        temp.scaleX = 1280;
        temp.scaleY = 720;
    }

    {
        auto bitrateText = std::string{};
        if(ExtractVariable(data, BITRATE_TAG, bitrateText))
            temp.bitrateKB = atoi(bitrateText.c_str());
        else
            temp.bitrateKB = 8192;
    }

    {
        auto vsyncText = std::string{};
        if(ExtractVariable(data, VSYNC_TAG, vsyncText))
            temp.vsyncMode = ParseVsyncModeString(vsyncText);
        else
            temp.vsyncMode = VsyncMode::VARIABLE_FPS;
    }

    {
        auto presetText = std::string{};
        if(ExtractVariable(data, ENCODER_PRESET_TAG, presetText))
            temp.preset = ParseEncoderPresetString(presetText);
        else
            temp.preset = EncoderPreset::ULTRAFAST;
    }

    {
        std::string qcf{};
        if(ExtractVariable(data, QUALITY_FACTOR_TAG, qcf))
            temp.constantRateFactor = atoi(qcf.c_str());
        else
            temp.constantRateFactor = 16;
    }

    {
        std::string codec{};
        if(ExtractVariable(data, VIDEO_CODEC_TAG, codec))
            temp.videoCodecMode = ParseVideoCodecModeString(codec);
        else
            temp.videoCodecMode = VideoCodecMode::H264;
    }

    {
        std::string hwaccel{};
        if(ExtractVariable(data, HWACCEL_TAG, hwaccel))
            temp.hwaccelMode = ParseHWAccelString(hwaccel);
        else
            temp.hwaccelMode = HWAccelMode::AUTO;
    }

    {
        std::string mouse{};
        if(ExtractVariable(data, MOUSE_SENS_TAG, mouse))
            temp.mouseSensitivity = atoi(mouse.c_str());
        else
            temp.mouseSensitivity = 10;
    }
    return temp;
}

bool Configuration::SaveFFMPEG(FFMPEG_Config const data)
{
    auto newData = std::string{this->data};
    
    {
        auto fps = std::to_string(data.desiredFrameRate);
        if(!ReplaceVariable(newData, FPS_TAG, fps, newData))
        {
            std::cout << FPS_TAG << " variable not found. Appended to config.\n";
            newData += std::string{FPS_TAG} + "=" + fps + ";\n";
        }
    }

    {
        auto videoResStr = ResolutionToString({data.videoX, data.videoY});
        if(!ReplaceVariable(newData, DESKTOP_RES_TAG, videoResStr, newData))
        {
            std::cout << DESKTOP_RES_TAG << " variable not found. Appended to config.\n";
            newData += std::string{DESKTOP_RES_TAG} + "=" + videoResStr + ";\n";
        }
    }

    {
        auto videoScaleStr = ResolutionToString({data.scaleX, data.scaleY});
        if(!ReplaceVariable(newData, SWITCH_RES_TAG, videoScaleStr, newData))
        {
            std::cout << SWITCH_RES_TAG << " variable not found. Appended to config.\n";
            newData += std::string{SWITCH_RES_TAG} + "=" + videoScaleStr + ";\n";
        }
    }

    {
        auto br = std::to_string(data.bitrateKB);
        if(!ReplaceVariable(newData, BITRATE_TAG, br, newData))
        {
            std::cout << BITRATE_TAG << " variable not found. Appended to config.\n";
            newData += std::string{BITRATE_TAG} + "=" + br + ";\n";
        }
    }

    {
        auto vsync = VsyncModeToString(data.vsyncMode);
        if(!ReplaceVariable(newData, VSYNC_TAG, vsync, newData))
        {
            std::cout << VSYNC_TAG << " variable not found. Appended to config.\n";
            newData += std::string{VSYNC_TAG} + "=" + vsync + ";\n";
        }
    }

    {
        auto preset = EncoderPresetToString(data.preset);
        if(!ReplaceVariable(newData, ENCODER_PRESET_TAG, preset, newData))
        {
            std::cout << ENCODER_PRESET_TAG << " variable not found. Appended to config.\n";
            newData += std::string{ENCODER_PRESET_TAG} + "=" + preset + ";\n";
        }
    }

    {
        auto crf = std::to_string(data.constantRateFactor);
        if(!ReplaceVariable(newData, QUALITY_FACTOR_TAG, crf, newData))
        {
            std::cout << QUALITY_FACTOR_TAG << " variable not found. Appended to config.\n";
            newData += std::string{QUALITY_FACTOR_TAG} + "=" + crf + ";\n";
        }
    }

    {
        auto codec = VideoCodecModeToString(data.videoCodecMode);
        if(!ReplaceVariable(newData, VIDEO_CODEC_TAG, codec, newData))
        {
            std::cout << VIDEO_CODEC_TAG << " variable not found. Appended to config.\n";
            newData += std::string{VIDEO_CODEC_TAG} + "=" + codec + ";\n";
        }
    }

    {
        auto hwaccel = HWAccelModeToString(data.hwaccelMode);
        if(!ReplaceVariable(newData, HWACCEL_TAG, hwaccel, newData))
        {
            std::cout << HWACCEL_TAG << " variable not found. Appended to config.\n";
            newData += std::string{HWACCEL_TAG} + "=" + hwaccel + ";\n";
        }
    }

    {
        auto mouse = std::to_string(data.mouseSensitivity);
        if(!ReplaceVariable(newData, MOUSE_SENS_TAG, mouse, newData))
        {
            std::cout << MOUSE_SENS_TAG << " variable not found. Appended to config.\n";
            newData += std::string{MOUSE_SENS_TAG} + "=" + mouse + ";\n";
        }
    }

    return SaveConfigFile(newData);
}

Controller_Config const Configuration::ControllerData() const
{
    auto temp = Controller_Config{};

    {
        std::string controllerMode{};
        if(ExtractVariable(data, CONTROLLER_MODE_TAG, controllerMode))
            temp.controllerMode = ParseControllerModeString(controllerMode);
        else
            temp.controllerMode = ControllerMode::X360;
    }

    return temp;
}

bool Configuration::SaveController(Controller_Config const data)
{
    auto newData = std::string{this->data};
    
    {
        auto controllerMode = ControllerModeToString(data.controllerMode);
        if(!ReplaceVariable(newData, CONTROLLER_MODE_TAG, controllerMode, newData))
        {
            std::cout << CONTROLLER_MODE_TAG << " variable not found. Appended to config.\n";
            newData += std::string{CONTROLLER_MODE_TAG} + "=" + controllerMode + ";\n";
        }
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