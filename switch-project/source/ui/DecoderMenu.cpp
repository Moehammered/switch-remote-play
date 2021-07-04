#include "DecoderMenu.h"
#include "../decoder/DecoderConfiguration.h"

DecoderMenu::DecoderMenu() : Menu(),
cursor{DecoderParameterList}, selected{},
flag1Menu{}, flag2Menu{}, accelMenu{},
discardMenu{}, menus{}, pageText{}
{
    title.value = "Decoder Menu";

    auto config = DecoderConfiguration{};
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

    pageText.value = "(pg 1/5) <--/-->";
    pageText.y = title.y + 65;
    pageText.x = 20;
    pageText.colour = { 255, 190, 90, 255 };
}

void DecoderMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Left)
    {
        --cursor;
        selected = *cursor;
        UpdatePageHeader(selected);
    }
    else if (kDown & HidNpadButton::HidNpadButton_Right)
    {
        ++cursor;
        selected = *cursor;
        UpdatePageHeader(selected);
    }

    auto currentMenu = menus.find(selected);
    if(currentMenu != menus.end())
        currentMenu->second->ProcessInput(pad);
    else
        menus.begin()->second->ProcessInput(pad);
}

void DecoderMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    pageText.Render(renderer, font);

    auto currentMenu = menus.find(selected);
    if(currentMenu != menus.end())
        currentMenu->second->Render(renderer, font);
    else
        menus.begin()->second->Render(renderer, font);
}

DecoderData const DecoderMenu::Settings() const
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

void DecoderMenu::UpdatePageHeader(DecoderParameters param)
{
    switch(param)
    {
        case DecoderParameters::Flags1:
            pageText.value = "(pg 1/5) <--/-->";
        break;
        
        case DecoderParameters::Flags2:
            pageText.value = "(pg 2/5) <--/-->";
        break;
        
        case DecoderParameters::AccelFlags:
            pageText.value = "(pg 3/5) <--/-->";
        break;
        
        case DecoderParameters::DiscardFilter:
            pageText.value = "(pg 4/5) <--/-->";
        break;

        case DecoderParameters::ThreadCount:
        case DecoderParameters::ThreadType:
            pageText.value = "(pg 5/5) <--/-->";
        break;

        default:
            pageText.value = "Unknown page value...";
        break;
    }
}