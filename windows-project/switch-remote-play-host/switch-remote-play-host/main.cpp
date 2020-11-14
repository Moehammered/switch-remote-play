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
    std::cout << "Switch Remote Play Host \\(^.^)/" << std::endl;

    WSADATA wsaStateData;
    WSAStartup(MAKEWORD(2, 2), &wsaStateData);

    std::string switchIP{};
    std::atomic_bool ipFound{ false };
    auto subnet = "192.168.0.255";
    auto port = 20000;
    auto handshakePort = 19999;
    Connection* handshakeConnection = nullptr;
    Broadcast* broadcastConnection = nullptr;

    auto handshakeProcedure = [&] {
        auto const replyKey = std::string{ "let-me-play" };

        //handshake method
        auto handshake = Connection(handshakePort);
        handshakeConnection = &handshake;

        if (ipFound && handshake.Ready())
        {
            std::cout << "Connecting to: " << switchIP << std::endl;
            if (handshake.ConnectTo(switchIP))
            {
                if (send(handshake.ConnectedSocket(), replyKey.c_str(), replyKey.length(), 0))
                    std::cout << "Sent reply to switch: " << replyKey << std::endl;
                else
                    std::cout << "Failed to send to switch: " << WSAGetLastError() << std::endl;
            }
        }
        handshake.Close();
        handshakeConnection = nullptr;
    };

    auto receiverProcedure = [&] {
        auto broadcaster = Broadcast(subnet, port);
        broadcastConnection = &broadcaster;

        if (broadcaster.ReadyToRecv())
        {
            auto const replyKey = std::string{ "let-me-play" };
            auto const waitTime = std::chrono::duration<int, std::milli>(400);
            while (broadcaster.ReadyToRecv())
            {
                auto receivedKey = std::string{};
                if(!broadcaster.Recv(receivedKey))
                    std::cout << "Error recv'ing: " << WSAGetLastError() << std::endl;
                else if(receivedKey == replyKey)
                {
                    switchIP = broadcaster.ReceivedIP();
                    ipFound = true;

                    handshakeProcedure();
                }
                else if (receivedKey != replyKey)
                    std::cout << "key didn't match. received: " << receivedKey << std::endl;

                std::this_thread::sleep_for(waitTime*2);
            }

            broadcaster.Close();
            broadcastConnection = nullptr;
        }
    };

    auto connectionDiscoveryThread = std::thread(receiverProcedure);

    // if beyond this point then we should have the switch IP and we've given
    // the host IP to the switch.
    // (But now that I've gotten it working I realise the host doesn't need to switch's IP...)

    std::atomic<bool> killStream = false;
    std::atomic<bool> gamepadActive = false;
    auto lastCommand = Command::IGNORE_COMMAND;
    auto lastPayload = CommandPayload{};
    auto commandPort = 20001;

    PROCESS_INFORMATION streamProcessInfo;
    ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
    std::thread gamepadThread;

    do
    {
        killStream.store(false, std::memory_order_release);

        auto connection = Connection(commandPort);
        if (connection.Ready())
        {
            std::cout << "Ready to receive a connection from the switch..." << std::endl << std::endl;
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
                std::cout << "Shutdown host PC (Not implemented)" << std::endl;
                TerminateProcess(streamProcessInfo.hProcess, 1);

                break;

            case Command::START_STREAM:
                TerminateProcess(streamProcessInfo.hProcess, 1);

                std::cout << "Start stream with last received config from switch..." << std::endl;
                std::cout << "FFMPEG Configuration: " << std::endl << ConfigToString(lastPayload.configData) << std::endl;
                streamProcessInfo = StartStream(lastPayload.configData, ffmpegStarted);
                if (ffmpegStarted)
                {
                    if (handshakeConnection != nullptr)
                        handshakeConnection->Shutdown();

                    gamepadThread = StartGamepadListener(killStream, gamepadActive);
                }

                break;
        }

        // wait here for the stream to change state
        std::cout << "waiting for gamepad thread to shutdown..." << std::endl;
        while (gamepadActive.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(500));

        if (gamepadThread.joinable())
            gamepadThread.join();

        std::cout << "making sure to kill stream..." << std::endl;
        killStream.store(true, std::memory_order_release);
        std::cout << "terminating the FFMPEG process" << std::endl;
        TerminateProcess(streamProcessInfo.hProcess, 1);

    } while (lastCommand != Command::CLOSE_SERVER && lastCommand != Command::SHUTDOWN);

    std::cout << "making sure to kill stream..." << std::endl;
    killStream.store(true, std::memory_order_release);
    std::cout << "terminating the FFMPEG process" << std::endl;
    TerminateProcess(streamProcessInfo.hProcess, 1);

    // wait here for the gamepad to close
    std::cout << "waiting for gamepad thread to shutdown..." << std::endl;
    while (gamepadActive.load(std::memory_order_acquire))
        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(500));

    if (gamepadThread.joinable())
        gamepadThread.join();

    if (handshakeConnection != nullptr)
        handshakeConnection->Shutdown();
    if (broadcastConnection != nullptr)
        broadcastConnection->Shutdown();

    if (connectionDiscoveryThread.joinable())
        connectionDiscoveryThread.join();

    WSACleanup();

    return 0;
}
