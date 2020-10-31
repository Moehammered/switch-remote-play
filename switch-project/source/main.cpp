/*
  switch-remote-play
    Remote PC connection focused on allowing PC games to played on the switch
*/

#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <atomic>
#include <switch.h>
#include "ScreenRenderer.h"
#include "VideoStream.h"
#include "InputThread.h"
#include "StreamDecoder.h"
#include "Text.h"
#include "FFMPEGConfigUI.h"
#include "SystemSetup.h"
#include "Broadcast.h"
#include "HostFinder.h"

auto constexpr defaultControlMessage = 
"Ready to accept a video stream connection.\n\
Press d-pad to cycle stream settings.\n\n\
Press 'R' to start stream connection.\n\
(will be unresponsive until a connection to a PC is made)";

auto constexpr streamURL = "tcp://0.0.0.0:2222";
auto constexpr handshakeKey = "let-me-play";
auto constexpr subnet = "192.168.0.255";

uint16_t constexpr broadcastPort = 20000;
uint16_t constexpr hostConnectPort = 20001;
uint16_t constexpr gamepadPort = 20002;

SDL_Color constexpr bgCol = {20, 20, 20, 255};
SDL_Color constexpr black = {0,0,0, 255};
SDL_Color constexpr pendingStreamCol = { 60, 60, 60, 255 };
uint32_t constexpr fontSize = 32;

//30fps refresh rate
auto constexpr thirtyThreeMs = std::chrono::duration<int, std::milli>(33);
auto constexpr oneSecond = std::chrono::duration<int, std::milli>(1000);

std::atomic_int32_t streamState;


int main(int argc, char **argv)
{
    initialiseSwitch(); 
    std::cout << "basic switch services initialised" << std::endl;

    streamState = { StreamState::INACTIVE };
    
    ScreenRenderer screen;
    
    Text const heading = {
        .x = 400, .y = 20, .colour = { 100, 200, 100, 255 },
        .value = "Switch Remote Play \\(^.^)/"
    };
    Text const controlsText = {
        .x = 100, .y = 60, .colour = { 100, 200, 100, 255 }, 
        .value = defaultControlMessage
    };
    Text const streamPendingText = {
        .x = 100, .y = 600, .colour = { 200, 100, 100, 255 },
        .value = "Stream Pending Connection..." 
    };
    Text hostConnectionText = {
        .x = 100, .y = 250, .colour = { 200, 100, 100, 255 },
        .value = "Host IP: not yet found..."
    };

    std::cout << "creating SDL window" << std::endl;
    bool initOK = screen.Initialise(1280, 720, false);

    if(!initOK)
    {
        //send a copy to the nxlink server
        auto errorMessage = "Failed to initialise screen renderer...";
        std::cout << errorMessage << std::endl;
        
        plExit();
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
        
        socketExit();
        return -1;
    }

    auto configRenderer = FFMPEGConfigUI();

    std::thread inputThread = std::thread([&] {
        RunInactiveStreamInput(streamState, configRenderer);
    });
    
    std::thread gamepadThread;
    VideoStream stream;

    StreamDecoder* streamDecoder = nullptr;
    AVPacket streamPacket;
    auto rendererScreenTexture = screen.GetScreenTexture();
    auto renderRegion = screen.Region();
    auto processStream = [&]{
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

    auto systemFont = LoadSystemFont(screen.Renderer(), fontSize, black);
    
    Connection* cnRef = nullptr;
    auto handshakeThread = std::thread(Handshake, handshakeKey, hostConnectPort, std::ref(cnRef));
    std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1000));
    auto broadcastingThread = std::thread(BroadcastIdentity, handshakeKey, subnet, broadcastPort);

    auto runApp = true;
    while(appletMainLoop() && runApp)
    {
        switch(streamState.load(std::memory_order_acquire))
        {
            case StreamState::INACTIVE:
            {
                screen.ClearScreen(bgCol);
        
                heading.Render(screen.Renderer(), systemFont);
                configRenderer.Render(screen.Renderer(), systemFont);

                controlsText.Render(screen.Renderer(), systemFont);

                if(hostFound.load(std::memory_order_acquire))
                {
                    hostConnectionText.value = "Host IP: " + foundIP;
                    hostConnectionText.Render(screen.Renderer(), systemFont, heading.colour);
                }
                else
                    hostConnectionText.Render(screen.Renderer(), systemFont);

                screen.PresentScreen();

                // no point thrashing the screen to refresh text
                std::this_thread::sleep_for(thirtyThreeMs);
            }
            break;

            case StreamState::REQUESTED:
            {
                //display on the screen a connection is pending
                screen.ClearScreen(pendingStreamCol);

                heading.Render(screen.Renderer(), systemFont);
                
                streamPendingText.Render(screen.Renderer(), systemFont);
                
                screen.PresentScreen();

                if(hostFound)
                {
                    RunStartConfiguredStreamCommand(foundIP, hostConnectPort, configRenderer.Settings());
                    auto streamOn = stream.WaitForStream(streamURL);
                    std::cout << "stream connection found? " << streamOn << std::endl;

                    if(streamOn)
                    {
                        auto streamInfo = stream.StreamInfo();
                        if(streamDecoder != nullptr)
                            delete streamDecoder;

                        streamDecoder = new StreamDecoder(streamInfo->codecpar, false);
                        std::cout << "making gamepad thread" << std::endl;
                        gamepadThread = std::thread(RunGamepadThread, foundIP, gamepadPort);
                        streamState.store(StreamState::ACTIVE, std::memory_order_release);
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
                processStream();
            }
            break;

            case StreamState::QUIT:
            {
                runApp = false;
            }
            break;
        }
    }

    std::cout << "exiting application..." << std::endl;

    SDL_Quit();

    //wait here if the stream is still on so we can clean it up properly
    if(streamState.load() == StreamState::ACTIVE)
    {
        std::cout << "waiting for stream to stop..." << std::endl;
        while(streamState.load() == StreamState::ACTIVE)
            std::this_thread::sleep_for(thirtyThreeMs);
        std::cout << "cleaning up stream" << std::endl;
        stream.Cleanup();

        if(streamDecoder != nullptr)
        {
            streamDecoder->Cleanup();
            delete streamDecoder;
            streamDecoder = nullptr;
        }
    }
    
    if(inputThread.joinable())
        inputThread.join();

    if(gamepadThread.joinable())
        gamepadThread.join();

    //make sure the network discovery threads don't lock up
    hostFound = true;
    if(cnRef != nullptr)
    {
        std::cout << "shutting down handshake connection..." << std::endl;

        if(cnRef->Shutdown())
            std::cout << "handshake connection shutdown" << std::endl;
        else
            std::cout << "failed to shutdown handshake connection" << std::endl;
    }
    
    std::cout << "Joining broadcaster thread... " << std::endl;
    if(broadcastingThread.joinable())
        broadcastingThread.join();
        
    std::cout << "Joining handshake thread... " << std::endl;
    if(handshakeThread.joinable())
        handshakeThread.join();

    /*
        if plExit is not called, after consecutively opening the application 
        it will cause hbloader to close too with error code for 'max sessions' (0x615)
    */
    plExit();
    FreeFont(systemFont);

    std::cout << "End of main reached" << std::endl;
    socketExit();
    return 0;
}