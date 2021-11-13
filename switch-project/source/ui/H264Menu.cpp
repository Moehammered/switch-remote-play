#include "H264Menu.h"
#include "srp/codec/h264/H264Configuration.h"
#include "../utils/Colours.h"
#include "../system/SoftwareKeyboard.h"

namespace
{
    int modifyParameter(h264::H264Parameters const param, int direction, h264::H264Data const currentData)
    {
        switch (param)
        {
        default:
        case h264::H264Parameters::Preset:
        case h264::H264Parameters::RateControlMode:
        case h264::H264Parameters::Profile:
            return direction;

        case h264::H264Parameters::ConstantRateFactor:
            return keyboardNumber(h264::minCRF, h264::maxCRF, currentData.constantRateFactor);
            break;
        }
    }
}

H264Menu::H264Menu() : Menu(),
textElements{}, codec{}, selected{}
{
    title.value = "H264 Codec - CPU";
    title.y += 30;

    auto config = H264Configuration{};
    codec.Set(config.Data());

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

    auto const modifyH264Param = [=](auto const param, auto direction)
    {
        return modifyParameter(param, direction, codec.Data());
    };

    if(kDown & HidNpadButton::HidNpadButton_A)
        codec.Increase(modifyH264Param);
    else if(kDown & HidNpadButton::HidNpadButton_B)
        codec.Decrease(modifyH264Param);
    
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

h264::H264Data const H264Menu::Settings() const
{
    return codec.Data();
}

void H264Menu::UpdateUI(h264::H264Parameters param)
{
    auto prefix = h264::h264ParamsDesc.at(param);
    auto data = codec.Data();

    switch(param)
    {
        case h264::H264Parameters::ConstantRateFactor:
        {
            auto crfStr = h264::constantRateFactorToDesc(data.constantRateFactor);
            textElements[param].value = prefix + ": " + crfStr;
        }
        break;

        case h264::H264Parameters::Preset:
        {
            auto presetStr = h264::encoderPresetToDesc(data.preset);
            textElements[param].value = prefix + ": " + presetStr;
        }
        break;

        case h264::H264Parameters::RateControlMode:
        {
            auto modeStr = h264::encoderBitrateModeToDesc(data.bitrateMode);
            textElements[param].value = prefix + ": " + modeStr;
        }
        break;

        case h264::H264Parameters::Profile:
        {
            auto profileStr = h264::encoderProfileToDesc(data.profile);
            textElements[param].value = prefix + ": " + profileStr;
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
    
    auto params = h264::h264ParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = colours::white;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}