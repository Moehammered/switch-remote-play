/*
  run this example from nxlink
  nxlink <-a switch_ip> nxlink_stdio.nro -s <arguments>
  -s or --server tells nxlink to open a socket nxlink can connect to.
*/

#include <string>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <unistd.h>

#include <switch.h>
#include "ScreenRenderer.h"
#include "VideoStream.h"
#include "configuration.h"

void initialiseSwitch()
{
    auto serviceType = PlServiceType_User;
    std::cout << "Calling plInitialize with value " << serviceType << std::endl;
    plInitialize(serviceType);
    std::cout << "Calling pcvInitialize" << std::endl;
    pcvInitialize();

    std::cout << "calling romfsInit" << std::endl;
    romfsInit();
}

void initialiseNetwork(SocketInitConfig& config)
{
    // std::cout << "Creating default socket initialisation" << std::endl;
    // socketInitializeDefault();
    socketInitialize(&config);
}

void destroyNetwork()
{
    std::cout << "destroying default socket initialisation" << std::endl;
    socketExit();
}

void speakToServer(std::string message)
{
    sockaddr_in serverAddr;
    unsigned short portNo = 20001;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNo);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.0.19");

    auto clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if(clientSocket < 0)
        std::cout << "error occurred trying to create socket" << std::endl;

    auto result = connect(clientSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr));
    if(result < 0)
        std::cout << "failed to connect to server" << std::endl;

    char msgBuffer[256];
    for(auto& c : msgBuffer)
        c = 0;
    
    result = recv(clientSocket, msgBuffer, 255, 0);
    if(result < 0)
        std::cout << "error receiving data from server" << std::endl;
    else
    {
        std::cout << "received " << result << " bytes. (msg: " << msgBuffer << ")" << std::endl;
    }
    std::string reply = "Thanks mate. I'm on the switch so no BSOD here, just atmos crash dumps.";
    result = send(clientSocket, reply.c_str(), reply.length(), 0);
    if(result < 0)
        std::cout << "failed to send reply" << std::endl;
    else
    {
        std::cout << "Sent " << reply.length() << " bytes." << std::endl;
    }
    
    close(clientSocket);
}

//callback function that can be given to the app to be called on state changes
void AppStateHook(AppletHookType hook, void* param)
{
    std::cout << "Hook Type: " << hook << std::endl;
    if(param != nullptr)
    {
        SDL_Color* col = (SDL_Color*)param;
        col->r = 150;
        col->g = 10;
        col->b = 10;
    }

    if(hook == AppletHookType_OnExitRequest)
        std::cout << "Hook into exit request" << std::endl;

    auto state = appletGetFocusState();
    std::cout << "App state: " << state << std::endl;

    if(hook == AppletHookType_OnFocusState)
    {
        auto state = appletGetFocusState();
        if(state == AppletFocusState_NotFocusedHomeSleep)
        {
            std::cout << "Sleeping now" << std::endl;
        }
    }
}

using namespace std;

