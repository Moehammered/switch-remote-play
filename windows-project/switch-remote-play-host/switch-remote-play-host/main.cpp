/**
 * Test console application to demonstrate connecting to a client automatically on the network without inputting IP manually.
 * To test this, run the client application first, then run the server application and the connection will occur.
*/

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SwitchStream.h"
#include "VirtualController.h"
#include "AtomicTest.h"
#include "Connection.h"
#include "FFMPEGHelper.h"

//broadcast a message from this 'server' application to let clients know it's live and ready to get connections
void BroadcastServer()
{
    using namespace std;

    sockaddr_in broadcastAddr;
    USHORT portNo = 20001;
    broadcastAddr.sin_port = htons(portNo);
    broadcastAddr.sin_family = AF_INET;
    // need to find a way to automatically get the (wireless)local area network IP and subnet mask to make the broadcast address
    inet_pton(AF_INET, "192.168.0.255", &broadcastAddr.sin_addr.S_un.S_addr); //broadcast addr of my LAN (gateway 192.168.0.1, subnet mask 255.255.255.0)
    //broadcastAddr.sin_addr.S_un.S_addr = htonl(INADDR_BROADCAST); //broadcast addr for all network IPs (from local network, to ISP network)

    auto broadcastSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    bool socketOption = true;
    int sizeofOption = sizeof(socketOption);
    //must setup the socket to explicitly want to broadcast
    auto result = setsockopt(broadcastSock, SOL_SOCKET, SO_BROADCAST, (char*)&socketOption, sizeofOption);
    if (result == SOCKET_ERROR)
        cout << "Failed to set socket option to broadcast: " << WSAGetLastError() << endl;

    string bcsMsg = "testes";
    cout << "broadcasting..." << endl;
    result = sendto(broadcastSock, bcsMsg.c_str(), bcsMsg.length(), 0, (const sockaddr*)&broadcastAddr, sizeof(broadcastAddr));

    if (result == SOCKET_ERROR)
        cout << "Failed to send on broadcast socket: " << WSAGetLastError() << endl;
    else
        cout << "broadcasted message" << endl;

    cout << "broadcast finished" << endl;

    closesocket(broadcastSock);
}

int main(int argc, char* argv[])
{
    using namespace std;

    cout << "FFMPEG stream starter" << endl;

    WSADATA wsaStateData;
    WSAStartup(MAKEWORD(2, 2), &wsaStateData);

    //BroadcastServer();

    std::atomic<bool> killStream = false;
    std::atomic<bool> gamepadActive = false;
    auto lastCommand = Command::IGNORE_COMMAND;
    auto lastPayload = CommandPayload{};
    auto listeningPort = 20001;

    PROCESS_INFORMATION streamProcessInfo;
    ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
    thread gamepadThread;

    do
    {
        killStream.store(false, memory_order_release);

        auto connection = Connection(listeningPort);
        if (connection.Ready())
        {
            cout << "connection ready" << endl << endl;
            if (connection.WaitForConnection())
            {
                lastPayload = ReadPayloadFromSwitch(connection.ConnectedSocket());
                lastCommand = lastPayload.commandCode;
            }
            connection.Close();
        }

        auto ffmpegStarted = false;
        switch (lastCommand)
        {
            case Command::SHUTDOWN_PC:
                cout << "Shutdown host PC (Not implemented)" << endl;
                TerminateProcess(streamProcessInfo.hProcess, 1);

                break;

            case Command::START_STREAM:
                TerminateProcess(streamProcessInfo.hProcess, 1);

                cout << "Start stream with last received config from switch..." << endl;
                cout << "FFMPEG Configuration: " << endl << ConfigToString(lastPayload.configData) << endl;
                streamProcessInfo = StartStream(lastPayload.configData, ffmpegStarted);
                if(ffmpegStarted)
                    gamepadThread = StartGamepadListener(killStream, gamepadActive);

                break;
        }

        // wait here for the stream to change state
        cout << "waiting for gamepad thread to shutdown..." << endl;
        while (gamepadActive.load(memory_order_acquire))
            this_thread::sleep_for(chrono::duration<int, milli>(500));

        if (gamepadThread.joinable())
            gamepadThread.join();

        cout << "making sure to kill stream..." << endl;
        killStream.store(true, memory_order_release);
        cout << "terminating the FFMPEG process" << endl;
        TerminateProcess(streamProcessInfo.hProcess, 1);

    } while (lastCommand != Command::CLOSE_SERVER && lastCommand != Command::SHUTDOWN);

    cout << "making sure to kill stream..." << endl;
    killStream.store(true, memory_order_release);
    cout << "terminating the FFMPEG process" << endl;
    TerminateProcess(streamProcessInfo.hProcess, 1);

    // wait here for the gamepad to close
    cout << "waiting for gamepad thread to shutdown..." << endl;
    while (gamepadActive.load(memory_order_acquire))
        this_thread::sleep_for(chrono::duration<int, milli>(500));

    if (gamepadThread.joinable())
        gamepadThread.join();

    WSACleanup();

    return 0;
}
