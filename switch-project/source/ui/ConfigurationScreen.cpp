#include "ConfigurationScreen.h"
#include <string>
#include <iostream>
#include "../system/Configuration.h"
#include "../dataHelpers/Resolution.h"

SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};
SDL_Color constexpr backgroundColour {.r = 100, .g = 100, .b = 100, .a = 255};
constexpr int maxCRF = 30;
constexpr int maxMouseSensitivity = 30;
constexpr int minMouseSensitivity = 3;

constexpr uint16_t mbToKb(double mbs)
{
    return mbs * 1024;
}

constexpr std::array<const int16_t, 2> framerates = { 30, 60 };
constexpr std::array<const Resolution, 7> desktopResolutions = {
    Resolution{1024, 576}, Resolution{1152, 648}, 
    Resolution{1280, 720}, Resolution{1366, 768},
    Resolution{1600, 900}, Resolution{1920, 1080},
    Resolution{2560, 1440}
};
constexpr std::array<const Resolution, 1> switchResolutions = {
    Resolution{1280, 720}/*, Resolution{1980, 720}*/
};
constexpr std::array<const uint16_t, 20> bitratesKB = {
    mbToKb(3), mbToKb(4), mbToKb(4.5), mbToKb(5), mbToKb(5.5), 
    mbToKb(6), mbToKb(6.5), mbToKb(7), mbToKb(7.5), mbToKb(8),
    mbToKb(8.5), mbToKb(9), mbToKb(9.5), mbToKb(10), mbToKb(11),
    mbToKb(12), mbToKb(13), mbToKb(14), mbToKb(15), mbToKb(16)
};

const char* crfToString(int16_t value)
{
    if(value == 0)
        return "Lossless Quality";
    else if(value < 12)
        return "Good Quality";
    else if(value < 23)
        return "Average Quality";
    else
        return "Bad Quality";
}

ConfigurationScreen::ConfigurationScreen() 
    : Menu(), settingIndex(0), settingsIndices{}, settingsText{}
{
    title.value = "Configuration Settings";
    const int settingTextX = 100;
    const int yOffset = 200;
    const int ySpace = 45;
    int counter = 1;
    for(auto &t : settingsText)
    {
        t.colour = textColour;
        t.x = settingTextX;
        t.y = yOffset + ySpace * counter++;
    }

    auto config = Configuration{};
    auto ffmpeg = config.FFMPEGData();

    auto fpsInd = 1;
    for(auto i = 0U; i < framerates.size(); ++i)
    {
        if(ffmpeg.desiredFrameRate == framerates[i])
        {
            fpsInd = i;
            break;
        }
    }
    settingsIndices[FfmpegConfigUiElements::FRAMERATE] = fpsInd;

    auto desktopResInd = 2;
    for(auto i = 0U; i < desktopResolutions.size(); ++i)
    {
        auto res = desktopResolutions[i];
        if(res.width == ffmpeg.videoX && res.height == ffmpeg.videoY)
        {
            desktopResInd = i;
            break;
        }
    }
    settingsIndices[FfmpegConfigUiElements::DESKTOP_RES] = desktopResInd;

    auto switchResInd = 0;
    for(auto i = 0U; i < switchResolutions.size(); ++i)
    {
        auto res = switchResolutions[i];
        if(res.width == ffmpeg.scaleX && res.height == ffmpeg.scaleY)
        {
            switchResInd = i;
            break;
        }
    }
    settingsIndices[FfmpegConfigUiElements::SWITCH_RES] = switchResInd;

    auto bitrateInd = 9;
    for(auto i = 0U; i < bitratesKB.size(); ++i)
    {
        if(bitratesKB[i] == ffmpeg.bitrateKB)
        {
            bitrateInd = i;
            break;
        }
    }
    settingsIndices[FfmpegConfigUiElements::BITRATE] = bitrateInd;

    settingsIndices[FfmpegConfigUiElements::VSYNC] = ffmpeg.vsyncMode;
    settingsIndices[FfmpegConfigUiElements::PRESET] = ffmpeg.preset;
    settingsIndices[FfmpegConfigUiElements::CRF] = ffmpeg.constantRateFactor;
    settingsIndices[FfmpegConfigUiElements::CODEC] = ffmpeg.videoCodecMode;
    settingsIndices[FfmpegConfigUiElements::HWACCEL] = ffmpeg.hwaccelMode;

    UpdateFramerate();
    UpdateVideoRes();
    UpdateVideoScale();
    UpdateBitrate();
    UpdateVsync();
    UpdatePreset();
    UpdateCRF();
    UpdateCodec();
    UpdateHWAccel();
}

