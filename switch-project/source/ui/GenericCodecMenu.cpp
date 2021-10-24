#include "GenericCodecMenu.h"
#include "../codec/general/GenericCodecConfiguration.h"

GenericCodecMenu::GenericCodecMenu() : Menu(),
textElements{}, codec{}, selected{}
{
    title.value = "General Codec Options";
    title.y += 30;
    selected = codec.Current();

    auto config = GenericCodecConfiguration{};
    codec.Set(config.Data());
    
    SetupText();
}

void GenericCodecMenu::ProcessInput(PadState const & pad)
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

void GenericCodecMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
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

codec::VideoData const GenericCodecMenu::VideoCodecSettings() const
{
    return codec.Data();
}

void GenericCodecMenu::UpdateUI(codec::VideoParameters param)
{
    auto prefix = codec::videoParamToDesc.at(param);
    auto data = codec.Data();

    switch(param)
    {
        case codec::VideoParameters::DesktopResolution:
        {
            auto str = resolutionToString(data.desktopResolution);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case codec::VideoParameters::SwitchResolution:
        {
            auto str = resolutionToString(data.switchResolution);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case codec::VideoParameters::DesiredFramerate:
        {
            auto str = std::to_string(data.desiredFrameRate);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case codec::VideoParameters::BitrateKB:
        {
            auto str = std::to_string(data.bitrateKB);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case codec::VideoParameters::VsyncMode:
        {
            auto str = ffmpeg::vsyncModeToDesc(data.vsyncMode);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case codec::VideoParameters::HWAccelMode:
        {
            auto str = ffmpeg::hwAccelModeToDesc(data.hwaccelMode);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case codec::VideoParameters::VideoCodec:
        {
            auto str = ffmpeg::videoCodecToDesc(data.videoCodec);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case codec::VideoParameters::MonitorNumber:
        {
            auto str = std::to_string(data.monitorNumber);
            textElements[param].value = prefix + ": " + str;
        }
        break;
    }
}

void GenericCodecMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = codec::videoParametersList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}