#include "H264Menu.h"

H264Menu::H264Menu() : Menu(),
ParamSelection{h264::ParamsList},
PresetSelection{h264::EncoderPresetDescMap},
constantRateFactor{h264::DefaultCRF},
textElements{}
{
    title.value = "H264 Codec - CPU";
    const int settingTextX = 100;
    const int yOffset = 200;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};
    for(auto &t : textElements)
    {
        t.colour = textColour;
        t.x = settingTextX;
        t.y = yOffset + ySpace * counter++;
    }
}

void H264Menu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        ParamSelection++;
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        ParamSelection--;

    if(kDown & HidNpadButton::HidNpadButton_A)
        IncreaseSetting();
    else if(kDown & HidNpadButton::HidNpadButton_B)
        DecreaseSetting();
    
    UpdateUI(*ParamSelection);
}

void H264Menu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    auto selected = ParamSelection.Value();

    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto i = 0; i < textElements.size(); ++i)
    {
        if(i != (int)selected)
            textElements[i].Render(renderer, font);
        else
            textElements[i].Render(renderer, font, highlightColour);
    }
}

void H264Menu::IncreaseSetting()
{
    auto choice = *ParamSelection;
    switch(choice)
    {
        case h264::Parameters::ConstantRateFactor:
            {
                if(++constantRateFactor > h264::MaxCRF)
                    constantRateFactor = h264::MinCRF;
            }
        break;

        case h264::Parameters::Preset:
            {
                PresetSelection++;
            }
        break;
    }
}

void H264Menu::DecreaseSetting()
{
    auto choice = *ParamSelection;
    switch(choice)
    {
        case h264::Parameters::ConstantRateFactor:
            {
                if(--constantRateFactor < h264::MinCRF)
                    constantRateFactor = h264::MaxCRF;
            }
        break;

        case h264::Parameters::Preset:
            {
                PresetSelection--;
            }
        break;
    }
}

void H264Menu::UpdateUI(h264::Parameters param)
{
    auto prefix = h264::ParamsToDesc.at(param);
    switch(param)
    {
        case h264::Parameters::ConstantRateFactor:
            {
                auto crfStr = h264::ConstantRateFactorToDesc(constantRateFactor);
                textElements[(int)param].value = prefix + ": " + crfStr;
            }
        break;

        case h264::Parameters::Preset:
            {
                auto preset = *PresetSelection;
                auto presetStr = h264::EncoderPresetToDesc(preset.first);
                textElements[(int)param].value = prefix + ": " + presetStr;
            }
        break;
    }
}