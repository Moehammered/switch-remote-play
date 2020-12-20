#include "MainScreen.h"
#include "ui/Menu.h"
#include "ui/FFMPEGConfigUI.h"
#include "ui/ManualNetworkConfig.h"
#include <iostream>

auto constexpr noHostInfoMessage = "Host IP: Not yet found. Press 'L' to start search...";
auto constexpr defaultControlMessage = 
"Welcome to Switch Remote Play. \n\
Press ZL/ZR to cycle screens. D-Pad to cycle settings where applicable.\n\n\
When the host IP is set, press 'R' to start stream connection.\n\
(will be unresponsive until a connection to a PC is made)";

Text const title{
    .x = 400, .y = 20, .colour = green,
    .value = "Switch Remote Play \\(^.^)/"
};
Text const controlText{
    .x = 100, .y = 60, .colour = green, 
    .value = defaultControlMessage
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

void UpdateScreens()
{
    configRenderer.Config(configurator.Settings());

    switch(currentMenu)
    {
        default:
        case MAIN:
            currentScreenText.value = "Main Screen";
        break;

        case CONFIG:
            currentScreenText.value = "Configuration Screen";
        break;

        case IP_SET:
            // render manual IP setting
            currentScreenText.value = "Manual IP Screen";
        break;
    }
}

void SetupMainScreen()
{
    hostConnectionText = Text{
        .x = 100, .y = 250, .colour = red,
        .value = noHostInfoMessage
    };

    currentScreenText = Text{
        .x = 400, .y = 300, .colour = white,
        .value = "Main Screen"
    };

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
            configRenderer.Render(renderer, systemFont);
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
    if(network != nullptr && network->HostFound())
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
    if(network.HostFound())
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
