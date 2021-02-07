#include "Configuration.h"
#include "FileOperations.h"
#include "../network/NetworkData.h"
#include "../dataHelpers/HWAccel.h"
#include "../dataHelpers/VsyncMode.h"
#include "../dataHelpers/Resolution.h"
#include "../dataHelpers/EncoderPreset.h"
#include "../dataHelpers/VideoCodecMode.h"
#include "../dataHelpers/ControllerMode.h"
#include "../dataHelpers/WordDelimiter.h"
#include "../dataHelpers/SwitchButtons.h"
#include <sstream>
#include <iterator>
#include <iostream>
#include <switch/services/hid.h>

auto constexpr MANUAL_IP_ENABLED_TAG ="use_manual_mode";
auto constexpr BROADCAST_ADDRESS_TAG ="broadcast_address";
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
auto constexpr CONTROLLER_BTN_MAP_TAG = "controller_map";
auto constexpr MOUSE_LEFT_CLICK_TAG = "mouse_left_click";
auto constexpr MOUSE_RIGHT_CLICK_TAG = "mouse_right_click";
auto constexpr MOUSE_ON_CONNECT_TAG = "mouse_on_connect";
auto constexpr DECODER_FLAG1_TAG = "decoder_flags1";
auto constexpr DECODER_FLAG2_TAG = "decoder_flags2";
auto constexpr DECODER_HWACCEL_FLAG_TAG = "decoder_hwaccelflags";
auto constexpr DECODER_SKIP_LOOP_FILTER_TAG = "decoder_skip_loop_filter";
auto constexpr DECODER_THREAD_TYPE_TAG = "decoder_thread_type";
auto constexpr DECODER_THREAD_COUNT_TAG = "decoder_thread_count";

Configuration::Configuration() : data{}
{
    data = ReadConfigFile(absolutePath);
}

bool Configuration::ManualIPEnabled() const
{
    auto option = std::string{};
    if(ExtractVariable(data, MANUAL_IP_ENABLED_TAG, option))
        return option == "yes";
    else
        return false;
}

