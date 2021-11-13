#include "EncoderMenu.h"
#include "../utils/Colours.h"

EncoderMenu::EncoderMenu() 
    : Menu(), h264Menu{}, amdMenu{}, generalMenu{},
    menus{
        {ffmpeg::VideoCodec::H264, &h264Menu},
        {ffmpeg::VideoCodec::H264_AMF, &amdMenu}
    },
    advancedOptions{false}, pageText{}
{
    title.value = "Encoder Configuration";
    pageText.value = "General (pg 1/2) <--/-->";
    pageText.y = title.y + 35;
    pageText.x = 20;
    pageText.colour = colours::orange;
}

void EncoderMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & (HidNpadButton_Left | HidNpadButton_Right))
        advancedOptions = !advancedOptions;

    auto updatePageText = [&](ffmpeg::VideoCodec codec)
    {
        switch(codec)
        {
            case ffmpeg::VideoCodec::H264:
                pageText.value = "H264 (pg 2/2) <--/-->";
            break;
            case ffmpeg::VideoCodec::H264_AMF:
                pageText.value = "H264 AMD (pg 2/2) <--/-->";
            break;
            default:
                pageText.value = "UNSUPPORTED (pg 2/2) <--/-->";
            break;
        } 
    };

    if(advancedOptions)
    {
        auto codec = generalMenu.VideoCodecSettings().videoCodec;
        auto menu = menus.find(codec);
        if(menu != menus.end())
            menu->second->ProcessInput(pad);
        updatePageText(codec);
    }
    else
    {
        pageText.value = "General (pg 1/2) <--/-->";
        generalMenu.ProcessInput(pad);
    }
}

void EncoderMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    pageText.Render(renderer, font);

    if(advancedOptions)
    {
        auto codec = generalMenu.VideoCodecSettings().videoCodec;
        auto menu = menus.find(codec);
        if(menu != menus.end())
            menu->second->Render(renderer, font);
    }
    else
        generalMenu.Render(renderer, font);
}

EncoderConfig const EncoderMenu::Settings() const
{
    auto data = EncoderConfig{};
    data.commonSettings = generalMenu.VideoCodecSettings();
    
    switch(data.commonSettings.videoCodec)
    {
        default:
        case ffmpeg::VideoCodec::H264:
            data.cpuSettings = h264Menu.Settings();
        break;
        
        case ffmpeg::VideoCodec::H264_AMF:
            data.amdSettings = amdMenu.Settings();
        break;
    }

    return data;
}
