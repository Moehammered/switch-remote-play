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

std::atomic_bool streamOn, streamRequested, quitApp;


int main(int argc, char **argv)
{
    const std::string streamURL = "tcp://0.0.0.0:2222";

    initialiseSwitch(); 
    std::cout << "basic switch services initialised" << std::endl;

    streamRequested = false;
    quitApp = false;
    streamOn = false;
    
    ScreenRenderer screen;
    
    Text heading = {
        .x = 400, .y = 20, .colour = { 100, 200, 100, 255 },
        .value = "Switch Remote Play \\(^.^)/"
    };
    Text controlsText = {
        .x = 100, .y = 60, .colour = { 100, 200, 100, 255 }, 
        .value = "" 
    };
    Text streamPendingText = {
        .x = 100, .y = 600, .colour = { 200, 100, 100, 255 },
        .value = "Stream Pending Connection..." 
    };
    Text hostConnectionText = {
        .x = 100, .y = 250, .colour = { 200, 100, 100, 255 },
        .value = "Host IP: not yet found..."
    };

    std::cout << "creating SDL window" << std::endl;
    bool initOK = screen.Initialise(1280, 720, false);

    std::string controlsMessage = "/!\\ Initialisation Failed! /!\\";
    if(initOK)
    {
        std::cout << "Initialising video stream config" << std::endl;

        std::stringstream defaultMessageStream;
        defaultMessageStream << "Ready to accept a video stream connection."<< std::endl;
        defaultMessageStream << "Press d-pad to cycle stream settings." << std::endl;
        defaultMessageStream << std::endl;
        defaultMessageStream << "Press 'R' to start stream connection" << std::endl;
        defaultMessageStream << "(will be unresponsive until a connection to a PC is made)." << std::endl;

        controlsMessage = defaultMessageStream.str();
        std::cout << controlsMessage << std::endl;

        controlsText.value = controlsMessage;
    }
    else
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

    std::thread inputThread = std::thread([&configRenderer]{
        RunInactiveStreamInput(streamRequested, streamOn, quitApp, configRenderer);
    });
    
    std::thread gamepadThread;
    VideoStream stream;

    StreamDecoder* streamDecoder = nullptr;
    AVPacket streamPacket;
    auto rendererScreenTexture = screen.GetScreenTexture();
    auto renderRegion = screen.Region();
    auto processStream = [&]{
        if(stream.Read(streamPacket) && streamOn.load(std::memory_order_acquire))
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

                // std::cout << "Updating SDL texture" << std::endl;
                SDL_UpdateYUVTexture(rendererScreenTexture, &renderRegion, 
                                    yPlane, yPlaneStride,
                                    uPlane, uPlaneStride, 
                                    vPlane, vPlaneStride);

                screen.RenderScreenTexture();
                
                streamPacket.data += decodedFrame.pkt_size; //think this is unnecessary
                streamPacket.size -= decodedFrame.pkt_size; //think this is unnecessary
            }

            av_packet_unref(&streamPacket);
        }
        else
        {
            stream.CloseStream();
            streamDecoder->Flush();
            streamDecoder->Cleanup();
            stream.Cleanup();
            streamOn = false;
            delete streamDecoder;
            streamDecoder = nullptr;

            if(gamepadThread.joinable())
                gamepadThread.join();
        }
    };

    constexpr SDL_Color bgCol = {20, 20, 20, 255};
    constexpr SDL_Color black = {0,0,0, 255};
    auto systemFont = LoadSystemFont(screen.Renderer(), 32, black);
    
    //30fps refresh rate
    auto const mainSleepDur = std::chrono::duration<int, std::milli>(33);
    
    auto subnet = "192.168.0.255";
    auto const handshakeKey = "let-me-play";
    Connection* cnRef = nullptr;
    auto handshakeThread = std::thread(Handshake, handshakeKey, 20001, std::ref(cnRef));
    sleep(1);
    auto broadcastingThread = std::thread(BroadcastIdentity, handshakeKey, subnet, 20000);

    while(appletMainLoop())
    {
        if(streamRequested.load(std::memory_order_acquire))
        {
            //display on the screen a connection is pending
            SDL_Color pendingStreamCol = { 60, 60, 60, 255 };
            screen.ClearScreen(pendingStreamCol);

            heading.Render(screen.Renderer(), systemFont);
            
            streamPendingText.Render(screen.Renderer(), systemFont);
            
            screen.PresentScreen();

            if(hostFound)
            {
                RunStartConfiguredStreamCommand(foundIP, 20001, configRenderer.Settings());
                streamOn = stream.WaitForStream(streamURL);
                streamRequested = false;
                std::cout << "stream connection found? " << streamOn << std::endl;

                if(streamOn)
                {
                    auto streamInfo = stream.StreamInfo();
                    if(streamDecoder != nullptr)
                        delete streamDecoder;

                    streamDecoder = new StreamDecoder(streamInfo->codecpar, false);
                    std::cout << "making gamepad thread" << std::endl;
                    gamepadThread = std::thread(RunGamepadThread, foundIP, 20002);
                }
            }
            else
            {
                sleep(1);
                streamRequested = false;
            }
        }
        if(streamOn)
        {
            processStream();
        }
        else
        { //no stream, so let's display some helpful info
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
            {
                hostConnectionText.Render(screen.Renderer(), systemFont);
            }

            screen.PresentScreen();

            if(quitApp)
                break;
            // no point thrashing the screen to refresh text
            std::this_thread::sleep_for(mainSleepDur);
        }
    }

    std::cout << "exiting application..." << std::endl;

    if(!initOK)
        consoleExit(NULL);
    else
        SDL_Quit();

    //wait here if the stream is still on so we can clean it up properly
    if(streamOn)
    {
        std::cout << "waiting for stream to stop..." << std::endl;
        while(streamOn)
            std::this_thread::sleep_for(mainSleepDur);
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

    hostFound = true;

    if(cnRef != nullptr)
    {
        std::cout << "shutting down handshake connection..." << std::endl;
        if(cnRef->Shutdown())
        {
            std::cout << "handshake connection shutdown" << std::endl;
        }
        else
        {
            std::cout << "failed to shutdown handshake connection" << std::endl;
        }
    }
    
    std::cout << "Joining broadcaster thread... " << foundIP << std::endl;
    if(broadcastingThread.joinable())
        broadcastingThread.join();
    std::cout << "Joining handshake thread... " << foundIP << std::endl;
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