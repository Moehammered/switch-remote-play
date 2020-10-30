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
#include "Broadcast.h"

int main(int argc, char* argv[])
{
    using namespace std;

    cout << "FFMPEG stream starter" << endl;

    WSADATA wsaStateData;
    WSAStartup(MAKEWORD(2, 2), &wsaStateData);

    std::string switchIP{};
    std::atomic_bool ipFound{ false };
    auto subnet = "192.168.0.255";
    auto port = 20000;
    
    auto receiverProcedure = [&] {
        auto broadcaster = Broadcast(subnet, port);

        if (broadcaster.ReadyToRecv())
        {
            auto const replyKey = std::string{ "let-me-play" };
            auto const waitTime = chrono::duration<int, milli>(400);
            while (!ipFound)
            {
                auto receivedKey = std::string{};
                if(!broadcaster.Recv(receivedKey))
                    cout << "Error recv'ing: " << WSAGetLastError() << endl;
                else if(receivedKey == replyKey)
                {
                    switchIP = broadcaster.ReceivedIP();
                    ipFound = true;
                }
                else if (receivedKey != replyKey)
                {
                    cout << "key didn't match. received: " << receivedKey << endl;
                }

                this_thread::sleep_for(waitTime);
            }
            this_thread::sleep_for(waitTime * 2); // wait a bit to let the other thread die

            //handshake method
            auto handshake = Connection(20001);
            if (ipFound && handshake.Ready())
            {
                std::cout << "Connecting to: " << switchIP << std::endl;
                if (handshake.ConnectTo(switchIP))
                {
                    if (send(handshake.ConnectedSocket(), replyKey.c_str(), replyKey.length(), 0))
                    {
                        std::cout << "Sent reply to switch: " << replyKey << std::endl;
                    }
                    else
                    {
                        std::cout << "Failed to send to switch: " << WSAGetLastError() << std::endl;
                    }
                }
            }
            handshake.Close();
            broadcaster.Close();
        }
    };

    auto connectionDiscoveryThread = thread(receiverProcedure);

    if (connectionDiscoveryThread.joinable())
        connectionDiscoveryThread.join();

    // if beyond this point then we should have the switch IP and we've given
    // the host IP to the switch.
    // (But now that I've gotten it working I realise the host doesn't need to switch's IP...)

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
