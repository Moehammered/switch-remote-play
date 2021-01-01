#include "MainScreen.h"
#include "ui/Menu.h"
#include "ui/ConfigurationScreen.h"
#include "ui/ManualNetworkConfig.h"
#include <iostream>
#include "system/Configuration.h"

auto constexpr noHostInfoMessage = "Host IP: Not yet found. Press 'L' to start search...";
auto constexpr defaultControlMessage = "'ZL'/'ZR' - cycle screens | 'A'/'B' - change settings | 'R' - start stream";

auto constexpr instructions =
"1. Please make sure the switch-remote-play-host application is running on your Windows PC.\n\
2. Please connect to a 5GHZ wifi network if you can. (e.g. homewifi_5GHz).\n\
3. Please make sure ports 19999 to 20004 aren't blocked or occupied on your PC.\n\
4. Please make sure Windows Firewall isn't blocking ffmpeg or switch-remote-play-host.\n\n\
Stream tips:\n\
- Hold '+' for more than 3 seconds to close the stream.\n\
- Hold 'ZL+ZR+B' or touch the screen for 3 seconds to toggle mouse/ps4 controller.\n\
- Favor lower desktop resolution to reduce latency.\n\
- You can mute your PC and audio will still stream to the switch.\
";

Text const title{
    .x = 400, .y = 20, .colour = green,
    .value = "Switch Remote Play \\(^.^)/"
};
Text const controlText{
    .x = 100, .y = 60, .colour = green, 
    .value = defaultControlMessage
};

Text const placeholderInstructions{
    .x = 10, .y = 250, .colour = orange, 
    .value = instructions
};

Text hostConnectionText{};
Text currentScreenText{};
Text const streamPendingText{
    .x = 100, .y = 600, .colour = red,
    .value = "Stream Pending Connection..." 
};
ConfigurationScreen configurator;
NetworkMenu networkScreen;
MenuScreen currentMenu {MAIN};
Configuration config;

void UpdateScreens()
{
    switch(currentMenu)
    {
        default:
        case MAIN:
            currentScreenText.value = "Help Screen";
        break;

        case CONFIG:
            currentScreenText.value = "Configuration Screen";
        break;

        case IP_SET:
            // render manual IP setting
            currentScreenText.value = "Manual IP Screen";
        break;
    }

    config = Configuration{};
}

void SetupMainScreen()
{
    hostConnectionText = Text{
        .x = 250, .y = 120, .colour = red,
        .value = noHostInfoMessage
    };

    currentScreenText = Text{
        .x = 440, .y = 180, .colour = white,
        .value = "Help Screen"
    };

    config = Configuration{};
    networkScreen = NetworkMenu{};

    UpdateScreens();
}

void ProcessScreenInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & KEY_ZL)
        PreviousScreen();
    else if(kDown & KEY_ZR)
        NextScreen();

    switch(currentMenu)
    {
        default:
        case MAIN:
        break;

        case CONFIG:
            configurator.ProcessInput(pad);
        break;

        case IP_SET:
            networkScreen.ProcessInput(pad);
        break;
    }
}

void NextScreen()
{
    auto menu = (int32_t)currentMenu;
    ++menu;
    if(menu >= (int32_t)COUNT)
        menu = MAIN;
    
    currentMenu = (MenuScreen)menu;
    UpdateScreens();
}

void PreviousScreen()
{
    auto menu = (int32_t)currentMenu;
    --menu;
    if(menu < (int32_t)MAIN)
        menu = COUNT - 1;
    
    currentMenu = (MenuScreen)menu;
    UpdateScreens();
}

void RenderMainScreen(SDL_Renderer * const renderer, FC_Font * const systemFont)
{
    title.Render(renderer, systemFont);
    controlText.Render(renderer, systemFont);
    currentScreenText.Render(renderer, systemFont);

    switch(currentMenu)
    {
        default:
        case MAIN:
            placeholderInstructions.Render(renderer, systemFont);
        break;

        case CONFIG:
            configurator.Render(renderer, systemFont);
        break;

        case IP_SET:
            // render manual IP setting
            networkScreen.Render(renderer, systemFont);
        break;
    }
}

void RenderNetworkStatus(SDL_Renderer * const renderer, FC_Font * const systemFont, NetworkDiscovery const * network)
{
    if(networkScreen.UseManualIP())
    {
        hostConnectionText.value = "Host IP: (Manual)" + networkScreen.ManualIPAddress();
        hostConnectionText.Render(renderer, systemFont, orange);
    }
    else if(network != nullptr && network->HostFound())
    {
        hostConnectionText.value = "Host IP: " + network->IPAddress();
        hostConnectionText.Render(renderer, systemFont, green);
    }
    else if(network != nullptr && network->Searching())
    {
        hostConnectionText.value = "Host IP: Searching...";
        hostConnectionText.Render(renderer, systemFont, blue);
    }
    else
    {
        hostConnectionText.value = noHostInfoMessage;
        hostConnectionText.Render(renderer, systemFont);
    }
}

void RenderNetworkStatus(SDL_Renderer * const renderer, FC_Font * const systemFont, NetworkDiscovery const & network)
{
    if(networkScreen.UseManualIP())
    {
        hostConnectionText.value = "Host IP: (Manual)" + networkScreen.ManualIPAddress();
        hostConnectionText.Render(renderer, systemFont, orange);
    }
    else if(network.HostFound())
    {
        hostConnectionText.value = "Host IP: " + network.IPAddress();
        hostConnectionText.Render(renderer, systemFont, green);
    }
    else if(network.Searching())
    {
        hostConnectionText.value = "Host IP: Searching...";
        hostConnectionText.Render(renderer, systemFont, blue);
    }
    else
    {
        hostConnectionText.value = noHostInfoMessage;
        hostConnectionText.Render(renderer, systemFont);
    }
}

void RenderPendingConnectionScreen(SDL_Renderer * const renderer, FC_Font * const systemFont)
{
    title.Render(renderer, systemFont);
    controlText.Render(renderer, systemFont);
    hostConnectionText.Render(renderer, systemFont);
    
    streamPendingText.Render(renderer, systemFont);
}

FFMPEG_Config const GetFfmpegSettings()
{
    return configurator.Settings();
}

bool UseManualIP()
{
    return networkScreen.UseManualIP();
}

std::string const GetManualIPAddress()
{
    return networkScreen.ManualIPAddress();
}