int main(int argc, char **argv)
{
    // consoleInit(NULL);

    SocketInitConfig socketConfiguration = {
        .bsdsockets_version = 1,

        .tcp_tx_buf_size = 0x80000,
        .tcp_rx_buf_size = 0x100000,
        .tcp_tx_buf_max_size = 0x300000,
        .tcp_rx_buf_max_size = 0x500000,

        .udp_tx_buf_size = 0x1400,
        .udp_rx_buf_size = 0x2500,

        .sb_efficiency = 3,

        // .serialized_out_addrinfos_max_size = 0x1000,
        // .serialized_out_hostent_max_size = 0x200,
        // .bypass_nsd = false,
        // .dns_timeout = 0,
    };
    initialiseNetwork(socketConfiguration);
    nxlinkStdio();
    cout << "initing switch" << endl;
    //initialiseSwitch(); //crash occurring inside this function but before the stdio calls can be sent over nxlink

    cout << "creating SDL window" << endl;

    ScreenRenderer screen;
    SDL_Color bgCol;
    bgCol.r = bgCol.g = bgCol.b = bgCol.a = 255;
    bool initOK = screen.Initialise(1280, 720, false);
    bool streamOK = false;
    bool useFrameSkip = false;
    //string url = "rtp://0.0.0.0:2222"; //very fast data retrieval - doesn't display correctly (RTP muxer necessary)
    //string url = "udp://0.0.0.0:2222"; //very fast data retrieval - can't keep up (buffer overflow - guessing it's from sockets?)
    string url = "tcp://0.0.0.0:2222";
    VideoStream stream;
    if(initOK)
    {
        cout << "SDL initialised. Console output will now be redirected to the server (nxlink.exe)" << endl;    
        cout << "Initialising video stream config" << endl;
        stream.Initialise();

        cout << "Ready to accept a video stream connection."<< endl;
        cout << "Press 'X' for FFMPEG example implementation." << endl;
        cout << "Press 'Y' for Packet Queuing implementation." << endl;
        cout << "Press 'R' for decoupled Decoder implementation." << endl;
        cout << "Press 'L' to toggle frame skip" << endl;
        // streamOK = stream.WaitForStream(url);
        // if(streamOK)
        // {
        //     cout << "Connection established, ready to stream." << endl;
        // }
        // else
        // {
        //     cout << "Connection couldn't be established. From timeout or error." << endl;
        // }
    }
    else
    {
        consoleInit(NULL);
    }
    
    //speakToServer("gday mate");
    // Main loop
    const double NANO_TO_SECONDS = 1000000000.0;
    uint64_t currTime, prevTime;
    currTime = armTicksToNs(armGetSystemTick());
    prevTime = currTime;
    uint64_t deltaTime = 0;
    int frameCounter = 180;
    
    AppletHookCookie cookie;
    appletHook(&cookie, AppStateHook, (void*)&bgCol);

    AppletFocusState focusState = appletGetFocusState();
    cout << "Focus State Val: " << focusState << endl;
    focusState = appletGetFocusState();
    //suspend home sleep notify will notify the hook when the system changes focus.
    appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleepNotify);
    cout << "Focus State after modification Val: " << focusState << endl;


    // appletLockExit(); //don't let the applet force close till we cleanup (Will force close if cleanup takes longer than 15s)
    // auto request = appletRequestToAcquireSleepLock();
    // cout << "Sleep lock request: " << request << endl;
    int streamTechnique = 0;
    const int FFMPEG_STREAM_EXAMPLE_IMPL = 0;
    const int PACKET_QUEUE_IMPL = 1;
    const int DECODER_IMPL = 2;
    
    while(appletMainLoop())
    {
        //Scan all the inputs. This should be done once for each frame
        hidScanInput();

        // Your code goes here

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break; // break in order to return to hbmenu

        if (kDown & KEY_A) {
            cout << "A pressed - making bg colour orange?" << endl;
            bgCol.r = 255;
            bgCol.g = 200;
            bgCol.b = 100;
        }
        if (kDown & KEY_B) {
            cout << "B pressed - making bg colour blue" << endl;
            bgCol.r = 100;
            bgCol.g = 100;
            bgCol.b = 255;
        }
        if(kDown & KEY_L)
        {
            useFrameSkip = !useFrameSkip;
            if(useFrameSkip) //set a colour to know if frame skip is on or off
            {
                bgCol.r = 220;
                bgCol.g = 40;
                bgCol.b = 220;
            }
            else
            {
                bgCol.r = 40;
                bgCol.g = 220;
                bgCol.b = 40;
            }
        }
        if(kDown & KEY_X && !streamOK)
        {
            streamOK = stream.WaitForStream(url);
            streamTechnique = FFMPEG_STREAM_EXAMPLE_IMPL;
        }
        else if(kDown & KEY_Y && !streamOK)
        {
            streamOK = stream.WaitForStream(url);
            streamTechnique = PACKET_QUEUE_IMPL;
        }
        else if(kDown & KEY_R)
        {
            streamOK = stream.WaitForStream(url);
            streamTechnique = DECODER_IMPL;
        }
        if(!initOK)
            consoleUpdate(NULL);
        else
        {
            if(streamOK)
            {
                //StreamVideo blocks. If we proceed beyond it, cleanup so we can re-connect properly if we press 'X'
                switch(streamTechnique)
                {
                    case FFMPEG_STREAM_EXAMPLE_IMPL:
                        stream.StreamVideo(screen);
                    break;

                    case PACKET_QUEUE_IMPL:
                        stream.StreamVideoQueue(screen);
                    break;

                    case DECODER_IMPL:
                        stream.StreamVideoViaDecoder(screen, useFrameSkip);
                    break;

                    default:
                        cout << "Unknown stream technique chosen. Killing connection and cleaning up" << endl;
                    break;
                }
                streamOK = false;
                stream.Cleanup();
            }
            else
            {
                screen.ClearScreen(bgCol);
                screen.PresentScreen();
            }
        }

        // currTime = armTicksToNs(armGetSystemTick());
        // deltaTime = deltaTime + (currTime - prevTime);
        // prevTime = currTime; 

        // if(--frameCounter == 0)
        // {
        //     frameCounter = 180;
        //     auto frameRate = frameCounter / (deltaTime / NANO_TO_SECONDS);
        //     cout << "Main Loop FPS: " << frameRate << endl;
        //     deltaTime = 0;
        // }
    }

    destroyNetwork();

    cout << "exiting application..." << endl;
    if(!initOK)
        consoleExit(NULL);
    else
        SDL_Quit();

    if(streamOK)
        stream.Cleanup();

    appletUnhook(&cookie);
    
    // appletReleaseSleepLock();
    
    // appletUnlockExit(); //release lock on application
    return 0;
}