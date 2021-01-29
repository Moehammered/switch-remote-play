/**
 * Switch-remote-play host application for the PC to stream video and audio to a switch running the Switch-Remote-Play switch app.
 * Relies on ffmpeg.exe being located within the same directory as this executable.
*/

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "SystemCalls.h"
#include "SwitchStream.h"
#include "VirtualController.h"
#include "AtomicTest.h"
#include "Connection.h"
#include "FFMPEGHelper.h"
#include "Broadcast.h"
#include "MasterVolume.h"
#include "NetworkAdapter.h"

auto constexpr applicationVersion = "0.8.1";

PROCESS_INFORMATION streamProcessInfo{ 0 };
PROCESS_INFORMATION audioProcessInfo{ 0 };
Broadcast* switchBroadcastListener{ nullptr };
Connection* switchHandshakeConnection{ nullptr };
Connection* switchCommandListener{ nullptr };

std::string ExtractParentDirectory(const char* executablePath)
{
    std::string fullPath{ executablePath };

    for(auto i = fullPath.size() - 1; i > 0; --i)
    {
        if (fullPath[i] == '\\' || fullPath[i] == '/')
            return fullPath.substr(0, i);
    }

    return fullPath;
}

int main(int argc, char* argv[])
{
    SetParentDirectory(ExtractParentDirectory(argv[0]));

    if (!WinsockReady())
    {
        std::cout << "Failed to initialise windows network sockets. Please check your firewall rules for ports and try again or try restarting your machine.\n";
        Sleep(2000);
        return -1;
    }

    std::cout << "Switch Remote Play Host \\(^.^)/ (PC Application version - " << applicationVersion << ")\n" << std::endl;

    auto initialMonitorSettings = DefaultMonitorInfo();
    auto const initialHeight = initialMonitorSettings.rcMonitor.bottom - initialMonitorSettings.rcMonitor.top;
    auto const initialWidth = initialMonitorSettings.rcMonitor.right - initialMonitorSettings.rcMonitor.left;
    PrintMonitorInfo(initialMonitorSettings);

    auto subnet = std::string{ "192.168.0.255" };
    ScanNetworkConnections(subnet);

    if (VirtualControllerDriverAvailable())
        std::cout << "\n    Virtual Controller driver seems to be installed correctly.\n\n";
    else
    {
        std::cout << "\n\n!!! Virtual Controller driver seems to be having issues. Please make sure it is installed correctly. !!!\n";
        std::cout << "If the problem persists, please try uninstalling the driver and installing the latest virtual controller driver\n\n";
        std::cout << "\n\n==== Press enter to close the program... ====\n\n";

        std::cin.get();
        return -1;
    }
    /*system("pause");
    return 0;*/

    std::string switchIP{};
    std::atomic_bool ipFound{ false };

    uint16_t constexpr handshakePort = 19999;
    uint16_t constexpr broadcastPort = 20000;
    uint16_t constexpr hostCommandPort = 20001;
    uint16_t constexpr gamepadPort = 20002;
    uint16_t constexpr videoPort = 20003;
    uint16_t constexpr audioPort = 20004;

    auto handshakeProcedure = [&] {
        auto const replyKey = std::string{ "let-me-play" };

        //handshake method
        auto handshake = Connection(handshakePort);
        switchHandshakeConnection = &handshake;

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
        switchHandshakeConnection = nullptr;
    };

    auto receiverProcedure = [&] {
        auto broadcaster = Broadcast(subnet, broadcastPort);
        switchBroadcastListener = &broadcaster;

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
            switchBroadcastListener = nullptr;
        }
    };

    auto connectionDiscoveryThread = std::thread(receiverProcedure);

    std::atomic<bool> killStream = false;
    std::atomic<bool> gamepadActive = false;
    auto lastCommand = Command::IGNORE_COMMAND;
    auto lastPayload = CommandPayload{};

    
    ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
    ZeroMemory(&audioProcessInfo, sizeof(audioProcessInfo));
    SetConsoleCtrlHandler(ConsoleWindowEventHandler, TRUE);
    std::thread gamepadThread{};

    auto masterVolume = MasterVolume{};
    auto originalMuteState = masterVolume.IsMuted();

    std::cout << "---- Connection ----\n";
    do
    {
        killStream.store(false, std::memory_order_release);

        auto connection = Connection(hostCommandPort);
        switchCommandListener = &connection;
        if (connection.Ready())
        {
            std::cout << "Ready to receive a connection from the switch..." << std::endl << std::endl;
            if (connection.WaitForConnection())
            {
                lastPayload = ReadPayloadFromSwitch(connection.ConnectedSocket());
                lastCommand = lastPayload.commandCode;

                if (!ipFound)
                {
                    switchIP = connection.ConnectedIP();
                    ipFound = true;
                }
            }
            else
                lastCommand = Command::CLOSE_SERVER;
            connection.Close();
        }

        auto ffmpegStarted = false;
        auto audioStarted = false;
        switch (lastCommand)
        {
            case Command::SHUTDOWN_PC:
                std::cout << "Shutdown host PC (Not implemented)" << std::endl;
                StopStreamProcesses();

                break;

            case Command::START_STREAM:
                StopStreamProcesses();

                if (ipFound)
                {
                    std::cout << "Start stream with last received config from switch..." << std::endl;
                    std::cout << "FFMPEG Configuration: " << std::endl << ConfigToString(lastPayload.configData) << std::endl;
                    ChangeResolution(lastPayload.configData.videoX, lastPayload.configData.videoY);
                    // make sure this function takes in the IP of the switch dynamically from the handshake
                    streamProcessInfo = StartStream(lastPayload.configData, switchIP, videoPort, ffmpegStarted);
                    audioProcessInfo = StartAudio(switchIP, audioPort, audioStarted);

                    if (ffmpegStarted)
                    {
                        if (switchHandshakeConnection != nullptr)
                            switchHandshakeConnection->Shutdown();

                        gamepadThread = StartGamepadListener(lastPayload.controllerData, killStream, gamepadActive, gamepadPort);
#ifdef RELEASE
                        if(IsWindowVisible(GetConsoleWindow()))
                            ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
#endif

                        originalMuteState = masterVolume.IsMuted();
                        masterVolume.Mute(true);
                    }
                }
                else
                    std::cout << "Switch IP has not been found. Please restart the application and switch application and try searching for the host PC on the switch.\n";
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
        StopStreamProcesses();
        
        std::cout << "Resetting resolution" << std::endl;
        ChangeResolution(initialWidth, initialHeight);

        std::cout << "Restoring audio mute state" << std::endl;
        masterVolume.Mute(originalMuteState);

    } while (lastCommand != Command::CLOSE_SERVER && lastCommand != Command::SHUTDOWN);

    std::cout << "making sure to kill stream..." << std::endl;
    killStream.store(true, std::memory_order_release);
    std::cout << "terminating the FFMPEG process" << std::endl;
    StopStreamProcesses();

    // wait here for the gamepad to close
    std::cout << "waiting for gamepad thread to shutdown..." << std::endl;
    while (gamepadActive.load(std::memory_order_acquire))
        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(500));

    if (gamepadThread.joinable())
        gamepadThread.join();

    if (switchHandshakeConnection != nullptr)
        switchHandshakeConnection->Shutdown();
    if (switchBroadcastListener != nullptr)
        switchBroadcastListener->Shutdown();

    if (connectionDiscoveryThread.joinable())
        connectionDiscoveryThread.join();

    WSACleanup();

    return 0;
}
