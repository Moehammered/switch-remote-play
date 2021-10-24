#include "MainScreen.h"
#include <iostream>

namespace
{
    auto constexpr noHostInfoMessage = "Host IP: Not yet found. Press 'L' to start search...";
    auto constexpr defaultControlMessage = "'ZL'/'ZR' - cycle screens | 'A'/'B' - change settings | 'R' - start stream";

    enum MenuScreen : int32_t
    {
        HELP,
        DECODER_CONFIG,
        ENCODER_CONFIG,
        CONTROLLER,
        IP_SET,
        TOUCH_MENU,
        MOUSE_MENU,
        KEYBOARD_MENU,
        COUNT
    };

    SDL_Color constexpr black = {0,0,0, 255};
    SDL_Color constexpr green = { 100, 200, 100, 255 };
    SDL_Color constexpr orange = { 255, 190, 90, 255 };
    SDL_Color constexpr red = { 200, 100, 100, 255 };
    SDL_Color constexpr blue = {100, 100, 200, 255};
    SDL_Color constexpr white = {255, 255, 255, 255};
}

MenuSelection::MenuSelection() : Menu(), 
    controlsText{},hostConnectionText{}, 
    streamPendingText{}, helpScreen{},
    encoderScreen{}, networkScreen{},
    menus{}, selectedMenu{KEYBOARD_MENU}, 
    newDecoderMenu{}, newControllerMenu{},
    touchMenu{}, mouseMenu{}, keyboardMenu{}
{
    menus[MenuScreen::HELP] = &helpScreen;
    menus[MenuScreen::DECODER_CONFIG] = &newDecoderMenu;
    menus[MenuScreen::CONTROLLER] = &newControllerMenu;
    menus[MenuScreen::ENCODER_CONFIG] = &encoderScreen;
    menus[MenuScreen::IP_SET] = &networkScreen;
    menus[MenuScreen::TOUCH_MENU] = &touchMenu;
    menus[MenuScreen::MOUSE_MENU] = &mouseMenu;
    menus[MenuScreen::KEYBOARD_MENU] = &keyboardMenu;

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
    if(kDown & HidNpadButton_ZL)
        --selected;
    else if(kDown & HidNpadButton_ZR)
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
    auto networkSettings = networkScreen.Settings();
    if(networkSettings.manualIPEnabled)
    {
        hostConnectionText.value = "Host IP: (Manual)" + networkSettings.manualIP;
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

controller::ControllerConfig const MenuSelection::GetControllerSettings()
{
    return newControllerMenu.Settings();
}

mouse::MouseConfig const MenuSelection::MouseSettings()
{
    return mouseMenu.Settings();
}

keyboard::KeyboardConfig const MenuSelection::KeyboardSettings() const
{
    return keyboardMenu.Settings();
}

touch::TouchConfig const MenuSelection::TouchSettings()
{
    return touchMenu.Settings();
}

network::NetworkConfig const MenuSelection::NetworkSettings() const
{
    return networkScreen.Settings();
}