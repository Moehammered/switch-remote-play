#include "MainScreen.h"
#include "ui/Menu.h"
#include "ui/FFMPEGConfigUI.h"
#include <iostream>

auto constexpr noHostInfoMessage = "Host IP: Not yet found. Press 'L' to start search...";
auto constexpr defaultControlMessage = 
"Ready to accept a video stream connection.\n\
Press d-pad to cycle stream settings.\n\n\
Press 'R' to start stream connection.\n\
(will be unresponsive until a connection to a PC is made)";
Menu titleMenu;
Text* hostConnectionText;
Text streamPendingText;
FFMPEGConfigUI configRenderer;

void SetupMainScreen()
{
    titleMenu = Menu{};
    titleMenu.AddElement({
        .x = 400, .y = 20, .colour = green,
        .value = "Switch Remote Play \\(^.^)/"
    });
    titleMenu.AddElement({
        .x = 100, .y = 60, .colour = green, 
        .value = defaultControlMessage
    });
    hostConnectionText = &(titleMenu.AddElement({
        .x = 100, .y = 250, .colour = red,
        .value = noHostInfoMessage
    }));

    configRenderer = FFMPEGConfigUI{};
    
    streamPendingText = {
        .x = 100, .y = 600, .colour = red,
        .value = "Stream Pending Connection..." 
    };
}

void RenderMainScreen(SDL_Renderer * const renderer, FC_Font * const systemFont)
{
    titleMenu.Render(renderer, systemFont);
    configRenderer.Render(renderer, systemFont);
}

void RenderNetworkStatus(SDL_Renderer * const renderer, FC_Font * const systemFont, NetworkDiscovery const * network)
{
    if(network != nullptr && network->HostFound())
    {
        hostConnectionText->value = "Host IP: " + network->IPAddress();
        hostConnectionText->Render(renderer, systemFont, green);
    }
    else if(network != nullptr && network->Searching())
    {
        hostConnectionText->value = "Host IP: Searching...";
        hostConnectionText->Render(renderer, systemFont, blue);
    }
    else
    {
        hostConnectionText->value = noHostInfoMessage;
        hostConnectionText->Render(renderer, systemFont);
    }
}

void RenderNetworkStatus(SDL_Renderer * const renderer, FC_Font * const systemFont, NetworkDiscovery const & network)
{
    if(network.HostFound())
    {
        hostConnectionText->value = "Host IP: " + network.IPAddress();
        hostConnectionText->Render(renderer, systemFont, green);
    }
    else if(network.Searching())
    {
        hostConnectionText->value = "Host IP: Searching...";
        hostConnectionText->Render(renderer, systemFont, blue);
    }
    else
    {
        hostConnectionText->value = noHostInfoMessage;
        hostConnectionText->Render(renderer, systemFont);
    }
}

void RenderPendingConnectionScreen(SDL_Renderer * const renderer, FC_Font * const systemFont)
{
    titleMenu.Render(renderer, systemFont);
    
    streamPendingText.Render(renderer, systemFont);
}
