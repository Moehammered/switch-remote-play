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
    plInitialize(PlServiceType_User);
    pcvInitialize();

    romfsInit();
}

void initialiseNetwork()
{
    std::cout << "Creating default socket initialisation" << std::endl;
    socketInitializeDefault();
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

int main(int argc, char **argv)
{
    using namespace std;

    // consoleInit(NULL);
    initialiseNetwork();
    initialiseSwitch();
    cout << "creating SDL window" << endl;

    ScreenRenderer screen;
    SDL_Color bgCol;
    bgCol.r = bgCol.g = bgCol.b = bgCol.a = 255;
    bool initOK = screen.Initialise(1280, 720);
    bool streamOK = false;

    string url = "tcp://0.0.0.0:2222";
    VideoStream stream;
    if(initOK)
    {
        nxlinkStdio();
        cout << "SDL initialised. Console output will now be redirected to the server (nxlink.exe)" << endl;    
        cout << "Attempting to create video stream." << endl;
        stream.Initialise();
        streamOK = stream.WaitForStream(url);
        if(streamOK)
        {
            cout << "Connection established, ready to stream." << endl;
        }
        else
        {
            cout << "Connection couldn't be established. From timeout or error." << endl;
        }
    }
    else
    {
        consoleInit(NULL);
    }
    
    //speakToServer("gday mate");
    // Main loop
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
            bgCol.g = 255;
            bgCol.b = 100;
        }
        if (kDown & KEY_B) {
            cout << "B pressed - making bg colour blue" << endl;
            bgCol.r = 100;
            bgCol.g = 100;
            bgCol.b = 255;
        }
        if(kDown & KEY_X && !streamOK)
        {
            streamOK = stream.WaitForStream(url);
        }
        if(!initOK)
            consoleUpdate(NULL);
        else
        {
            screen.ClearScreen(bgCol);

            if(streamOK)
            {
                stream.StreamVideo(screen);
                //blocks here if we proceed, cleanup so we can re-connect properly if we press 'X'
                streamOK = false;
                stream.Cleanup();
            }
            else
                screen.PresentScreen();
        }
    }

    destroyNetwork();

    cout << "exiting application..." << endl;
    if(!initOK)
        consoleExit(NULL);
    else
        SDL_Quit();

    if(streamOK)
        stream.Cleanup();

    return 0;
}