void ConfigurationScreen::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & KEY_DUP)
        SelectPrevious();
    else if(kDown & KEY_DDOWN)
        SelectNext();

    if(kDown & KEY_A)
        IncreaseSetting();
    else if(kDown & KEY_B)
        DecreaseSetting();
}

void ConfigurationScreen::IncreaseSetting()
{
    auto currInd = settingsIndices[settingIndex];

    switch(settingIndex)
    {
        case FfmpegConfigUiElements::FRAMERATE:
            if(++currInd >= framerates.size())
                currInd = 0;
        break;

        case FfmpegConfigUiElements::DESKTOP_RES:
            if(++currInd >= desktopResolutions.size())
                currInd = 0;
        break;

        case FfmpegConfigUiElements::SWITCH_RES:
            if(++currInd >= switchResolutions.size())
                currInd = 0;
        break;

        case FfmpegConfigUiElements::BITRATE:
            if(++currInd >= bitratesKB.size())
                currInd = 0;
        break;

        case FfmpegConfigUiElements::VSYNC:
            if(++currInd >= VsyncMode::VSYNC_MODE_COUNT)
                currInd = VsyncMode::VSYNC_AUTO;
        break;

        case FfmpegConfigUiElements::PRESET:
            if(++currInd >= ENCODER_PRESET_COUNT)
                currInd = 0;
        break;

        case FfmpegConfigUiElements::CRF:
            if(++currInd > maxCRF)
                currInd = 0;
        break;

        case FfmpegConfigUiElements::CODEC:
            if(++currInd >= VideoCodecMode::VIDEOCODECMODE_COUNT)
                currInd = 0;
        break;

        case FfmpegConfigUiElements::HWACCEL:
            if(++currInd >= HWAccelMode::CUDA)
                currInd = 0;
        break;
    }

    settingsIndices[settingIndex] = currInd;

    UpdateFramerate();
    UpdateVideoRes();
    UpdateVideoScale();
    UpdateBitrate();
    UpdateVsync();
    UpdatePreset();
    UpdateCRF();
    UpdateCodec();
    UpdateHWAccel();
}

void ConfigurationScreen::DecreaseSetting()
{
    auto currInd = settingsIndices[settingIndex];

    switch(settingIndex)
    {
        case FfmpegConfigUiElements::FRAMERATE:
            if(--currInd < 0)
                currInd = framerates.size() - 1;
        break;

        case FfmpegConfigUiElements::DESKTOP_RES:
            if(--currInd < 0)
                currInd = desktopResolutions.size() - 1;
        break;

        case FfmpegConfigUiElements::SWITCH_RES:
            if(--currInd < 0)
                currInd = switchResolutions.size() - 1;
        break;

        case FfmpegConfigUiElements::BITRATE:
            if(--currInd < 0)
                currInd = bitratesKB.size() - 1;
        break;

        case FfmpegConfigUiElements::VSYNC:
            if(--currInd < 0)
                currInd = VsyncMode::VSYNC_MODE_COUNT - 1;
        break;

        case FfmpegConfigUiElements::PRESET:
            if(--currInd < 0)
                currInd = EncoderPreset::ENCODER_PRESET_COUNT - 1;
        break;

        case FfmpegConfigUiElements::CRF:
            if(--currInd < 0)
                currInd = maxCRF;
        break;

        case FfmpegConfigUiElements::CODEC:
            if(--currInd < 0)
                currInd = VideoCodecMode::VIDEOCODECMODE_COUNT - 1;
        break;

        case FfmpegConfigUiElements::HWACCEL:
            if(--currInd < 0)
                currInd = HWAccelMode::CUDA - 1;
        break;
    }

    settingsIndices[settingIndex] = currInd;

    UpdateFramerate();
    UpdateVideoRes();
    UpdateVideoScale();
    UpdateBitrate();
    UpdateVsync();
    UpdatePreset();
    UpdateCRF();
    UpdateCodec();
    UpdateHWAccel();
}

void ConfigurationScreen::SelectNext()
{
    ++settingIndex;
    if(settingIndex >= settingsIndices.size())
        settingIndex = 0;
}

void ConfigurationScreen::SelectPrevious()
{
    --settingIndex;
    if(settingIndex < 0)
        settingIndex = settingsIndices.size() - 1;
}

