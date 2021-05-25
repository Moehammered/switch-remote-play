#include "DecoderMenu2.h"
#include "../decoder/DecoderConfiguration.h"

DecoderMenu2::DecoderMenu2() : Menu(),
cursor{DecoderParameterList}, selected{},
flag1Menu{}, flag2Menu{}, accelMenu{},
discardMenu{},
menus{}
{
    title.value = "Decoder Menu 2";

    //config loading goes here
    auto config = DecoderConfiguration{"sdmc:/switch/switch-remote-play/decoder.ini"};
    auto settings = config.Data();
    flag1Menu.SetFlags(settings.flags1);
    flag2Menu.SetFlags(settings.flags2);

    accelMenu.SetFlags(settings.accelFlags);

    discardMenu.SetFlag(settings.discardFilter);
    threadMenu.SetTypeFlag(settings.threadType);
    threadMenu.SetThreadCount(settings.threadCount);

    menus[DecoderParameters::Flags1] = &flag1Menu;
    menus[DecoderParameters::Flags2] = &flag2Menu;
    menus[DecoderParameters::AccelFlags] = &accelMenu;
    menus[DecoderParameters::DiscardFilter] = &discardMenu;
    menus[DecoderParameters::ThreadType] = &threadMenu;
}

void DecoderMenu2::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Left)
    {
        --cursor;
        selected = *cursor;
    }
    else if (kDown & HidNpadButton::HidNpadButton_Right)
    {
        ++cursor;
        selected = *cursor;
    }

    auto currentMenu = menus.find(selected);
    if(currentMenu != menus.end())
        currentMenu->second->ProcessInput(pad);
    else
        menus.begin()->second->ProcessInput(pad);
}

void DecoderMenu2::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    auto currentMenu = menus.find(selected);
    if(currentMenu != menus.end())
        currentMenu->second->Render(renderer, font);
    else
        menus.begin()->second->Render(renderer, font);
}

DecoderData const DecoderMenu2::Settings() const
{
    return DecoderData{ 
        .flags1 = flag1Menu.Flags(),
        .flags2 = flag2Menu.Flags(),
        .accelFlags = accelMenu.Flags(),
        .discardFilter = discardMenu.Flag(),
        .threadType = threadMenu.TypeFlag(),
        .threadCount = threadMenu.ThreadCount()
    };
}
