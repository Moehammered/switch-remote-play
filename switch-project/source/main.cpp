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

#define USE_NON_BLOCK_STREAM
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

    std::cout << "creating SDL window" << std::endl;
    bool initOK = screen.Initialise(1280, 720, false);

    std::string controlsMessage = "/!\\ Initialisation Failed! /!\\";
    if(initOK)
    {
        std::cout << "Initialising video stream config" << std::endl;

        std::stringstream defaultMessageStream;
        defaultMessageStream << "Ready to accept a video stream connection."<< std::endl;
        defaultMessageStream << "Press d-pad up or down to cycle stream settings." << std::endl;
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
            //cout << "closing application in " << countdown << endl;
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

#ifdef USE_NON_BLOCK_STREAM
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
            streamDecoder->Cleanup();
            stream.Cleanup();
            streamOn = false;
            delete streamDecoder;
            streamDecoder = nullptr;

            if(gamepadThread.joinable())
                gamepadThread.join();
        }
    };
#endif

    constexpr SDL_Color bgCol = {20, 20, 20, 255};
    constexpr SDL_Color black = {0,0,0, 255};
    auto systemFont = LoadSystemFont(screen.Renderer(), 32, black);
    
    //30fps refresh rate
    auto const mainSleepDur = std::chrono::duration<int, std::milli>(33);

#ifdef USE_AUTO_CONNECT
    std::string hostIP{};
    std::atomic_bool ipFound{ false };
    auto subnet = "192.168.0.255";
    auto port = 20010;
    auto broadcaster = Broadcast(subnet, port);

    auto broadcastProcedure = [&] {
        if (broadcaster.ReadyToSend())
        {
            auto const key = std::string{"let-me-play"};
            auto const waitTime = std::chrono::duration<int, std::milli>(500);
            while (!ipFound)
            {
                if (!broadcaster.Send(key))
                    std::cout << "Error broadcasting: " << strerror(errno) << std::endl;

                std::this_thread::sleep_for(waitTime);
            }

        std::this_thread::sleep_for(waitTime * 2);
        //should replace this with a proper connection to handshake
        if(ipFound && broadcaster.ReadyToRecv())
        {
            auto const startKey = std::string{"lets-go"};
            auto const waitTime = std::chrono::duration<int, std::milli>(500);
            auto lastReceived = std::string{};
            do
            {
                if(!broadcaster.Recv(lastReceived))
                    std::cout << "Handshake Error recv-ing: " << strerror(errno) << std::endl;
                else if(lastReceived != startKey)
                    std::cout << "Handshake didn't match" << std::endl;

                std::this_thread::sleep_for(waitTime);

            } while(lastReceived != startKey);

            std::cout << "we're ready to roll" << std::endl;
        }
        else
            std::cout << "broadcaster not ready to receive or ip is missing..." << std::endl;

            // broadcaster.Close();
        }
        else
            std::cout << "Error: broadcaster isn't ready to send" << std::endl;
    };

    auto receiverProcedure = [&] {
        if (broadcaster.ReadyToRecv())
        {
            auto const replyKey = std::string{ "switch-remote-play" };
            auto const waitTime = std::chrono::duration<int, std::milli>(400);
            while (!ipFound)
            {
                auto receivedKey = std::string{};
                if(!broadcaster.Recv(receivedKey))
                    std::cout << "Error recv-ing: " << strerror(errno) << std::endl;
                else if(receivedKey == replyKey)
                {
                    hostIP = broadcaster.ReceivedIP();
                    ipFound = true;
                }
                else if (receivedKey != replyKey)
                {
                    std::cout << "key didn't match. received: " << receivedKey << std::endl;
                }

                std::this_thread::sleep_for(waitTime);
            }
        }
        else
            std::cout << "Error: broadcaster isn't ready to recv" << std::endl;

        // broadcaster.Close();
    };

    auto broadcastThread = std::thread(broadcastProcedure);
    auto receiverThread = std::thread(receiverProcedure);

    std::cout << "Joining: broadcaster" << std::endl;
    if (broadcastThread.joinable())
        broadcastThread.join();
    std::cout << "Joining: receiver" << std::endl;
    if (receiverThread.joinable())
        receiverThread.join();
    broadcaster.Close();

    if(ipFound)
        std::cout << "Found connection: " << hostIP << std::endl;;
#endif

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

            RunStartConfiguredStreamCommand("192.168.0.19", 20001, configRenderer.Settings());
            streamOn = stream.WaitForStream(streamURL);
            streamRequested = false;
            std::cout << "stream connection found? " << streamOn << std::endl;

#ifdef USE_NON_BLOCK_STREAM
            std::cout << "Using Non-blocking call technique for VideoStream" << std::endl;
            // non-blocking test init code here
            if(streamOn)
            {
                auto streamInfo = stream.StreamInfo();
                if(streamDecoder != nullptr)
                    delete streamDecoder;

                streamDecoder = new StreamDecoder(streamInfo->codecpar, false);
                std::cout << "making gamepad thread" << std::endl;
                gamepadThread = std::thread(RunGamepadThread,"192.168.0.19", 20002);
            }
            // non-blocking test init code end
#endif
        }
        if(streamOn)
        {
#ifdef USE_NON_BLOCK_STREAM
            processStream();
#else
            std::cout << "making gamepad thread" << std::endl;
            gamepadThread = thread(RunGamepadThread,"192.168.0.19", 20002);
            std::cout << "entering video decoder" << std::endl;
            //start stream
            stream.StreamVideoViaDecoder(screen, streamOn);
             //If PC killed the connection, then we need to make sure we know too
            streamOn = false;
            streamRequested = false;

            std::cout << "Stream ended" << std::endl;

            stream.Cleanup();
            std::cout << "Stream cleaned up" << std::endl;

            if(gamepadThread.joinable())
                gamepadThread.join();
#endif
        }
        else
        { //no stream, so let's display some helpful info
            screen.ClearScreen(bgCol);
        
            heading.Render(screen.Renderer(), systemFont);
            configRenderer.Render(screen.Renderer(), systemFont);

            controlsText.Render(screen.Renderer(), systemFont);

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

        #ifdef USE_NON_BLOCK_STREAM
        if(streamDecoder != nullptr)
        {
            streamDecoder->Cleanup();
            delete streamDecoder;
            streamDecoder = nullptr;
        }
        #endif
    }
    
    if(inputThread.joinable())
        inputThread.join();

    if(gamepadThread.joinable())
        gamepadThread.join();
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