bool Configuration::SaveManualIPEnabled(bool enabled)
{
    auto newData = std::string{};
    auto option = std::string{enabled ? "yes" : "no"};
    if(ReplaceVariable(data, MANUAL_IP_ENABLED_TAG, option, newData))
        return SaveConfigFile(newData);
    else
    {
        newData = std::string{MANUAL_IP_ENABLED_TAG} + "=" + option + ";\n";
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

std::string Configuration::BroadcastAddress() const
{
    auto addr = std::string{};
    if(ExtractVariable(data, BROADCAST_ADDRESS_TAG, addr))
    {
        auto dotCount = 0;
        for(auto const & c : addr)
        {
            if(c == '.')
                ++dotCount;
        }
        if(dotCount == 3)
            return addr;
        else
        {
            std::cout << "Invalid broadcast address format! Defaulting to 192.168.0.255";
            return "192.168.0.255";
        }
    }
    else
        return "192.168.0.255";
}

EncoderConfig const Configuration::FFMPEGData() const
{
    auto temp = EncoderConfig{};

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

    return temp;
}

bool Configuration::SaveFFMPEG(EncoderConfig const data)
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

    return SaveConfigFile(newData);
}

ControllerConfig const Configuration::ControllerData() const
{
    auto temp = ControllerConfig{};

    {
        std::string controllerMode{};
        if(ExtractVariable(data, CONTROLLER_MODE_TAG, controllerMode))
            temp.controllerMode = ParseControllerModeString(controllerMode);
        else
            temp.controllerMode = ControllerMode::X360;
    }

    {
        std::string controllerMap{};
        if(ExtractVariable(data, CONTROLLER_BTN_MAP_TAG, controllerMap))
            temp.controllerMap = ParseControllerButtonMapString(controllerMap);
        else
            temp.controllerMap = ControllerButtonMap::CONTROLLER_MAP_DEFAULT;
    }

    {
        std::string leftClick{};
        if(ExtractVariable(data, MOUSE_LEFT_CLICK_TAG, leftClick))
            temp.leftClickButton = ParseSwitchButtonString(leftClick);
        else
            temp.leftClickButton = KEY_L;
    }

    {
        std::string rightClick{};
        if(ExtractVariable(data, MOUSE_RIGHT_CLICK_TAG, rightClick))
            temp.rightClickButton = ParseSwitchButtonString(rightClick);
        else
            temp.rightClickButton = KEY_R;
    }

    {
        std::string mouseSensitivity{};
        if(ExtractVariable(data, MOUSE_SENS_TAG, mouseSensitivity))
            temp.mouseSensitivity = atoi(mouseSensitivity.c_str());
        else
            temp.mouseSensitivity = 10;
    }

    {
        std::string mouseOnConnect{};
        if(ExtractVariable(data, MOUSE_ON_CONNECT_TAG, mouseOnConnect))
            temp.mouseOnConnect = mouseOnConnect == "yes" ? true : false;
        else
            temp.mouseOnConnect = false;
    }

    return temp;
}

bool Configuration::SaveController(ControllerConfig const data)
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

    {
        auto controllerMap = ControllerButtonMapToString(data.controllerMap);
        if(!ReplaceVariable(newData, CONTROLLER_BTN_MAP_TAG, controllerMap, newData))
        {
            std::cout << CONTROLLER_BTN_MAP_TAG << " variable not found. Appended to config.\n";
            newData += std::string{CONTROLLER_BTN_MAP_TAG} + "=" + controllerMap + ";\n";
        }
    }

    {
        auto leftClickButton = SwitchButtonToString(data.leftClickButton);
        if(!ReplaceVariable(newData, MOUSE_LEFT_CLICK_TAG, leftClickButton, newData))
        {
            std::cout << MOUSE_LEFT_CLICK_TAG << " variable not found. Appended to config.\n";
            newData += std::string{MOUSE_LEFT_CLICK_TAG} + "=" + leftClickButton + ";\n";
        }
    }

    {
        auto rightClickButton = SwitchButtonToString(data.rightClickButton);
        if(!ReplaceVariable(newData, MOUSE_RIGHT_CLICK_TAG, rightClickButton, newData))
        {
            std::cout << MOUSE_RIGHT_CLICK_TAG << " variable not found. Appended to config.\n";
            newData += std::string{MOUSE_RIGHT_CLICK_TAG} + "=" + rightClickButton + ";\n";
        }
    }

    {
        auto mouseOnConnect = data.mouseOnConnect ? "yes" : "no";
        if(!ReplaceVariable(newData, MOUSE_ON_CONNECT_TAG, mouseOnConnect, newData))
        {
            std::cout << MOUSE_ON_CONNECT_TAG << " variable not found. Appended to config.\n";
            newData += std::string{MOUSE_ON_CONNECT_TAG} + "=" + mouseOnConnect + ";\n";
        }
    }

    {
        auto mouseSensitivity = std::to_string(data.mouseSensitivity);
        if(!ReplaceVariable(newData, MOUSE_SENS_TAG, mouseSensitivity, newData))
        {
            std::cout << MOUSE_SENS_TAG << " variable not found. Appended to config.\n";
            newData += std::string{MOUSE_SENS_TAG} + "=" + mouseSensitivity + ";\n";
        }
    }

    return SaveConfigFile(newData);
}

DecoderConfiguration const Configuration::DecoderData() const
{
    auto temp = DecoderConfiguration{};

    {
        std::string flag1{};
        if(ExtractVariable(data, DECODER_FLAG1_TAG, flag1))
        {
            auto flags = split(flag1, ',');
            temp.flag1 = ParseDecoderFlags1Strings(flags);
        }
        else
            temp.flag1 = DefaultDecoderFlags1;
    }

    {
        std::cout << "Flag 2 loading...\n";
        std::string flag2{};
        if(ExtractVariable(data, DECODER_FLAG2_TAG, flag2))
        {
            auto flags = split(flag2, ',');
            temp.flag2 = ParseDecoderFlags2Strings(flags);
        }
        else
            temp.flag2 = DefaultDecoderFlags2;
    }

    {
        std::string hwFlags{};
        if(ExtractVariable(data, DECODER_HWACCEL_FLAG_TAG, hwFlags))
        {
            auto flags = split(hwFlags, ',');
            temp.hwAccelFlags = ParseHWAccelFlagStrings(flags);
        }
        else
            temp.hwAccelFlags = DefaultHWAccelFlags;
    }

    {
        std::string skipLoopFilter{};
        if(ExtractVariable(data, DECODER_SKIP_LOOP_FILTER_TAG, skipLoopFilter))
            temp.skipLoopFilter = ParseAVDiscardString(skipLoopFilter);
        else
            temp.skipLoopFilter = DefaultSkipLoopFilter;
    }

    {
        std::string threadType{};
        if(ExtractVariable(data, DECODER_THREAD_TYPE_TAG, threadType))
            temp.threadType = ParseThreadTypeString(threadType);
        else
            temp.threadType = DefaultThreadType;
    }

    {
        std::string threadCount{};
        if(ExtractVariable(data, DECODER_THREAD_COUNT_TAG, threadCount))
            temp.threadCount = atoi(threadCount.c_str());
        else
            temp.threadCount = DefaultThreadCount;
    }

    return temp;
}

