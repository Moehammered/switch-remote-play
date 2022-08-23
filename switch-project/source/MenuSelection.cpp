#include "MenuSelection.h"
#include "utils/Colours.h"
#include <iostream>

namespace
{
    auto constexpr noHostInfoMessage = "Host IP: Not yet found. Press 'L' to start search...";
    auto constexpr defaultControlMessage = "'ZL'/'ZR' - cycle screens | 'A'/'B' - change settings | 'R' - start stream";

    enum MenuScreen : int32_t
    {
        ENCODER_SCREEN,
        CONTROLLER_SCREEN,
        MOUSE_SCREEN,
        TOUCH_SCREEN,
        KEYBOARD_SCREEN,
        AUDIO_SCREEN,
        NETWORK_SCREEN,
        DECODER_SCREEN,
        HELP_SCREEN,
        COUNT
    };
}

MenuSelection::MenuSelection() : Menu(), 
    controlsText{},hostConnectionText{}, 
    streamPendingText{}, helpScreen{},
    encoderScreen{}, networkScreen{},
    menus{}, selectedMenu{ENCODER_SCREEN}, 
    newDecoderMenu{}, newControllerMenu{},
    touchMenu{}, mouseMenu{}, keyboardMenu{},
    audioMenu{}
{
    menus[MenuScreen::HELP_SCREEN] = &helpScreen;
    menus[MenuScreen::DECODER_SCREEN] = &newDecoderMenu;
    menus[MenuScreen::CONTROLLER_SCREEN] = &newControllerMenu;
    menus[MenuScreen::ENCODER_SCREEN] = &encoderScreen;
    menus[MenuScreen::NETWORK_SCREEN] = &networkScreen;
    menus[MenuScreen::TOUCH_SCREEN] = &touchMenu;
    menus[MenuScreen::MOUSE_SCREEN] = &mouseMenu;
    menus[MenuScreen::KEYBOARD_SCREEN] = &keyboardMenu;
    menus[MenuScreen::AUDIO_SCREEN] = &audioMenu;

    title.x = 400; title.y = 5;
    title.colour = colours::green;
    title.value = "Switch Remote Play \\(^.^)/";

    controlsText.x = 100; controlsText.y = 45;
    controlsText.colour = colours::green;
    controlsText.value = defaultControlMessage;

    hostConnectionText.x = 100; hostConnectionText.y = 85;
    hostConnectionText.colour = colours::red;
    hostConnectionText.value = noHostInfoMessage;

    streamPendingText.x = 100; streamPendingText.y = 600;
    streamPendingText.colour = colours::red;
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
        hostConnectionText.Render(renderer, font, colours::orange);
    }
    else if(network.HostFound())
    {
        hostConnectionText.value = "Host IP: " + network.IPAddress();
        hostConnectionText.Render(renderer, font, colours::green);
    }
    else if(network.Searching())
    {
        hostConnectionText.value = "Host IP: Searching...";
        hostConnectionText.Render(renderer, font, colours::blue);
    }
    else
    {
        hostConnectionText.value = noHostInfoMessage;
        hostConnectionText.Render(renderer, font);
    }
}

EncoderConfig const MenuSelection::GetFfmpegSettings() const
{
    return encoderScreen.Settings();
}

DecoderData const MenuSelection::GetDecoderSettings() const
{
    return newDecoderMenu.Settings();
}

controller::ControllerConfig const MenuSelection::GetControllerSettings() const
{
    return newControllerMenu.Settings();
}

mouse::MouseConfig const MenuSelection::MouseSettings() const
{
    return mouseMenu.Settings();
}

keyboard::KeyboardConfig const MenuSelection::KeyboardSettings() const
{
    return keyboardMenu.Settings();
}

touch::TouchConfig const MenuSelection::TouchSettings() const
{
    return touchMenu.Settings();
}

network::NetworkConfig const MenuSelection::NetworkSettings() const
{
    return networkScreen.Settings();
}

ConfigContainer const MenuSelection::ConfigurationSettings() const
{
    return ConfigContainer
    {
        .encoderConfig = GetFfmpegSettings(),
        .decoderConfig = GetDecoderSettings(),
        .controllerConfig = GetControllerSettings(),
        .mouseConfig = MouseSettings(),
        .keyboardConfig = KeyboardSettings(),
        .touchConfig = TouchSettings(),
        .networkConfig = NetworkSettings()
    };
}