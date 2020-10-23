#ifndef __FFMPEGCONFIGUI_H__
#define __FFMPEGCONFIGUI_H__

#include <array>
#include <SDL2/SDL_render.h>
#include "Text.h"
#include "NetworkData.h"

struct Resolution
{
    int16_t width;
    int16_t height;
};

constexpr uint16_t mbToKb(double mbs)
{
    return mbs * 1024;
}

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
        static constexpr int framerateIndex = 0;
        static constexpr int videoResIndex = 1;
        static constexpr int videoScaleIndex = 2;
        static constexpr int bitrateIndex = 3;
        static constexpr int vsyncIndex = 4;
        int settingIndex;

        static constexpr int settingsCount = 5;
        std::array<int, settingsCount> settingsIndices;

        std::array<Text, settingsCount> settingsText;

        static constexpr std::array<const int16_t, 2> framerates = { 30, 60 };
        static constexpr std::array<const Resolution, 2> videoCaptureResolutions = {
            1920, 1080, 1280, 720
        };
        static constexpr std::array<const Resolution, 2> videoScaleResolutions = {
            1280, 720, 1980, 720
        };
        static constexpr std::array<const uint16_t, 10> bitratesKB = {
            mbToKb(3), mbToKb(4), mbToKb(4.5), mbToKb(5), mbToKb(5.5), 
            mbToKb(6), mbToKb(6.5), mbToKb(7), mbToKb(7.5), mbToKb(8)
        };
        static constexpr std::array<const int8_t, 5> vsyncModes = {
            -1, 0, 1, 2, 3
        };

        void UpdateFramerate();
        void UpdateVideoRes();
        void UpdateVideoScale();
        void UpdateBitrate();
        void UpdateVsync();
};

#endif
