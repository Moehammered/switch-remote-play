/*
  switch-remote-play
    Remote PC connection focused on allowing PC games to be played on the switch
*/

#define PCM_AUDIO

#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <switch.h>
#include "ScreenRenderer.h"
#include "stream/VideoStream.h"
#include "InputThread.h"
#include "stream/StreamDecoder.h"
#include "ui/Text.h"
#include "ui/FFMPEGConfigUI.h"
#include "system/SystemSetup.h"
#include "network/Broadcast.h"
#include "network/HostFinder.h"
#include "system/FileOperations.h"
#include "system/Configuration.h"
#include "network/NetworkDiscovery.h"
#include "ui/Menu.h"
#include "MainScreen.h"
#include "stream/PcmStream.h"

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

uint32_t constexpr fontSize = 30;

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
    std::cout << "Initialising Text\n";
    SetupMainScreen();

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
    while(appletMainLoop() && runApp)
    {
        switch(streamState.load(std::memory_order_acquire))
        {
            case StreamState::INACTIVE:
            {
                screen.ClearScreen(bgCol);

                if(audioStream.Running())
                    audioStream.Shutdown();
        
                hidScanInput();
                auto kDown = hidKeysDown(CONTROLLER_P1_AUTO);

                if(kDown & KEY_PLUS)
                    streamState.store(StreamState::QUIT, std::memory_order_release);
                if(kDown & KEY_DUP)
                    SelectPrevious();
                else if(kDown & KEY_DDOWN)
                    SelectNext();
                else if(kDown & KEY_R)
                    streamState.store(StreamState::REQUESTED, std::memory_order_release);
                else if(kDown & KEY_ZL)
                    PreviousScreen();
                else if(kDown & KEY_ZR)
                    NextScreen();

                if(kDown & KEY_DRIGHT)
                    IncreaseSetting();
                else if(kDown & KEY_DLEFT)
                    DecreaseSetting();
                
                if(kDown & KEY_L)
                {
                    if(!network.Searching())
                        network.Search();
                }

                RenderMainScreen(screen.Renderer(), systemFont);
                RenderNetworkStatus(screen.Renderer(), systemFont, network);
                screen.PresentScreen();
                
                // no point thrashing the screen to refresh text
                std::this_thread::sleep_for(thirtyThreeMs);
            }
            break;

            case StreamState::REQUESTED:
            {
                //display on the screen a connection is pending
                screen.ClearScreen(pendingStreamBgCol);
                
                RenderPendingConnectionScreen(screen.Renderer(), systemFont);
                RenderNetworkStatus(screen.Renderer(), systemFont, network);
                
                screen.PresentScreen();

                if(network.HostFound())
                {
                    RunStartConfiguredStreamCommand(network.IPAddress(), hostCommandPort, GetFfmpegSettings());
                    auto streamOn = stream.WaitForStream(videoPort);

                    if(streamOn)
                    {
                        auto streamInfo = stream.StreamInfo();
                        if(streamDecoder != nullptr)
                            delete streamDecoder;

                        streamDecoder = new StreamDecoder(streamInfo->codecpar, false);
                        gamepadThread = std::thread(RunGamepadThread, network.IPAddress(), gamepadPort);
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
    
    if(network.Searching())
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