/*
  switch-remote-play
    Remote PC connection focused on allowing PC games to be played on the switch
*/

#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <switch.h>
#include "ScreenRenderer.h"
#include "MainScreen.h"
#include "InputThread.h"
#include "network/NetworkDiscovery.h"
#include "stream/VideoStream.h"
#include "stream/StreamDecoder.h"
#include "stream/PcmStream.h"
#include "system/SystemSetup.h"
#include "system/Configuration.h"

auto constexpr handshakeKey = "let-me-play";
auto constexpr subnet = "192.168.0.255";

uint16_t constexpr handshakePort = 19999;
uint16_t constexpr broadcastPort = 20000;
uint16_t constexpr hostCommandPort = 20001;
uint16_t constexpr gamepadPort = 20002;
uint16_t constexpr videoPort = 20003;
uint16_t constexpr audioPort = 20004;

SDL_Color constexpr bgCol = {20, 20, 20, 255};
SDL_Color constexpr pendingStreamBgCol = { 60, 60, 60, 255 };

uint32_t constexpr fontSize = 24;

//30fps refresh rate
auto constexpr thirtyThreeMs = std::chrono::duration<int, std::milli>(33);
auto constexpr oneSecond = std::chrono::duration<int, std::milli>(1000);

std::atomic_int32_t streamState;

void processStream(VideoStream& stream, AVPacket& streamPacket, StreamDecoder*& streamDecoder,
                    SDL_Texture* rendererScreenTexture, SDL_Rect& renderRegion,
                    ScreenRenderer& screen, std::thread& gamepadThread)
{
    if(stream.Read(streamPacket))
    {
        if(streamDecoder->DecodeFramePacket(streamPacket))
        {
            // render frame data - expecting YUV420 format
            // (stride values represent space between horizontal lines across screen)
            auto decodedFrame = streamDecoder->DecodedFrame();

            auto yPlane = decodedFrame.data[0];
            auto yPlaneStride = decodedFrame.width;
            auto uPlane = decodedFrame.data[1];
            auto uPlaneStride = decodedFrame.width/2;
            auto vPlane = decodedFrame.data[2];
            auto vPlaneStride = decodedFrame.width/2;

            SDL_UpdateYUVTexture(rendererScreenTexture, &renderRegion, 
                                yPlane, yPlaneStride,
                                uPlane, uPlaneStride, 
                                vPlane, vPlaneStride);

            screen.RenderScreenTexture();
        }

        av_packet_unref(&streamPacket);
    }
    else
    {
        stream.CloseStream();
        streamDecoder->Flush();
        streamDecoder->Cleanup();
        stream.Cleanup();
        streamState.store(StreamState::INACTIVE, std::memory_order_release);
        delete streamDecoder;
        streamDecoder = nullptr;

        if(gamepadThread.joinable())
            gamepadThread.join();
    }
};

