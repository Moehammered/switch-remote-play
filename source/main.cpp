/*
  switch-remote-play
    Remote PC connection focused on allowing PC games to played on the switch
*/

#include <string>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>

#include <switch.h>
#include "ScreenRenderer.h"
#include "VideoStream.h"
#include "InputThread.h"

using namespace std;

std::mutex streamOnMutex;
StreamConfigData sconfig;

int loopCount = 0; //just to have a visual on the screen to make sure the thread is alive
const int FFMPEG_STREAM_EXAMPLE_IMPL = 0;
const int PACKET_QUEUE_IMPL = 1;
const int DECODER_IMPL = 2;
const std::string techniqueStrings[] = {
    "ffmpeg example code",
    "packet queuing test",
    "decoupled decoder test(+frameskip)"
};
const int techniqueCount = sizeof(techniqueStrings)/sizeof(techniqueStrings[0]);

void initialiseSwitch(SocketInitConfig& config)
{
    socketInitialize(&config);
    nxlinkStdio();

    auto serviceType = PlServiceType_User;
    cout << "Calling plInitialize with value " << serviceType << endl;
    plInitialize(serviceType); //required to access system resources (font data for example)
}

void destroyNetwork()
{
    cout << "destroying default socket initialisation" << endl;
    socketExit();
}

string BuildDisplayText(stringstream& displayStream, string prependText, StreamConfigData& config)
{
    displayStream.str(string()); //clear the stream
    auto techniqueStr = (sconfig.streamTechnique < techniqueCount && sconfig.streamTechnique > -1 ? techniqueStrings[sconfig.streamTechnique] : "invalid");

    displayStream << "Stream implementation technique: " << techniqueStr << endl;
    displayStream << "Target Framerate: " << sconfig.framerate << endl;

    if(sconfig.streamTechnique == DECODER_IMPL)
        displayStream << "Frameskip enabled? " << (sconfig.useFrameSkip ? "yes" : "no") << endl;

    displayStream << "Loop count: " << loopCount << endl;

    if(config.streamOn)
    {
        config.bgCol.g = 200;
        config.bgCol.b = 50;
        config.bgCol.r = 50;
        displayStream << endl << "Waiting for connection... (Use 'Home' to quit if frozen for too long)" << endl;

        return displayStream.str();
    }
    
    stringstream displayMessage;
    displayMessage << "Hello there! \\(^_^) Welcome to switch-remote-play!" << endl;
    displayMessage << prependText << endl;
    displayMessage << endl;
    displayMessage << displayStream.str() << endl;

    return displayMessage.str();
}

