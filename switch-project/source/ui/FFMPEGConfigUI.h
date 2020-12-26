#ifndef __FFMPEGCONFIGUI_H__
#define __FFMPEGCONFIGUI_H__

#include <array>
#include <SDL2/SDL_render.h>
#include "Text.h"
#include "../network/NetworkData.h"

struct Resolution
{
    int16_t width;
    int16_t height;
};

enum FfmpegConfigUiElements : int32_t
{
    FRAMERATE = 0,
    DESKTOP_RES,
    SWITCH_RES,
    BITRATE,
    VSYNC,
    CRF,
    HWACCEL,
    PRESET,
    CODEC,
    CONFIG_COUNT
};

class FFMPEGConfigUI
{
    public:
        FFMPEGConfigUI();
        void IncreaseSetting();
        void DecreaseSetting();
        void SelectNext();
        void SelectPrevious();
        void Render(SDL_Renderer* renderer, FC_Font* font);
        FFMPEG_Config const Settings();

    private:
        int settingIndex;

        std::array<int, FfmpegConfigUiElements::CONFIG_COUNT> settingsIndices;

        std::array<Text, FfmpegConfigUiElements::CONFIG_COUNT> settingsText;

        void UpdateFramerate();
        void UpdateVideoRes();
        void UpdateVideoScale();
        void UpdateBitrate();
        void UpdateVsync();
        void UpdatePreset();
        void UpdateCRF();
        void UpdateCodec();
        void UpdateHWAccel();
};

#endif
