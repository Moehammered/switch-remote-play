#include "MainScreen.h"
#include <iostream>

auto constexpr noHostInfoMessage = "Host IP: Not yet found. Press 'L' to start search...";
auto constexpr defaultControlMessage = "'ZL'/'ZR' - cycle screens | 'A'/'B' - change settings | 'R' - start stream";

MenuSelection::MenuSelection() : Menu(), 
    controlsText{},hostConnectionText{}, 
    streamPendingText{}, helpScreen{},
    controllerScreen{}, encoderScreen{}, networkScreen{},
    menus{}, selectedMenu{ENCODER_CONFIG}, newDecoderMenu{}
{
    menus[MenuScreen::HELP] = &helpScreen;
    menus[MenuScreen::DECODER_CONFIG] = &newDecoderMenu;
    menus[MenuScreen::CONTROLLER] = &controllerScreen;
    menus[MenuScreen::ENCODER_CONFIG] = &encoderScreen;
    menus[MenuScreen::IP_SET] = &networkScreen;

    title.x = 400; title.y = 5;
    title.colour = green;
    title.value = "Switch Remote Play \\(^.^)/";

    controlsText.x = 100; controlsText.y = 45;
    controlsText.colour = green;
    controlsText.value = defaultControlMessage;

    hostConnectionText.x = 100; hostConnectionText.y = 85;
    hostConnectionText.colour = red;
    hostConnectionText.value = noHostInfoMessage;

    streamPendingText.x = 100; streamPendingText.y = 600;
    streamPendingText.colour = red;
    streamPendingText.value = "Stream Pending Connection...";
}

void MenuSelection::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    auto selected = (int32_t)selectedMenu;
    if(kDown & KEY_ZL)
        --selected;
    else if(kDown & KEY_ZR)
        ++selected;

    if(selected < 0)
        selected = MenuScreen::COUNT - 1;
    else if(selected >= MenuScreen::COUNT)
        selected = 0;

    selectedMenu = (MenuScreen)selected;

    menus[selectedMenu]->ProcessInput(pad);
}

void MenuSelection::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    menus[selectedMenu]->Render(renderer, font);
}

void MenuSelection::RenderTitle(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    controlsText.Render(renderer, font);
}

void MenuSelection::RenderPendingConnection(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    controlsText.Render(renderer, font);
    streamPendingText.Render(renderer, font);
}

void MenuSelection::RenderNetworkStatus(SDL_Renderer * const renderer, FC_Font * const font, NetworkDiscovery const & network)
{
    if(networkScreen.UseManualIP())
    {
        hostConnectionText.value = "Host IP: (Manual)" + networkScreen.ManualIPAddress();
        hostConnectionText.Render(renderer, font, orange);
    }
    else if(network.HostFound())
    {
        hostConnectionText.value = "Host IP: " + network.IPAddress();
        hostConnectionText.Render(renderer, font, green);
    }
    else if(network.Searching())
    {
        hostConnectionText.value = "Host IP: Searching...";
        hostConnectionText.Render(renderer, font, blue);
    }
    else
    {
        hostConnectionText.value = noHostInfoMessage;
        hostConnectionText.Render(renderer, font);
    }
}

EncoderConfig const MenuSelection::GetFfmpegSettings()
{
    return encoderScreen.Settings();
}

DecoderData const MenuSelection::GetDecoderSettings()
{
    return newDecoderMenu.Settings();
}

ControllerConfig const MenuSelection::GetControllerSettings()
{
    return controllerScreen.Settings();
}

bool MenuSelection::UseManualIP()
{
    return networkScreen.UseManualIP();
}

std::string const MenuSelection::GetManualIPAddress()
{
    return networkScreen.ManualIPAddress();
}
