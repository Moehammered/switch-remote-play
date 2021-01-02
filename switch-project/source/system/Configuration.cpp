#include "Configuration.h"
#include "FileOperations.h"
#include "../network/NetworkData.h"

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

int16_t VsyncTextToEnum(std::string s)
{
    if(s == "auto")
        return -1;
    else if(s == "passthrough")
        return 0;
    else if(s == "constant frame rate")
        return 1;
    else if(s == "variable frame rate")
        return 2;
    else if(s == "drop time")
        return 3;
    else
        return 2;
}

std::string VsyncToText(const int16_t mode)
{
    switch(mode)
    {
        case -1:
            return "auto";

        case 0:
            return "passthrough";

        case 1:
            return "constant frame rate";

        default:
        case 2:
            return "variable frame rate";

        case 3:
            return "drop time";
    }
}

std::string PresetEnumToText(EncoderPreset preset)
{
    switch(preset)
    {
        default:
        case EncoderPreset::ULTRAFAST:
            return "ultrafast";
        case EncoderPreset::VERYFAST:
            return "veryfast";
        case EncoderPreset::FAST:
            return "fast";
        case EncoderPreset::MEDIUM:
            return "medium";
        case EncoderPreset::SLOW:
            return "slow";
        case EncoderPreset::VERYSLOW:
            return "veryslow";
    }
}

EncoderPreset PresetTextToEnum(std::string s)
{
    if(s == "ultrafast")
        return EncoderPreset::ULTRAFAST;
    else if(s == "veryfast")
        return EncoderPreset::VERYFAST;
    else if(s == "fast")
        return EncoderPreset::FAST;
    else if(s == "medium")
        return EncoderPreset::MEDIUM;
    else if(s == "slow")
        return EncoderPreset::SLOW;
    else if(s == "veryslow")
        return EncoderPreset::VERYSLOW;
    else
        return EncoderPreset::ULTRAFAST;
}

std::string HWAccelEnumToText(HWAccelMode mode)
{
    switch(mode)
    {
        default:
        case HWAccelMode::AUTO:
            return "auto";
        case HWAccelMode::DXVA2:
            return "dxva2";
        case HWAccelMode::VAAPI:
            return "vaapi";
        case HWAccelMode::CUDA:
            return "cuda";
    }
}

HWAccelMode HWAccelTextToEnum(std::string s)
{
    if(s == "auto")
        return HWAccelMode::AUTO;
    else if(s == "dxva2")
        return HWAccelMode::DXVA2;
    else if(s == "cuda")
        return HWAccelMode::CUDA;
    else if(s == "vaapi")
        return HWAccelMode::VAAPI;
    else
        return HWAccelMode::AUTO;
}

std::string VideoCodecEnumToText(VideoCodecMode mode)
{
    switch(mode)
    {
        default:
        case VideoCodecMode::H264:
            return "h264";
        case VideoCodecMode::H264_AMF:
            return "h264_amf";
        case VideoCodecMode::H264_NVENC:
            return "h264_nvenc";
        case VideoCodecMode::H264_QSV:
            return "h264_qsv";
    }
}

VideoCodecMode VideoCodecTextToEnum(std::string s)
{
    if(s == "h264")
        return VideoCodecMode::H264;
    else if(s == "h264_amf")
        return VideoCodecMode::H264_AMF;
    else if(s == "h264_nvenc")
        return VideoCodecMode::H264_NVENC;
    else if(s == "h264_qsv")
        return VideoCodecMode::H264_QSV;
    else
        return VideoCodecMode::H264;
}

std::string ControllerModeEnumToText(ControllerMode controllerMode)
{
    switch(controllerMode)
    {
        default:
        case ControllerMode::X360:
            return "x360";
        case ControllerMode::DS4:
            return "ds4";
    }
}

ControllerMode ControllerModeTextToEnum(std::string s)
{
    if(s == "x360")
        return ControllerMode::X360;
    else if(s == "ds4")
        return ControllerMode::DS4;
    else
        return ControllerMode::X360;
}

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
        temp.videoX = 1280;
        temp.videoY = 720;
    }

    auto scaleResText = std::string{};
    if(ExtractVariable(data, SWITCH_RES_TAG, scaleResText))
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
            temp.vsyncMode = VsyncTextToEnum(vsyncText);
        else
            temp.vsyncMode = 2;
    }

    {
        auto presetText = std::string{};
        if(ExtractVariable(data, ENCODER_PRESET_TAG, presetText))
            temp.preset = PresetTextToEnum(presetText);
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
            temp.videoCodecMode = VideoCodecTextToEnum(codec);
        else
            temp.videoCodecMode = VideoCodecMode::H264;
    }

    {
        std::string hwaccel{};
        if(ExtractVariable(data, HWACCEL_TAG, hwaccel))
            temp.hwaccelMode = HWAccelTextToEnum(hwaccel);
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
        auto vx = std::to_string(data.videoX);
        auto vy = std::to_string(data.videoY);
        auto videoResStr = vx+"x"+vy;
        
        if(!ReplaceVariable(newData, DESKTOP_RES_TAG, videoResStr, newData))
        {
            std::cout << DESKTOP_RES_TAG << " variable not found. Appended to config.\n";
            newData += std::string{DESKTOP_RES_TAG} + "=" + videoResStr + ";\n";
        }
    }

    {
        auto sx = std::to_string(data.scaleX);
        auto sy = std::to_string(data.scaleY);
        auto videoScaleStr = sx+"x"+sy;
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
        auto vsync = VsyncToText(data.vsyncMode);
        if(!ReplaceVariable(newData, VSYNC_TAG, vsync, newData))
        {
            std::cout << VSYNC_TAG << " variable not found. Appended to config.\n";
            newData += std::string{VSYNC_TAG} + "=" + vsync + ";\n";
        }
    }

    {
        auto preset = PresetEnumToText(data.preset);
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
        auto codec = VideoCodecEnumToText(data.videoCodecMode);
        if(!ReplaceVariable(newData, VIDEO_CODEC_TAG, codec, newData))
        {
            std::cout << VIDEO_CODEC_TAG << " variable not found. Appended to config.\n";
            newData += std::string{VIDEO_CODEC_TAG} + "=" + codec + ";\n";
        }
    }

    {
        auto hwaccel = HWAccelEnumToText(data.hwaccelMode);
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
            temp.controllerMode = ControllerModeTextToEnum(controllerMode);
        else
            temp.controllerMode = ControllerMode::X360;
    }

    return temp;
}

bool Configuration::SaveController(Controller_Config const data)
{
    auto newData = std::string{this->data};
    
    {
        auto controllerMode = ControllerModeEnumToText(data.controllerMode);
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