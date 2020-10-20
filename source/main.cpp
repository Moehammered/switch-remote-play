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
#include "Text.h"
#include "FFMPEGConfigUI.h"
#include "SystemSetup.h"

using namespace std;

atomic_bool streamOn, streamRequested, quitApp;
atomic_int32_t streamProfile;

int loopCount = 0; //just to have a visual on the screen to make sure the thread is alive

const std::string defaultStreamSettings[] = {
    "Low Latency, 30fps, 5000 kb/s",
    "OK Latency, 60fps, 5000 kb/s",
    "Low Latency, variable fps, 5000 kb/s"
};
const int techniqueCount = sizeof(defaultStreamSettings)/sizeof(defaultStreamSettings[0]);

string StreamTechniqueToStr(STREAM_MODE mode)
{
    stringstream displayStream;
    if(mode < techniqueCount && mode >= 0)
        displayStream << "Stream Profile: " << defaultStreamSettings[mode] << endl;
    else
        displayStream << "Stream Profile: Unknown - " << mode << endl;
        
    return displayStream.str();
}

string LoopCountToStr()
{
    stringstream stream;
    stream << "Loop count: " << loopCount << endl;
    return stream.str();
}

int main(int argc, char **argv)
{
    const string streamURL = "tcp://0.0.0.0:2222";

    initialiseSwitch();
    cout << "Console output will now be redirected to the server (nxlink.exe)" << endl;    
    cout << "basic switch services initialised" << endl;

    streamRequested = false;
    quitApp = false;
    streamOn = false;
    streamProfile = STREAM_MODE::LOW_LATENCY_30_FPS;
    
    ScreenRenderer screen;
    constexpr SDL_Color bgCol = {20, 20, 20, 255};
    
    Text heading = {
        .x = 400, .y = 20, .colour = { 100, 200, 100, 255 },
        .value = "Switch Remote Play \\(^.^)/"
    };
    Text controlsText = {
        .x = 100, .y = 60, .colour = { 100, 200, 100, 255 }, 
        .value = "" 
    };
    Text streamProfileText = { 
        .x = 100, .y = 280, .colour = { 100, 200, 100, 255 }, 
        .value = "" 
    };
    Text streamPendingText = {
        .x = 100, .y = 600, .colour = { 200, 100, 100, 255 },
        .value = "Stream Pending Connection..." 
    };

    cout << "creating SDL window" << endl;
    bool initOK = screen.Initialise(1280, 720, false);
    VideoStream stream;
    string controlsMessage = "/!\\ Initialisation Failed! /!\\";
    if(initOK)
    {
        cout << "Initialising video stream config" << endl;

        stringstream defaultMessageStream;
        defaultMessageStream << "Ready to accept a video stream connection."<< endl;
        defaultMessageStream << "Press d-pad up or down to cycle stream settings." << endl;
        defaultMessageStream << endl;
        defaultMessageStream << "Press 'R' to start stream connection" << endl;
        defaultMessageStream << "(will be unresponsive until a connection to a PC is made)." << endl;

        controlsMessage = defaultMessageStream.str();
        cout << controlsMessage << endl;

        controlsText.value = controlsMessage;
    }
    else
    {
        //send a copy to the nxlink server
        auto errorMessage = "Failed to initialise screen renderer...";
        cout << errorMessage << endl;
        
        plExit();
        //doesn't display for now on the switch screen from nxlink redirecting stdio
        consoleInit(NULL);
        cout << errorMessage << endl;
        
        consoleUpdate(NULL);
        int countdown = 3;
        while(countdown > 0)
        {
            --countdown;
            this_thread::sleep_for(chrono::duration<int, milli>(1000));
            //cout << "closing application in " << countdown << endl;
            consoleUpdate(NULL);
        }

        consoleExit(NULL);
        
        socketExit();
        return -1;
    }
    constexpr SDL_Color black = {0,0,0, 255};
    auto systemFont = LoadSystemFont(screen.Renderer(), 32, black);

    auto configRenderer = FFMPEGConfigUI();

    thread inputThread = thread([&configRenderer]{
        RunInactiveStreamInput(streamRequested, streamOn
        , quitApp, streamProfile, configRenderer);
    });
    //inputThread.detach(); //detach doesn't work and causes crashes
    thread gamepadThread;

    //30fps refresh rate
    auto const mainSleepDur = chrono::duration<int, milli>(33);
    //PrintOutAtomicLockInfo();

    while(appletMainLoop())
    {
        loopCount++;

        if(streamRequested.load(std::memory_order_acquire))
        {
            //display on the screen a connection is pending
            SDL_Color pendingStreamCol = { 60, 60, 60, 255 };
            screen.ClearScreen(pendingStreamCol);

            heading.Render(screen.Renderer(), systemFont);
            
            auto mode = (STREAM_MODE)streamProfile.load(std::memory_order_acquire);

            streamProfileText.value = StreamTechniqueToStr(mode);
            streamProfileText.Render(screen.Renderer(), systemFont);
            streamPendingText.Render(screen.Renderer(), systemFont);
            
            screen.PresentScreen();

            RunStartStreamCommand("192.168.0.19", 20001, mode);
            streamOn = stream.WaitForStream(streamURL);
            
            cout << "stream connection found? " << streamOn << endl;
        }
        if(streamOn)
        {
            cout << "making gamepad thread" << endl;
            gamepadThread = thread(RunGamepadThread,"192.168.0.19", 20002);
            cout << "entering video decoder" << endl;
            //start stream
            stream.StreamVideoViaDecoder(screen, streamOn);
             //If PC killed the connection, then we need to make sure we know too
            streamOn = false;
            streamRequested = false;

            cout << "Stream ended" << endl;

            stream.Cleanup();
            cout << "Stream cleaned up" << endl;

            if(gamepadThread.joinable())
                gamepadThread.join();
        }
        else
        { //no stream, so let's display some helpful info
            screen.ClearScreen(bgCol);
        
            heading.Render(screen.Renderer(), systemFont);
            configRenderer.Render(screen.Renderer(), systemFont);

            controlsText.Render(screen.Renderer(), systemFont);

            auto mode = (STREAM_MODE)streamProfile.load(std::memory_order_acquire);
            streamProfileText.value = StreamTechniqueToStr(mode);
            streamProfileText.Render(screen.Renderer(), systemFont);

            screen.PresentScreen();

            if(quitApp)
                break;
        }
        // no point thrashing the screen to refresh text
        this_thread::sleep_for(mainSleepDur);
    }

    cout << "exiting application..." << endl;

    if(!initOK)
        consoleExit(NULL);
    else
        SDL_Quit();

    //wait here if the stream is still on so we can clean it up properly
    if(streamOn)
    {
        cout << "waiting for stream to stop..." << endl;
        while(streamOn)
            this_thread::sleep_for(mainSleepDur);
        cout << "cleaning up stream" << endl;
        stream.Cleanup();
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

    cout << "End of main reached" << endl;
    socketExit();
    return 0;
}