void ConfigurationScreen::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto i = 0; i < settingsText.size(); ++i)
    {
        if(i != settingIndex)
            settingsText[i].Render(renderer, font);
        else
            settingsText[i].Render(renderer, font, highlightColour);
    }
}

FFMPEG_Config const ConfigurationScreen::Settings() const
{
    auto framerate = framerates[settingsIndices[FfmpegConfigUiElements::FRAMERATE]];
    auto bitrate = bitratesKB[settingsIndices[FfmpegConfigUiElements::BITRATE]];
    auto videoRes = desktopResolutions[settingsIndices[FfmpegConfigUiElements::DESKTOP_RES]];
    auto scaleRes = switchResolutions[settingsIndices[FfmpegConfigUiElements::SWITCH_RES]];
    auto vsMode = (VsyncMode)settingsIndices[FfmpegConfigUiElements::VSYNC];
    auto crf = settingsIndices[FfmpegConfigUiElements::CRF];
    auto preset = (EncoderPreset)settingsIndices[FfmpegConfigUiElements::PRESET];
    auto hwaccel = (HWAccelMode)settingsIndices[FfmpegConfigUiElements::HWACCEL];
    auto videoCodecMode = (VideoCodecMode)settingsIndices[FfmpegConfigUiElements::CODEC];

    return FFMPEG_Config{
        .desiredFrameRate = framerate,
        .videoX = videoRes.width, .videoY = videoRes.height,
        .scaleX = scaleRes.width, .scaleY = scaleRes.height,
        .bitrateKB = bitrate,
        .vsyncMode = vsMode,
        .constantRateFactor = crf,
        .preset = preset,
        .hwaccelMode = hwaccel,
        .videoCodecMode = videoCodecMode
    };
}

void ConfigurationScreen::UpdateFramerate()
{
    const auto frInd = settingsIndices[FfmpegConfigUiElements::FRAMERATE];
    const auto framerate = framerates[frInd];

    settingsText[FfmpegConfigUiElements::FRAMERATE].value = "Desired Framerate:      " + std::to_string(framerate);
}

void ConfigurationScreen::UpdateVideoRes()
{
    const auto vrInd = settingsIndices[FfmpegConfigUiElements::DESKTOP_RES];
    const auto res = desktopResolutions[vrInd];

    settingsText[FfmpegConfigUiElements::DESKTOP_RES].value = "Desktop Resolution:     " + ResolutionToString(res);
}

void ConfigurationScreen::UpdateVideoScale()
{
    const auto vrInd = settingsIndices[FfmpegConfigUiElements::SWITCH_RES];
    const auto res = switchResolutions[vrInd];

    settingsText[FfmpegConfigUiElements::SWITCH_RES].value = "Switch Resolution:      " + ResolutionToString(res);
}

void ConfigurationScreen::UpdateBitrate()
{
    const auto brInd = settingsIndices[FfmpegConfigUiElements::BITRATE];
    const auto br = bitratesKB[brInd];

    settingsText[FfmpegConfigUiElements::BITRATE].value = "Bitrate(KB/s):          " + std::to_string(br);
}

void ConfigurationScreen::UpdateVsync()
{
    const auto mode = (VsyncMode)settingsIndices[FfmpegConfigUiElements::VSYNC];
    settingsText[FfmpegConfigUiElements::VSYNC].value = "Vsync Mode:             " + VsyncModeDescription(mode);
}

void ConfigurationScreen::UpdatePreset()
{
    const auto preset = (EncoderPreset)settingsIndices[FfmpegConfigUiElements::PRESET];
    settingsText[FfmpegConfigUiElements::PRESET].value = "Encoder Preset:         " + EncoderPresetDescription(preset);
}

void ConfigurationScreen::UpdateCRF()
{
    const auto crf = settingsIndices[FfmpegConfigUiElements::CRF];
    settingsText[FfmpegConfigUiElements::CRF].value = "Quality Control Factor: " + std::to_string(crf) + " " + crfToString(crf);
}

void ConfigurationScreen::UpdateCodec()
{
    const auto codec = (VideoCodecMode)settingsIndices[FfmpegConfigUiElements::CODEC];
    settingsText[FfmpegConfigUiElements::CODEC].value = "Video Codec:            " + VideoCodecModeDescription(codec);
}

void ConfigurationScreen::UpdateHWAccel()
{
    const auto hwaccel = (HWAccelMode)settingsIndices[FfmpegConfigUiElements::HWACCEL];
    settingsText[FfmpegConfigUiElements::HWACCEL].value = "Hardware Accel Mode:    " + HWAccelDescription(hwaccel);
}
