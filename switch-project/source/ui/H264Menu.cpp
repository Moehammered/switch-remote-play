#include "H264Menu.h"

H264Menu::H264Menu() : Menu(),
textElements{}, codec{}, selected{}
{
    title.value = "H264 Codec - CPU";
    title.y += 30;
    SetupText();
    selected = codec.Current();
}

void H264Menu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        selected = codec.Prev();
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        selected = codec.Next();

    if(kDown & HidNpadButton::HidNpadButton_A)
        codec.Increase();
    else if(kDown & HidNpadButton::HidNpadButton_B)
        codec.Decrease();
    
    UpdateUI(selected);
}

void H264Menu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto const & t : textElements)
    {
        if(t.first == selected)
            t.second.Render(renderer, font, highlightColour);
        else
            t.second.Render(renderer, font);
    }
}

void H264Menu::UpdateUI(h264::Parameters param)
{
    auto prefix = h264::ParamsToDesc.at(param);
    auto data = codec.Data();

    switch(param)
    {
        case h264::Parameters::ConstantRateFactor:
            {
                auto crfStr = h264::ConstantRateFactorToDesc(data.ConstantRateFactor);
                textElements[param].value = prefix + ": " + crfStr;
            }
        break;

        case h264::Parameters::Preset:
            {
                auto presetStr = h264::EncoderPresetToDesc(data.Preset);
                textElements[param].value = prefix + ": " + presetStr;
            }
        break;
    }
}

void H264Menu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = h264::ParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}