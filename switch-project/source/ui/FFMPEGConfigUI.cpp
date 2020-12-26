#include "FFMPEGConfigUI.h"
#include <string>
#include <iostream>

SDL_Color constexpr highlight {.r = 200, .g = 200, .b = 50, .a = 255};
SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};
SDL_Color constexpr backgroundColour {.r = 100, .g = 100, .b = 100, .a = 255};
constexpr int maxCRF = 30;

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
constexpr std::array<const Resolution, 1> videoScaleResolutions = {
    Resolution{1280, 720}/*, Resolution{1980, 720}*/
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

std::string HWAccelToString(HWAccelMode mode)
{
    switch(mode)
    {
        case HWAccelMode::AUTO:
            return "Auto (FFMPEG chooses best option)";
        case HWAccelMode::DXVA2:
            return "DXVA2 (Direct X)";
        case HWAccelMode::VAAPI:
            return "VAAPI (Intel/AMD - unsupported)";
        case HWAccelMode::CUDA:
            return "CUDA (NVENC Transcoder - unsupported)";

        default:
            return "unknown";
    }
}

std::string VideoCodecModeToString(VideoCodecMode mode)
{
    switch(mode)
    {
        case VideoCodecMode::H264:
            return "h264";
        case VideoCodecMode::H264_AMF:
            return "h264 AMF (AMD)";
        case VideoCodecMode::H264_NVENC:
            return "h264 NVENC (NVIDIA)";
        case VideoCodecMode::H264_QSV:
            return "h264 Quick Scan (Intel - requires Intel CPU)";
        default:
            return "unknown";
    }
}

std::string PresetToString(EncoderPreset preset)
{
    switch(preset)
    {
        case EncoderPreset::ULTRAFAST:
            return "ultra fast (bad quality)";
        case EncoderPreset::VERYFAST:
            return "very fast";
        case EncoderPreset::FAST:
            return "fast";
        case EncoderPreset::MEDIUM:
            return "medium (balanced quality)";
        case EncoderPreset::SLOW:
            return "slow";
        case EncoderPreset::VERYSLOW:
            return "very slow (best quality)";

        default:
            return "unknown";
    }
}

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

constexpr const char* vsyncModeToString(const int16_t mode)
{
    switch(mode)
    {
        case -1:
            return "auto (generally bad - ffmpeg decides.)";

        case 0:
            return "passthrough (render from timestamps)";

        case 1:
            return "constant frame rate (force fps - latency)";

        case 2:
            return "variable frame rate";

        case 3:
            return "drop time (render ignoring timestamps)";

        default:
            return "unknown";
    }
}

FFMPEGConfigUI::FFMPEGConfigUI() 
    : settingIndex(0), settingsIndices{}, settingsText{}
{
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

    settingsIndices[FfmpegConfigUiElements::FRAMERATE] = 1;
    settingsIndices[FfmpegConfigUiElements::DESKTOP_RES] = 2;
    settingsIndices[FfmpegConfigUiElements::SWITCH_RES] = 0;
    settingsIndices[FfmpegConfigUiElements::BITRATE] = 9;
    settingsIndices[FfmpegConfigUiElements::VSYNC] = 3;
    settingsIndices[FfmpegConfigUiElements::PRESET] = EncoderPreset::VERYFAST;
    settingsIndices[FfmpegConfigUiElements::CRF] = 16;
    settingsIndices[FfmpegConfigUiElements::CODEC] = VideoCodecMode::H264;
    settingsIndices[FfmpegConfigUiElements::HWACCEL] = HWAccelMode::AUTO;

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

void FFMPEGConfigUI::IncreaseSetting()
{
    auto currInd = settingsIndices[settingIndex];

    switch(settingIndex)
    {
        case FfmpegConfigUiElements::FRAMERATE:
            if(++currInd >= framerates.size())
                currInd = 0;
        break;

        case FfmpegConfigUiElements::DESKTOP_RES:
            if(++currInd >= videoCaptureResolutions.size())
                currInd = 0;
        break;

        case FfmpegConfigUiElements::SWITCH_RES:
            if(++currInd >= videoScaleResolutions.size())
                currInd = 0;
        break;

        case FfmpegConfigUiElements::BITRATE:
            if(++currInd >= bitratesKB.size())
                currInd = 0;
        break;

        case FfmpegConfigUiElements::VSYNC:
            if(++currInd >= vsyncModes.size())
                currInd = 0;
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

void FFMPEGConfigUI::DecreaseSetting()
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
                currInd = videoCaptureResolutions.size() - 1;
        break;

        case FfmpegConfigUiElements::SWITCH_RES:
            if(--currInd < 0)
                currInd = videoScaleResolutions.size() - 1;
        break;

        case FfmpegConfigUiElements::BITRATE:
            if(--currInd < 0)
                currInd = bitratesKB.size() - 1;
        break;

        case FfmpegConfigUiElements::VSYNC:
            if(--currInd < 0)
                currInd = vsyncModes.size() - 1;
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
    auto framerate = framerates[settingsIndices[FfmpegConfigUiElements::FRAMERATE]];
    auto vsMode = vsyncModes[settingsIndices[FfmpegConfigUiElements::VSYNC]];
    auto bitrate = bitratesKB[settingsIndices[FfmpegConfigUiElements::BITRATE]];
    auto videoRes = videoCaptureResolutions[settingsIndices[FfmpegConfigUiElements::DESKTOP_RES]];
    auto scaleRes = videoScaleResolutions[settingsIndices[FfmpegConfigUiElements::SWITCH_RES]];
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

void FFMPEGConfigUI::UpdateFramerate()
{
    const auto frInd = settingsIndices[FfmpegConfigUiElements::FRAMERATE];
    const auto framerate = framerates[frInd];

    settingsText[FfmpegConfigUiElements::FRAMERATE].value = "Desired Framerate:      " + std::to_string(framerate);
}

void FFMPEGConfigUI::UpdateVideoRes()
{
    const auto vrInd = settingsIndices[FfmpegConfigUiElements::DESKTOP_RES];
    const auto res = videoCaptureResolutions[vrInd];

    settingsText[FfmpegConfigUiElements::DESKTOP_RES].value = "Desktop Resolution:     " 
                                        + std::to_string(res.width) 
                                        + "x" + std::to_string(res.height);
}

void FFMPEGConfigUI::UpdateVideoScale()
{
    const auto vrInd = settingsIndices[FfmpegConfigUiElements::SWITCH_RES];
    const auto res = videoScaleResolutions[vrInd];

    settingsText[FfmpegConfigUiElements::SWITCH_RES].value = "Switch Resolution:      "
                                        + std::to_string(res.width) 
                                        + "x" + std::to_string(res.height);
}

void FFMPEGConfigUI::UpdateBitrate()
{
    const auto brInd = settingsIndices[FfmpegConfigUiElements::BITRATE];
    const auto br = bitratesKB[brInd];

    settingsText[FfmpegConfigUiElements::BITRATE].value = "Bitrate(KB/s):          " + std::to_string(br);
}

void FFMPEGConfigUI::UpdateVsync()
{
    const auto vsInd = settingsIndices[FfmpegConfigUiElements::VSYNC];
    const auto mode = vsyncModes[vsInd];
    settingsText[FfmpegConfigUiElements::VSYNC].value = "Vsync Mode:             " + std::string(vsyncModeToString(mode));
}

void FFMPEGConfigUI::UpdatePreset()
{
    const auto preset = (EncoderPreset)settingsIndices[FfmpegConfigUiElements::PRESET];
    settingsText[FfmpegConfigUiElements::PRESET].value = "Encoder Preset:         " + PresetToString(preset);
}

void FFMPEGConfigUI::UpdateCRF()
{
    const auto crf = settingsIndices[FfmpegConfigUiElements::CRF];
    settingsText[FfmpegConfigUiElements::CRF].value = "Quality Control Factor: " + std::to_string(crf) + " " + crfToString(crf);
}

void FFMPEGConfigUI::UpdateCodec()
{
    const auto codec = (VideoCodecMode)settingsIndices[FfmpegConfigUiElements::CODEC];
    settingsText[FfmpegConfigUiElements::CODEC].value = "Video Codec:            " + VideoCodecModeToString(codec);
}

void FFMPEGConfigUI::UpdateHWAccel()
{
    const auto hwaccel = (HWAccelMode)settingsIndices[FfmpegConfigUiElements::HWACCEL];
    settingsText[FfmpegConfigUiElements::HWACCEL].value = "Hardware Accel Mode:    " + HWAccelToString(hwaccel);
}