#include "EncoderMenu.h"

EncoderMenu::EncoderMenu() 
    : Menu(), h264Menu{}, amdMenu{}, generalMenu{},
    menus{
        {VideoCodec::H264, &h264Menu},
        {VideoCodec::H264_AMF, &amdMenu}
    },
    advancedOptions{false}, pageText{}
    // menus{}, menuCursor{menus}
{
    title.value = "Encoder Configuration";
    pageText.value = "General (pg 1/2) - <--/-->";
    pageText.y = title.y + 35;
    pageText.x = 20;
    pageText.colour = { 255, 190, 90, 255 };
    // auto config = Configuration_Old{};
    // auto ffmpeg = config.FFMPEGData();

    // menus.push_back(&generalMenu);
    // menus.push_back(&h264Menu);
    // menus.push_back(&amdMenu);
}

void EncoderMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & (KEY_DLEFT | KEY_DRIGHT))
        advancedOptions = !advancedOptions;

    auto updatePageText = [&](VideoCodec codec)
    {
        switch(codec)
        {
            case VideoCodec::H264:
                pageText.value = "H264 (pg 2/2) - <--/-->";
            break;
            case VideoCodec::H264_AMF:
                pageText.value = "H264 AMD (pg 2/2) - <--/-->";
            break;
            default:
                pageText.value = "UNSUPPORTED (pg 2/2) - <--/-->";
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
        pageText.value = "General (pg 1/2) - <--/-->";
        generalMenu.ProcessInput(pad);
    }
    // cursor impl
    // if(kDown & KEY_DLEFT)
    //     --menuCursor;
    // else if(kDown & KEY_DRIGHT)
    //     ++menuCursor;

    // auto menu = *menuCursor;
    // menu->ProcessInput(pad);
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
    // cursor impl
    // auto menu = *menuCursor;
    // menu->Render(renderer, font);
}

EncoderConfig const EncoderMenu::Settings() const
{
    return {};
}
