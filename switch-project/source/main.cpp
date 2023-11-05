/*
  switch-remote-play
    Remote PC connection focused on allowing PC games to be played on the switch
*/

#include "ScreenRenderer.h"
#include "MenuSelection.h"
#include "InputThread.h"
#include "utils/StreamHelper.h"
#include "utils/ConfigHelper.h"
#include "utils/Colours.h"
#include "network/NetworkDiscovery.h"
#include "stream/LiveStream.h"
#include "stream/StreamState.h"
#include "system/SystemSetup.h"
#include "network/NetworkConfiguration.h"
#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <switch.h>

namespace
{
    uint16_t constexpr contextWidth = 1280;
    uint16_t constexpr contextHeight = 720;
    uint32_t constexpr fontSize = 24;

    //30fps refresh rate
    auto constexpr thirtyThreeMs = std::chrono::duration<int, std::milli>(33);
    auto constexpr oneSecond = std::chrono::duration<int, std::milli>(1000);

    std::atomic_int32_t streamState;

    void handleInitFailure()
    {
        auto errorMessage = "Failed to initialise screen renderer...";
        //doesn't display on the switch screen from nxlink redirecting stdio
        consoleInit(NULL);
        std::cout << errorMessage << std::endl;
        
        consoleUpdate(NULL);
        auto countdown = 3;
        while(countdown > 0)
        {
            --countdown;
            std::this_thread::sleep_for(oneSecond);
            consoleUpdate(NULL);
        }

        consoleExit(NULL);
        
        cleanupSystem();
    }
}

int main(int argc, char **argv)
{
    std::cout << "Initialising Switch\n";
    initialiseSwitch(); 
    streamState = { StreamState::INACTIVE };
    
    ScreenRenderer screen;
    std::cout << "Initialising Screen\n";
    bool initOK = screen.Initialise(contextWidth, contextHeight, false);

    if(!initOK)
    {
        handleInitFailure();
        cleanupSystem();
        return -1;
    }

    auto const startupNetworkSettings = NetworkConfiguration().Data();
    std::string broadcastAddress{startupNetworkSettings.broadcastIP};
    std::cout << "Initialising Network Discovery\n";
    std::cout << "Broadcasting discovery on address " << broadcastAddress << "\n";
    NetworkDiscovery network {startupNetworkSettings.handshakePort, broadcastAddress, startupNetworkSettings.broadcastPort};
    
    std::thread gamepadThread{};
    auto framesTillCout = 1000;
    auto liveRenderCallback = [&] (YUVFrame frameData) 
    {
        if(--framesTillCout == 0)
        {
            std::cout << "render frame called\n\n";
            framesTillCout = 1000;
        }
        renderStreamFrame(screen, frameData);
    };
    LiveStream liveStream { startupNetworkSettings.audioPort };
    
    std::cout << "Loading System Fonts\n";
    auto systemFont = loadSystemFont(screen.Renderer(), fontSize, colours::white);
    setMainSystemFont(systemFont);
    
    //initialise hid
    PadState mainPad {0};
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&mainPad);

    std::cout << "Creating menu selection screens\n";
    auto menuScreens = MenuSelection{};
    
    auto runApp { true };
    std::cout << "Starting main loop\n";
    while(appletMainLoop() && runApp)
    {
        switch(streamState.load(std::memory_order_acquire))
        {
            case StreamState::INACTIVE:
            {
                auto nextState = renderMenus(screen, mainPad, menuScreens, network, systemFont);
                streamState.store(nextState, std::memory_order_release);
            }
            break;

            case StreamState::REQUESTED:
            {
                auto nextState = processStreamRequest(screen, 
                                                    menuScreens, 
                                                    liveStream, 
                                                    network, 
                                                    startupNetworkSettings, 
                                                    systemFont, 
                                                    gamepadThread);
                streamState.store(nextState, std::memory_order_release);
            }
            break;

            case StreamState::ACTIVE:
            {
                if(!liveStream.Run(liveRenderCallback))
                {
                    streamState.store(StreamState::INACTIVE, std::memory_order_release);
            
                    if(gamepadThread.joinable())
                        gamepadThread.join();
                }
                padUpdate(&mainPad); // stop the '+' button from quitting the app when stream stops
            }
            break;

            case StreamState::QUIT:
            {
                runApp = false;
            }
            break;
        }
    }

    std::cout << "shutting down stream state and cleaning up livestream\n";
    //wait here if the stream is still on so we can clean it up properly
    if(streamState.load() == StreamState::ACTIVE)
    {
        while(streamState.load() == StreamState::ACTIVE)
            std::this_thread::sleep_for(thirtyThreeMs);

        liveStream.Cleanup();
    }
    
    std::cout << "shutting down network broadcaster\n";
    network.Shutdown();

    if(gamepadThread.joinable())
        gamepadThread.join();

    std::cout << "quitting SDL\n";
    SDL_Quit();
    auto libnxRes = audoutStopAudioOut();
    if(!R_SUCCEEDED(libnxRes))
        std::cout << "Failed to call audoutStopAudioOut with result: " << libnxRes << std::endl;
    freeFont(systemFont);
    cleanupSystem();
    
    return 0;
}