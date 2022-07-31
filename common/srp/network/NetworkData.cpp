#include "NetworkData.h"
#include <iostream>

namespace
{
    auto const getPropertyName = [](auto const paramType, auto const& paramMap)
    {
        auto const entry = paramMap.find(paramType);
        if(entry != paramMap.end())
            return entry->second;
        else
            return std::string{"Unknown Prop"};
    };

    auto const printPropertyInfo = [](auto const& data, auto toStr, auto const& paramMap, auto linePrefix)
    {
        auto const propertyInfo = toStr(data);
        for(auto const& property : propertyInfo)
        {
            auto const propertyName = getPropertyName(property.first, paramMap);
            auto const& propertyValue = property.second;
            std::cout << linePrefix << propertyName << ": " << propertyValue << "\n";
        } 
    };

    void printEncoderConfig(EncoderConfig const& encoderData)
    {
        using namespace std;

        cout << "--- Encoder Config Data ---\n";
        cout << "  + Video Data +\n";
        printPropertyInfo(encoderData.commonSettings, codec::videoParamsToStr, codec::videoParamToDesc, "   ");

        switch(encoderData.commonSettings.videoCodec)
        {
            case ffmpeg::VideoCodec::H264:
            {
                cout << "      # h264 Data #\n";
                printPropertyInfo(encoderData.cpuSettings, h264::codecParamsToStr, h264::h264ParamsDesc, "      ");
            }
            break;

            case ffmpeg::VideoCodec::H264_AMF:
            {
                cout << "      # h264 AMF Data #\n";
                printPropertyInfo(encoderData.amdSettings, h264amf::codecParamsToStr, h264amf::h264AmfParamsDesc, "      ");
            }
            break;

            default:
            case ffmpeg::VideoCodec::H264_NVENC:
            case ffmpeg::VideoCodec::H264_QSV:
            {
                cout << "      Unsupported codec type\n";
            }
            break;
        }

        cout << "\n\n";
    }

    void printControllerConfig(controller::ControllerConfig const& controllerData)
    {
        std::cout << "--- Controller Config Data ---\n";
        printPropertyInfo(controllerData, controller::controllerParamsToStr, controller::controllerParamsDesc, "   ");
        std::cout << "\n\n";
    }

    void printMouseConfig(mouse::MouseConfig const & mouseData)
    {
        std::cout << "--- Mouse Config Data ---\n";
        printPropertyInfo(mouseData, mouse::mouseParamsToStr, mouse::mouseParamsDesc, "   ");
        std::cout << "\n\n";
    }

    void printKeyboardConfig(keyboard::KeyboardConfig const& keyboardData)
    {
        std::cout << "--- Keyboard Config Data ---\n";
        printPropertyInfo(keyboardData, keyboard::keybindingsToString, keyboard::keyParamsDesc, "   ");
        std::cout << "\n\n";
    }

    void printTouchConfig(touch::TouchConfig const& touchData)
    {
        std::cout << "--- Touch Config Data ---\n";
        printPropertyInfo(touchData, touch::touchConfigParamsToStr, touch::touchConfigParamsDesc, "   ");
        switch(touchData.touchMode)
        {
            case touch::TouchScreenMode::SimulatedMouse:
            {
                std::cout << "   + Simulated Mouse Data +\n";
                printPropertyInfo(touchData.simulatedTouchMouseSettings, touch::simulatedTouchParamsToStr, touch::simulatedTouchParamsDesc, "      ");
            }
            break;

            case touch::TouchScreenMode::VirtualTouch:
            {
                std::cout << "   + Virtual Touch Data +\n";
                printPropertyInfo(touchData.virtualTouchSettings, touch::virtualTouchParamsToStr, touch::virtualTouchParamsDesc, "      ");
            }
            break;

            default:
            {
                std::cout << "      Unsupported touch mode\n";
            }
            break;
        }
        std::cout << "\n\n";
    }
}

void printCommandPayload(CommandPayload const& payload)
{
    printEncoderConfig(payload.encoderData);
    printControllerConfig(payload.controllerData);
    printMouseConfig(payload.mouseData);
    printKeyboardConfig(payload.keyboardData);
    printTouchConfig(payload.touchData);
}