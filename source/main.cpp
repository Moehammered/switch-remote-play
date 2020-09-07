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

    consoleInit(NULL);
    initialiseNetwork();
    initialiseSwitch();
    cout << "console output will now be redirected to the server (nxlink.exe)" << endl;

    nxlinkStdio();

    cout << "entering main loop" << endl;

    speakToServer("gday mate");
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
            cout << "A pressed" << endl;
        }
        if (kDown & KEY_B) {
            cout << "B pressed" << endl;
        }

        consoleUpdate(NULL);
    }

    destroyNetwork();

    cout << "exiting application..." << endl;
    consoleExit(NULL);
    return 0;
}