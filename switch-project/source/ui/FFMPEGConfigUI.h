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

static constexpr int framerateIndex = 0;
static constexpr int videoResIndex = 1;
static constexpr int videoScaleIndex = 2;
static constexpr int bitrateIndex = 3;
static constexpr int vsyncIndex = 4;
static constexpr int settingsCount = 5;
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
    
        SDL_Color highlight;
        SDL_Color textColour;
        SDL_Color backgroundColour;

    private:
        int settingIndex;

        std::array<int, settingsCount> settingsIndices;

        std::array<Text, settingsCount> settingsText;

        void UpdateFramerate();
        void UpdateVideoRes();
        void UpdateVideoScale();
        void UpdateBitrate();
        void UpdateVsync();
};

#endif
