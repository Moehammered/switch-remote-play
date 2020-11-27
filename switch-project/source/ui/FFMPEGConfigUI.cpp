#include "FFMPEGConfigUI.h"
#include <string>

constexpr uint16_t mbToKb(double mbs)
{
    return mbs * 1024;
}

constexpr std::array<const int16_t, 2> framerates = { 30, 60 };
constexpr std::array<const Resolution, 7> videoCaptureResolutions = {
    Resolution{1024, 576}, Resolution{1152, 648}, 
    Resolution{1280, 720}, Resolution{1366, 768},
    Resolution{1600, 900}, Resolution{1920, 1080},
    Resolution{2560, 1440}
};
constexpr std::array<const Resolution, 2> videoScaleResolutions = {
    Resolution{1280, 720}, Resolution{1980, 720}
};
constexpr std::array<const uint16_t, 20> bitratesKB = {
    mbToKb(3), mbToKb(4), mbToKb(4.5), mbToKb(5), mbToKb(5.5), 
    mbToKb(6), mbToKb(6.5), mbToKb(7), mbToKb(7.5), mbToKb(8),
    mbToKb(8.5), mbToKb(9), mbToKb(9.5), mbToKb(10), mbToKb(11),
    mbToKb(12), mbToKb(13), mbToKb(14), mbToKb(15), mbToKb(16)
};
constexpr std::array<const int16_t, 5> vsyncModes = {
    -1, 0, 1, 2, 3
};

constexpr const char* vsyncModeToString(const int16_t mode)
{
    switch(mode)
    {
        case -1:
            return "auto (generally bad, picks what FFMPEG deems OK)";

        case 0:
            return "passthrough (render as it comes, matching timestamps)";

        case 1:
            return "constant frame rate (force desired fps = latency)";

        case 2:
            return "variable frame rate";

        case 3:
            return "drop time (like passthrough, ignores packet timestamps)";

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

    settingsIndices[framerateIndex] = 1;
    settingsIndices[videoResIndex] = 2;
    settingsIndices[videoScaleIndex] = 0;
    settingsIndices[bitrateIndex] = 4;
    settingsIndices[vsyncIndex] = 4;

    auto defFps = framerates[settingsIndices[framerateIndex]];
    auto defVideoRes = videoCaptureResolutions[settingsIndices[videoResIndex]];
    auto defScale = videoScaleResolutions[settingsIndices[videoScaleIndex]];
    auto defBitrate = bitratesKB[settingsIndices[bitrateIndex]];
    auto defVsync = vsyncModeToString(vsyncModes[settingsIndices[vsyncIndex]]);

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