bool Configuration::SaveDecoderConfig(DecoderConfiguration const data)
{
    auto newData = std::string{this->data};
    
    {
        auto flag1 = DecoderFlags1ToStrings(data.flag1);
        std::string commaSeparatedFlags {};
        for(auto& f : flag1)
            commaSeparatedFlags += f.second + ",";

        commaSeparatedFlags = commaSeparatedFlags.substr(0, commaSeparatedFlags.size() - 1);
        if(!ReplaceVariable(newData, DECODER_FLAG1_TAG, commaSeparatedFlags, newData))
        {
            std::cout << DECODER_FLAG1_TAG << " variable not found. Appended to config.\n";
            newData += std::string{DECODER_FLAG1_TAG} + "=" + commaSeparatedFlags + ";\n";
        }
    }

    {
        auto flag2 = DecoderFlags2ToStrings(data.flag2);
        std::string commaSeparatedFlags {};
        for(auto& f : flag2)
            commaSeparatedFlags += f.second + ",";

        commaSeparatedFlags = commaSeparatedFlags.substr(0, commaSeparatedFlags.size() - 1);
        if(!ReplaceVariable(newData, DECODER_FLAG2_TAG, commaSeparatedFlags, newData))
        {
            std::cout << DECODER_FLAG2_TAG << " variable not found. Appended to config.\n";
            newData += std::string{DECODER_FLAG2_TAG} + "=" + commaSeparatedFlags + ";\n";
        }
    }

    {
        auto hwFlags = HWAccelFlagsToStrings(data.hwAccelFlags);
        std::string commaSeparatedFlags {};
        for(auto& f : hwFlags)
            commaSeparatedFlags += f.second + ",";

        commaSeparatedFlags = commaSeparatedFlags.substr(0, commaSeparatedFlags.size() - 1);
        if(!ReplaceVariable(newData, DECODER_HWACCEL_FLAG_TAG, commaSeparatedFlags, newData))
        {
            std::cout << DECODER_HWACCEL_FLAG_TAG << " variable not found. Appended to config.\n";
            newData += std::string{DECODER_HWACCEL_FLAG_TAG} + "=" + commaSeparatedFlags + ";\n";
        }
    }

    {
        auto skipLoopFilter = AVDiscardToString(data.skipLoopFilter);
        if(!ReplaceVariable(newData, DECODER_SKIP_LOOP_FILTER_TAG, skipLoopFilter, newData))
        {
            std::cout << DECODER_SKIP_LOOP_FILTER_TAG << " variable not found. Appended to config.\n";
            newData += std::string{DECODER_SKIP_LOOP_FILTER_TAG} + "=" + skipLoopFilter + ";\n";
        }
    }

    {
        auto threadType = ThreadTypeToString(data.threadType);
        if(!ReplaceVariable(newData, DECODER_THREAD_TYPE_TAG, threadType, newData))
        {
            std::cout << DECODER_THREAD_TYPE_TAG << " variable not found. Appended to config.\n";
            newData += std::string{DECODER_THREAD_TYPE_TAG} + "=" + threadType + ";\n";
        }
    }

    {
        auto threadCount = std::to_string(data.threadCount);
        if(!ReplaceVariable(newData, DECODER_THREAD_COUNT_TAG, threadCount, newData))
        {
            std::cout << DECODER_THREAD_COUNT_TAG << " variable not found. Appended to config.\n";
            newData += std::string{DECODER_THREAD_COUNT_TAG} + "=" + threadCount + ";\n";
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