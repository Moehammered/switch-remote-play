#include "GenericCodecMenu.h"

GenericCodecMenu::GenericCodecMenu() : Menu(),
textElements{}, codec{}, selected{}
{
    title.value = "General Codec Options";
    title.y += 30;
    selected = codec.Current();

    //defaults
    codec.Set(VideoData{
        .desktopResolution = hdRes,
        .switchResolution = hdRes,
        .desiredFrameRate = Framerates.front(),
        .vsyncMode = DefaultVsyncMode,
        .hwaccelMode = DefaultHWAccelMode,
        .videoCodec = DefaultVideoCodec
    });
    
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

VideoData const GenericCodecMenu::VideoCodecSettings() const
{
    return codec.Data();
}

void GenericCodecMenu::UpdateUI(VideoParameters param)
{
    auto prefix = VideoParamToDesc.at(param);
    auto data = codec.Data();

    switch(param)
    {
        case VideoParameters::DesktopResolution:
        {
            auto str = ResolutionToString(data.desktopResolution);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case VideoParameters::SwitchResolution:
        {
            auto str = ResolutionToString(data.switchResolution);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case VideoParameters::DesiredFramerate:
        {
            auto str = std::to_string(data.desiredFrameRate);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case VideoParameters::VsyncMode:
        {
            auto str = VsyncModeToDesc(data.vsyncMode);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case VideoParameters::HWAccelMode:
        {
            auto str = HWAccelModeToDesc(data.hwaccelMode);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case VideoParameters::VideoCodec:
        {
            auto str = VideoCodecToDesc(data.videoCodec);
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

    auto params = VideoParametersList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}