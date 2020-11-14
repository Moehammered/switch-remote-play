#include "FFMPEGConfigUI.h"
#include <string>

constexpr std::array<const int16_t, 2> FFMPEGConfigUI::framerates;
constexpr std::array<const Resolution, 2> FFMPEGConfigUI::videoCaptureResolutions;
constexpr std::array<const Resolution, 2> FFMPEGConfigUI::videoScaleResolutions;
constexpr std::array<const uint16_t, 10> FFMPEGConfigUI::bitratesKB;
constexpr std::array<const int8_t, 5> FFMPEGConfigUI::vsyncModes;

constexpr const char* vsyncModeToString(const int8_t mode)
{
    switch(mode)
    {
        case -1:
            return "auto";

        case 0:
            return "passthrough";

        case 1:
            return "constant frame rate";

        case 2:
            return "variable frame rate";

        case 3:
            return "drop time (like passthrough)";

        default:
            return "unknown";
    }
}


FFMPEGConfigUI::FFMPEGConfigUI() 
    : settingIndex(0), settingsIndices{}, menu{}
{
    settingsIndices[framerateIndex] = 1;
    settingsIndices[videoResIndex] = 0;
    settingsIndices[videoScaleIndex] = 0;
    settingsIndices[bitrateIndex] = 4;
    settingsIndices[vsyncIndex] = 4;

    constexpr auto defFps = framerates[1];
    constexpr auto defVideoRes = videoCaptureResolutions[0];
    constexpr auto defScale = videoScaleResolutions[0];
    constexpr auto defBitrate = bitratesKB[4];
    constexpr auto defVsync = vsyncModeToString(vsyncModes[4]);

    auto textColour = SDL_Color{.r = 200, .g = 200, .b = 200, .a = 255};
    const int settingTextX = 100;
    const int yOffset = 300;
    const int ySpace = 50;

    for(size_t i = 0; i < settingsCount; ++i)
    {
        Text element{};
        element.colour = textColour;
        element.x = settingTextX;
        element.y = yOffset + ySpace * (i+1);

        switch(i)
        {
            case framerateIndex:
                element.value = "Desired framerate: " + std::to_string(defFps);
            break;

            case videoResIndex:
                element.value = "Video Res: " 
                                + std::to_string(defVideoRes.width) 
                                + "x" + std::to_string(defVideoRes.height);
            break;

            case videoScaleIndex:
                element.value = "Scale Res: "
                                + std::to_string(defScale.width) 
                                + "x" + std::to_string(defScale.height);
            break;

            case bitrateIndex:
                element.value = "Bitrate(KB/s): " + std::to_string(defBitrate);
            break;

            case vsyncIndex:
                element.value = "Vsync Mode: " + std::string(defVsync);
            break;
        }
        menu.AddElement(element);
    }

    menu.OnAccept([&] (int index, Text& element) {
        UpdateSetting(index, element, 1);
    });

    menu.OnCancel([&] (int index, Text& element) {
        UpdateSetting(index, element, -1);
    });
}

void FFMPEGConfigUI::IncreaseSetting()
{
    menu.Accept();
}

void FFMPEGConfigUI::DecreaseSetting()
{
    menu.Cancel();
}

void FFMPEGConfigUI::SelectNext()
{
    menu.Next();
}

void FFMPEGConfigUI::SelectPrevious()
{
    menu.Prev();
}

void FFMPEGConfigUI::Render(SDL_Renderer* renderer, FC_Font* font)
{
    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    menu.Render(renderer, font);
}

FFMPEG_Config const FFMPEGConfigUI::Settings()
{
    auto framerate = framerates[settingsIndices[framerateIndex]];
    auto vsMode = vsyncModes[settingsIndices[vsyncIndex]];
    auto bitrate = bitratesKB[settingsIndices[bitrateIndex]];
    auto videoRes = videoCaptureResolutions[settingsIndices[videoResIndex]];
    auto scaleRes = videoScaleResolutions[settingsIndices[videoScaleIndex]];

    return FFMPEG_Config{
        .desiredFrameRate = framerate,
        .videoX = videoRes.width, .videoY = videoRes.height,
        .scaleX = scaleRes.width, .scaleY = scaleRes.height,
        .bitrateKB = bitrate,
        .vsyncMode = vsMode,
    };
}

void FFMPEGConfigUI::UpdateSetting(int index, Text& element, int direction)
{
    auto currInd = settingsIndices[index];
    
    switch(index)
    {
        case framerateIndex:
        {
            if(direction <= 0 && currInd == 0)
                currInd = framerates.size()-1;
            else
                currInd += direction;

            if(currInd >= framerates.size())
                currInd = 0;
            
            const auto setting = framerates[currInd];

            element.value = "Desired framerate: " + std::to_string(setting);
            settingsIndices[index] = currInd;
        }
        break;

        case videoResIndex:
        {
            if(direction <= 0 && currInd == 0)
                currInd = videoCaptureResolutions.size()-1;
            else
                currInd += direction;

            if(currInd >= videoCaptureResolutions.size())
                currInd = 0;

            const auto setting = videoCaptureResolutions[currInd];

            element.value = "Video Res: " 
                            + std::to_string(setting.width) 
                            + "x" + std::to_string(setting.height);
            settingsIndices[index] = currInd;
        }
        break;

        case videoScaleIndex:
        {
            if(direction <= 0 && currInd == 0)
                currInd = videoScaleResolutions.size()-1;
            else
                currInd += direction;

            if(currInd >= videoScaleResolutions.size())
                currInd = 0;

            const auto setting = videoScaleResolutions[currInd];

            element.value = "Scale Res: " 
                            + std::to_string(setting.width) 
                            + "x" + std::to_string(setting.height);
            settingsIndices[index] = currInd;
        }
        break;

        case bitrateIndex:
        {
            if(direction <= 0 && currInd == 0)
                currInd = bitratesKB.size()-1;
            else
                currInd += direction;

            if(currInd >= bitratesKB.size())
                currInd = 0;

            const auto setting = bitratesKB[currInd];

            element.value = "Bitrate(KB/s): " + std::to_string(setting);
            settingsIndices[index] = currInd;
        }
        break;

        case vsyncIndex:
        {
            if(direction <= 0 && currInd == 0)
                currInd = vsyncModes.size()-1;
            else
                currInd += direction;

            if(currInd >= vsyncModes.size())
                currInd = 0;

            const auto setting = vsyncModes[currInd];

            element.value = "Vsync Mode: " + std::string(vsyncModeToString(setting));
            settingsIndices[index] = currInd;
        }
        break;
    }
}
