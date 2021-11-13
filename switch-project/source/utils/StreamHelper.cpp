#include "StreamHelper.h"
#include "Colours.h"
#include "../InputThread.h"
#include <chrono>
#include <switch.h>

namespace
{
    auto constexpr thirtyThreeMs = std::chrono::duration<int, std::milli>(33);
    auto constexpr oneSecond = std::chrono::duration<int, std::milli>(1000);
}

StreamState const renderMenus(ScreenRenderer& screen,
    PadState& mainPad,
    MenuSelection& menuScreens,
    NetworkDiscovery& network,
    FC_Font* systemFont)
{
    screen.ClearScreen(colours::offblack);

    padUpdate(&mainPad);

    auto kDown = padGetButtonsDown(&mainPad);
    auto nextState = StreamState::INACTIVE;

    if(kDown & HidNpadButton_Plus)
        nextState = StreamState::QUIT;
    else if(kDown & HidNpadButton_R)
        nextState = StreamState::REQUESTED;

    menuScreens.ProcessInput(mainPad);
    
    if(kDown & HidNpadButton_L)
    {
        if(!network.Searching())
            network.Search();
    }

    menuScreens.RenderTitle(screen.Renderer(), systemFont);
    menuScreens.RenderNetworkStatus(screen.Renderer(), systemFont, network);
    menuScreens.Render(screen.Renderer(), systemFont);
    screen.PresentScreen();
    
    // no point thrashing the screen to refresh text
    std::this_thread::sleep_for(thirtyThreeMs);

    return nextState;
}

StreamState const processStreamRequest(ScreenRenderer& screen,
    MenuSelection& menuScreens,
    LiveStream& liveStream,
    NetworkDiscovery& network,
    network::NetworkConfig const startupNetworkSettings,
    FC_Font* systemFont,
    std::thread& gamepadThread)
{
    //display on the screen a connection is pending
    screen.ClearScreen(colours::grey);
    menuScreens.RenderTitle(screen.Renderer(), systemFont);
    menuScreens.RenderPendingConnection(screen.Renderer(), systemFont);
    menuScreens.RenderNetworkStatus(screen.Renderer(), systemFont, network);
    
    screen.PresentScreen();
    auto const settings = menuScreens.ConfigurationSettings();
    SaveConfigData(settings);

    auto const& networkSettings = settings.networkConfig;
    if(network.HostFound() || networkSettings.manualIPEnabled)
    {
        auto ip = std::string{};
        if(networkSettings.manualIPEnabled)
            ip = networkSettings.manualIP;
        else
            ip = network.IPAddress();

        sendStartStreamCommand(ip, startupNetworkSettings.commandPort, settings);

        if(liveStream.Startup(settings.decoderConfig, startupNetworkSettings.videoPort))
        {
            gamepadThread = std::thread(runGamepadThread, ip, startupNetworkSettings.gamepadPort);
            return StreamState::ACTIVE;
        }
        else
            return StreamState::INACTIVE;
    }
    else // no host to connect to
    {
        std::this_thread::sleep_for(oneSecond);
        return StreamState::INACTIVE;
    }
}

void renderStreamFrame(ScreenRenderer& screen, YUVFrame frameData)
{
    auto region = screen.Region();
    SDL_UpdateYUVTexture(screen.GetScreenTexture(), &region, 
                        frameData.yPlane, frameData.yPlaneStride,
                        frameData.uPlane, frameData.uPlaneStride,
                        frameData.vPlane, frameData.vPlaneStride);

    screen.RenderScreenTexture();
}