int main(int argc, char **argv)
{
    string url = "tcp://0.0.0.0:2222";

    SocketInitConfig socketConfiguration = {
        .bsdsockets_version = 1,

        .tcp_tx_buf_size = 0x80000,
        .tcp_rx_buf_size = 0x100000,
        .tcp_tx_buf_max_size = 0x300000,
        .tcp_rx_buf_max_size = 0x500000,

        .udp_tx_buf_size = 0x1400,
        .udp_rx_buf_size = 0x2500,

        .sb_efficiency = 3,
    };

    initialiseSwitch(socketConfiguration);
    cout << "Console output will now be redirected to the server (nxlink.exe)" << endl;    
    cout << "basic switch services initialised" << endl;

    sconfig.bgCol = {255, 255, 255, 255};
    sconfig.textColour = { 50, 20, 50, 255 };
    sconfig.framerate = 60;
    sconfig.quitApp = false;
    sconfig.streamOn = false;
    sconfig.streamTechnique = 2;
    sconfig.useFrameSkip = true;

    ScreenRenderer screen;
    
    cout << "creating SDL window" << endl;
    bool initOK = screen.Initialise(1280, 720, 32, false);
    
    VideoStream stream;

    string controlsMessage = "/!\\ Initialisation Failed! /!\\";
    if(initOK)
    {
        cout << "Initialising video stream config" << endl;
        stream.Initialise();

        stringstream defaultMessageStream;
        defaultMessageStream << "Ready to accept a video stream connection."<< endl;
        // defaultMessageStream << "Press 'Minus' to find host PC" << endl;
        defaultMessageStream << "Press d-pad up or down to cycle stream implementations." << endl;
        defaultMessageStream << "Press 'L' to toggle frame skip (for decoupled decoder technique only)" << endl;
        defaultMessageStream << endl;
        defaultMessageStream << "Press 'R' to start stream connection" << endl;
        defaultMessageStream << "(will be unresponsive until a connection to a PC is made)." << endl;

        controlsMessage = defaultMessageStream.str();
        cout << controlsMessage << endl;
    }
    else
    {
        consoleInit(NULL);
    }
    
    stringstream configDisplayString;

    thread inputThread = StartInputThread(sconfig, streamOnMutex);

    while(appletMainLoop())
    {
        loopCount++;

        if(initOK)
        {
            { //lock mutex to check if stream has been requested to start
                lock_guard<mutex> streamGuard(streamOnMutex);
                
                if(sconfig.streamOn)
                {
                    //display on the screen a connection is pending
                    auto displayText = BuildDisplayText(configDisplayString, controlsMessage, sconfig);

                    screen.ClearScreen(sconfig.bgCol);
                    screen.DrawText(displayText, 100, 100, sconfig.textColour);
                    screen.PresentScreen();

                    sconfig.streamOn = stream.WaitForStream(url);
                }
            }
            bool streamActive = false;
            { //update the config to let it know if the stream failed to connect
                lock_guard<mutex> streamGuard(streamOnMutex);
                streamActive = sconfig.streamOn;
            }
            
            if(streamActive)
            {
                auto technique = 0;
                { 
                    lock_guard<mutex> streamGuard(streamOnMutex);
                    technique = sconfig.streamTechnique;
                }
                //StreamVideo blocks. If we proceed beyond it, cleanup so we can re-connect properly if we press 'X'
                switch(technique)
                {
                    case FFMPEG_STREAM_EXAMPLE_IMPL:
                        stream.StreamVideo(screen);
                    break;

                    case PACKET_QUEUE_IMPL:
                        stream.StreamVideoQueue(screen);
                    break;

                    case DECODER_IMPL: //this inside will lock after rendering to check the stream state and if it should close the connection
                        stream.StreamVideoViaDecoder(screen, sconfig, streamOnMutex);
                    break;

                    default:
                        cout << "Unknown stream technique chosen. Killing connection and cleaning up" << endl;
                    break;
                }
                
                {
                    lock_guard<mutex> lock(streamOnMutex);
                    sconfig.streamOn = false; //If PC killed the connection, then we need to make sure we know too
                }
                cout << "Stream ended" << endl;
                stream.Cleanup();
                cout << "Stream cleaned up" << endl;
            }
            else
            { //no stream, so let's display some helpful info
                lock_guard<mutex> streamGuard(streamOnMutex);
                auto displayText = BuildDisplayText(configDisplayString, controlsMessage, sconfig);

                screen.ClearScreen(sconfig.bgCol);
                screen.DrawText(displayText, 100, 100, sconfig.textColour);
                screen.PresentScreen();

                if(sconfig.quitApp)
                    break;
            }
        }
        else
            consoleUpdate(NULL);

        sleep(1); // no point thrashing the screen to refresh text
    }

    destroyNetwork();

    cout << "exiting application..." << endl;
    screen.CleanupFont();

    if(!initOK)
        consoleExit(NULL);
    else
        SDL_Quit();

    {
        lock_guard<mutex> streamGuard(streamOnMutex);
        if(sconfig.streamOn)
            stream.Cleanup();
    }

    if(inputThread.joinable())
        inputThread.join();
    /*
        if plExit is not called, after consecutively opening the application 
        it will cause hbloader to close too with error code for 'max sessions' (0x615)
    */
    plExit(); //close the pl session we made when starting up the app
    return 0;
}