int main(int argc, char **argv)
{
    std::cout << "Initialising Switch\n";
    initialiseSwitch(); 
    streamState = { StreamState::INACTIVE };
    
    ScreenRenderer screen;
    std::cout << "Initialising Screen\n";
    bool initOK = screen.Initialise(1280, 720, false);

    if(!initOK)
    {
        //send a copy to the nxlink server
        auto errorMessage = "Failed to initialise screen renderer...";
        std::cout << errorMessage << std::endl;
        
        //doesn't display for now on the switch screen from nxlink redirecting stdio
        consoleInit(NULL);
        std::cout << errorMessage << std::endl;
        
        consoleUpdate(NULL);
        int countdown = 3;
        while(countdown > 0)
        {
            --countdown;
            std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1000));
            consoleUpdate(NULL);
        }

        consoleExit(NULL);
        
        CleanupSystem();
        return -1;
    }

    std::cout << "Initialising Network Discovery\n";
    NetworkDiscovery network {handshakePort, subnet, broadcastPort};
    
    std::thread gamepadThread{};
    
    std::cout << "Initialising PcmStream\n";
    PcmStream audioStream {audioPort};
    std::cout << "Initialising Video Stream\n";
    VideoStream stream{};

    StreamDecoder* streamDecoder {nullptr};
    AVPacket streamPacket{};
    auto rendererScreenTexture = screen.GetScreenTexture();
    auto renderRegion = screen.Region();
    
    std::cout << "Loading System Fonts\n";
    auto systemFont = LoadSystemFont(screen.Renderer(), fontSize, white);
    
    auto runApp {true};
    std::cout << "Starting main loop\n";

    //initialise hid
    PadState mainPad {0};
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&mainPad);

    std::cout << "Creating menu selection screens\n";
    auto menuScreens = MenuSelection{};
    
    while(appletMainLoop() && runApp)
    {
        switch(streamState.load(std::memory_order_acquire))
        {
            case StreamState::INACTIVE:
            {
                screen.ClearScreen(bgCol);

                if(audioStream.Running())
                    audioStream.Shutdown();
        
                padUpdate(&mainPad);

                auto kDown = padGetButtonsDown(&mainPad);

                if(kDown & KEY_PLUS)
                    streamState.store(StreamState::QUIT, std::memory_order_release);
                else if(kDown & KEY_R)
                    streamState.store(StreamState::REQUESTED, std::memory_order_release);

                menuScreens.ProcessInput(mainPad);
                
                if(kDown & KEY_L)
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
            }
            break;

            case StreamState::REQUESTED:
            {
                //display on the screen a connection is pending
                screen.ClearScreen(pendingStreamBgCol);
                menuScreens.RenderTitle(screen.Renderer(), systemFont);
                menuScreens.RenderPendingConnection(screen.Renderer(), systemFont);
                menuScreens.RenderNetworkStatus(screen.Renderer(), systemFont, network);
                
                screen.PresentScreen();

                if(network.HostFound() || menuScreens.UseManualIP())
                {
                    auto ip = std::string{};
                    if(menuScreens.UseManualIP())
                        ip = menuScreens.GetManualIPAddress();
                    else
                        ip = network.IPAddress();
                  
                    auto ffmpegConfig = menuScreens.GetFfmpegSettings();
                    auto controllerConfig = menuScreens.GetControllerSettings();
                    auto configfile = Configuration{};
                    configfile.SaveFFMPEG(ffmpegConfig);
                    configfile.SaveController(controllerConfig);
                    RunStartConfiguredStreamCommand(ip, hostCommandPort, ffmpegConfig, controllerConfig);
                    auto streamOn = stream.WaitForStream(menuScreens.GetDecoderSettings(), videoPort);

                    if(streamOn)
                    {
                        auto streamInfo = stream.StreamInfo();
                        if(streamDecoder != nullptr)
                            delete streamDecoder;

                        streamDecoder = new StreamDecoder(streamInfo->codecpar, false);
                        gamepadThread = std::thread(RunGamepadThread, ip, gamepadPort);
                        streamState.store(StreamState::ACTIVE, std::memory_order_release);

                        if(audioStream.Ready() && !audioStream.Running())
                            audioStream.Start();
                    }
                }
                else // no host to connect to
                {
                    std::this_thread::sleep_for(oneSecond);
                    streamState.store(StreamState::INACTIVE, std::memory_order_release);
                }
            }
            break;

            case StreamState::ACTIVE:
            {
                processStream(stream, streamPacket, streamDecoder, 
                                rendererScreenTexture, renderRegion, 
                                screen, gamepadThread);
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

    SDL_Quit();

    //wait here if the stream is still on so we can clean it up properly
    if(streamState.load() == StreamState::ACTIVE)
    {
        while(streamState.load() == StreamState::ACTIVE)
            std::this_thread::sleep_for(thirtyThreeMs);
        stream.Cleanup();

        if(streamDecoder != nullptr)
        {
            streamDecoder->Cleanup();
            delete streamDecoder;
            streamDecoder = nullptr;
        }
    }
    
    network.Shutdown();

    if(gamepadThread.joinable())
        gamepadThread.join();

    if(audioStream.Running())
        audioStream.Shutdown();
    
    auto libnxRes = audoutStopAudioOut();
    if(!R_SUCCEEDED(libnxRes))
        std::cout << "Failed to call audoutStopAudioOut with result: " << libnxRes << std::endl;
    FreeFont(systemFont);
    CleanupSystem();
    
    return 0;
}