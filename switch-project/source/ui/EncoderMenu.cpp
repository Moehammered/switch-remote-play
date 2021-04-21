#include "EncoderMenu.h"

EncoderMenu::EncoderMenu() 
    : Menu(), h264Menu{}, amdMenu{},
    menus{}, menuCursor{menus}
{
    title.value = "Encoder Configuration";
    
    // auto config = Configuration_Old{};
    // auto ffmpeg = config.FFMPEGData();

    menus.push_back(&h264Menu);
    menus.push_back(&amdMenu);
}

void EncoderMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & KEY_DLEFT)
        --menuCursor;
    else if(kDown & KEY_DRIGHT)
        ++menuCursor;

    auto menu = *menuCursor;
    menu->ProcessInput(pad);
}

void EncoderMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    auto menu = *menuCursor;
    menu->Render(renderer, font);
}

EncoderConfig const EncoderMenu::Settings() const
{
    return {};
}
