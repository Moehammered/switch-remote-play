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


FFMPEGConfigUI::FFMPEGConfigUI() : 
highlight {.r = 200, .g = 200, .b = 50, .a = 255}, 
textColour {.r = 200, .g = 200, .b = 200, .a = 255},
backgroundColour {.r = 100, .g = 100, .b = 100, .a = 255},
settingIndex(0), settingsIndices{}, settingsText{}
{
    const int settingTextX = 100;
    const int yOffset = 300;
    const int ySpace = 50;
    int counter = 1;
    for(auto &t : settingsText)
    {
        t.colour = textColour;
        t.x = settingTextX;
        t.y = yOffset + ySpace * counter++;
    }

    constexpr auto defFps = framerates[0];
    constexpr auto defVideoRes = videoCaptureResolutions[0];
    constexpr auto defScale = videoScaleResolutions[0];
    constexpr auto defBitrate = bitratesKB[0];
    constexpr auto defVsync = vsyncModeToString(vsyncModes[0]);

    settingsText[framerateIndex].value = "Desired framerate: " + std::to_string(defFps);
    settingsText[videoResIndex].value = "Video Res: " 
                                        + std::to_string(defVideoRes.width) 
                                        + "x" + std::to_string(defVideoRes.height);
    settingsText[videoScaleIndex].value = "Scale Res: "
                                        + std::to_string(defScale.width) 
                                        + "x" + std::to_string(defScale.height);
    settingsText[bitrateIndex].value = "Bitrate(KB/s): " + std::to_string(defBitrate);
    settingsText[vsyncIndex].value = "Vsync Mode: " + std::string(defVsync);
}

void FFMPEGConfigUI::IncreaseSetting()
{
    auto currInd = settingsIndices[settingIndex];

    switch(settingIndex)
    {
        case framerateIndex:
            if(++currInd >= framerates.size())
                currInd = 0;
        break;

        case videoResIndex:
            if(++currInd >= videoCaptureResolutions.size())
                currInd = 0;
        break;

        case videoScaleIndex:
            if(++currInd >= videoScaleResolutions.size())
                currInd = 0;
        break;

        case bitrateIndex:
            if(++currInd >= bitratesKB.size())
                currInd = 0;
        break;

        case vsyncIndex:
            if(++currInd >= vsyncModes.size())
                currInd = 0;
        break;
    }

    settingsIndices[settingIndex] = currInd;

    UpdateFramerate();
    UpdateVideoRes();
    UpdateVideoScale();
    UpdateBitrate();
    UpdateVsync();
}

void FFMPEGConfigUI::DecreaseSetting()
{
    auto currInd = settingsIndices[settingIndex];

    switch(settingIndex)
    {
        case framerateIndex:
            if(--currInd < 0)
                currInd = framerates.size() - 1;
        break;

        case videoResIndex:
            if(--currInd < 0)
                currInd = videoCaptureResolutions.size() - 1;
        break;

        case videoScaleIndex:
            if(--currInd < 0)
                currInd = videoScaleResolutions.size() - 1;
        break;

        case bitrateIndex:
            if(--currInd < 0)
                currInd = bitratesKB.size() - 1;
        break;

        case vsyncIndex:
            if(--currInd < 0)
                currInd = vsyncModes.size() - 1;
        break;
    }

    settingsIndices[settingIndex] = currInd;

    UpdateFramerate();
    UpdateVideoRes();
    UpdateVideoScale();
    UpdateBitrate();
    UpdateVsync();
}

void FFMPEGConfigUI::SelectNext()
{
    ++settingIndex;
    if(settingIndex >= settingsIndices.size())
        settingIndex = 0;
}

void FFMPEGConfigUI::SelectPrevious()
{
    --settingIndex;
    if(settingIndex < 0)
        settingIndex = settingsIndices.size() - 1;
}

void FFMPEGConfigUI::Render(SDL_Renderer* renderer, FC_Font* font)
{
    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto i = 0; i < settingsText.size(); ++i)
    {
        if(i != settingIndex)
            settingsText[i].Render(renderer, font);
        else
            settingsText[i].Render(renderer, font, highlight);
    }
}

void FFMPEGConfigUI::UpdateFramerate()
{
    const auto frInd = settingsIndices[framerateIndex];
    const auto framerate = framerates[frInd];

    settingsText[framerateIndex].value = "Desired framerate: " + std::to_string(framerate);
}

void FFMPEGConfigUI::UpdateVideoRes()
{
    const auto vrInd = settingsIndices[videoResIndex];
    const auto res = videoCaptureResolutions[vrInd];

    settingsText[videoResIndex].value = "Video Res: " 
                                        + std::to_string(res.width) 
                                        + "x" + std::to_string(res.height);
}

void FFMPEGConfigUI::UpdateVideoScale()
{
    const auto vrInd = settingsIndices[videoScaleIndex];
    const auto res = videoScaleResolutions[vrInd];

    settingsText[videoScaleIndex].value = "Scale Res: "
                                        + std::to_string(res.width) 
                                        + "x" + std::to_string(res.height);
}

void FFMPEGConfigUI::UpdateBitrate()
{
    const auto brInd = settingsIndices[bitrateIndex];
    const auto br = bitratesKB[brInd];

    settingsText[bitrateIndex].value = "Bitrate(KB/s): " + std::to_string(br);
}

void FFMPEGConfigUI::UpdateVsync()
{
    const auto vsInd = settingsIndices[vsyncIndex];
    const auto mode = vsyncModes[vsInd];
    settingsText[vsyncIndex].value = "Vsync Mode: " + std::string(vsyncModeToString(mode));
}