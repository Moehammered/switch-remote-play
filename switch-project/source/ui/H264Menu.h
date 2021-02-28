#ifndef __H264MENU_H__
#define __H264MENU_H__

#include "Menu.h"
#include <array>
#include "../codec/H264Options.h"
#include "../dataHelpers/Circulator.h"

class H264Menu : public Menu
{
    public:
        H264Menu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    private:
        Circulator<h264::Parameters, std::vector<h264::Parameters>> ParamSelection;
        Circulator<std::pair<h264::EncoderPreset, std::string>, 
                    std::unordered_map<h264::EncoderPreset, std::string>>  PresetSelection;
        int32_t constantRateFactor;

        std::array<Text, h264::Parameters::ParamCount> textElements;

        void IncreaseSetting();
        void DecreaseSetting();

        void UpdateUI(h264::Parameters param);
};

#endif
