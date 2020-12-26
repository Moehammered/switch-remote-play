#include "MainScreen.h"
#include "ui/Menu.h"
#include "ui/FFMPEGConfigUI.h"
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
FFMPEGConfigUI configurator;
FfmpegConfigRenderer configRenderer;
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
    configRenderer.Config(config.FFMPEGData());
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
    SetupManualNetworkScreen();

    UpdateScreens();
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

void SelectNext()
{
    switch(currentMenu)
    {
        default:
        case MAIN:
            // render current settings and start button
            
        break;

        case CONFIG:
            configurator.SelectNext();
        break;

        case IP_SET:
            // render manual IP setting
            SelectNextSegment();
        break;
    }
}

void SelectPrevious()
{
    switch(currentMenu)
    {
        default:
        case MAIN:
            // render current settings and start button
            
        break;

        case CONFIG:
            configurator.SelectPrevious();
        break;

        case IP_SET:
            // render manual IP setting
            SelectPreviousSegment();
        break;
    }
}

void IncreaseSetting()
{
    switch(currentMenu)
    {
        default:
        case MAIN:
            // render current settings and start button
            
        break;

        case CONFIG:
            configurator.IncreaseSetting();
        break;

        case IP_SET:
            // render manual IP setting
            IncreaseSegment();
        break;
    }
}

void DecreaseSetting()
{
    switch(currentMenu)
    {
        default:
        case MAIN:
            // render current settings and start button
            
        break;

        case CONFIG:
            configurator.DecreaseSetting();
        break;

        case IP_SET:
            // render manual IP setting
            DecreaseSegment();
        break;
    }
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
            // render current settings and start button
            // hide config renderer screen since it does nothing for now
            //configRenderer.Render(renderer, systemFont);
            placeholderInstructions.Render(renderer, systemFont);
        break;

        case CONFIG:
            configurator.Render(renderer, systemFont);
        break;

        case IP_SET:
            // render manual IP setting
            RenderNetworkConfigScreen(renderer, systemFont);
        break;
    }
}

void RenderNetworkStatus(SDL_Renderer * const renderer, FC_Font * const systemFont, NetworkDiscovery const * network)
{
    if(ManualIPMode())
    {
        hostConnectionText.value = "Host IP: (Manual)" + ManualIPAddress();
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
    if(ManualIPMode())
    {
        hostConnectionText.value = "Host IP: (Manual)" + ManualIPAddress();
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
    return ManualIPMode();
}

std::string const GetManualIPAddress()
{
    return ManualIPAddress();
}