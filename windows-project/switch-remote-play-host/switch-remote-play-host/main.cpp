/**
 * Switch-remote-play host application for the PC to stream video and audio to a switch running the Switch-Remote-Play switch app.
 * Relies on ffmpeg.exe being located within the same directory as this executable.
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

auto constexpr applicationVersion = "0.7.1";

MONITORINFOEX DefaultMonitorInfo()
{
    auto monitorPoint = POINT{ 0 };
    monitorPoint.x = 0; monitorPoint.y = 0;
    auto handle = MonitorFromPoint(monitorPoint, MONITOR_DEFAULTTOPRIMARY);

    auto monitorInfo = MONITORINFOEX{};
    monitorInfo.cbSize = sizeof(monitorInfo);

    GetMonitorInfo(handle, &monitorInfo);

    return monitorInfo;
}

bool ChangeResolution(int width, int height)
{
    auto deviceMode = DEVMODE{ 0 };

    deviceMode.dmSize = sizeof(deviceMode);
    deviceMode.dmBitsPerPel = 32;
    deviceMode.dmPelsWidth = width;
    deviceMode.dmPelsHeight = height;
    deviceMode.dmDisplayFrequency = 60;
    deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    auto result = ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
    return result == DISP_CHANGE_SUCCESSFUL;
}

PROCESS_INFORMATION streamProcessInfo{ 0 };
PROCESS_INFORMATION audioProcessInfo{ 0 };
Broadcast* switchBroadcastListener{ nullptr };
Connection* switchHandshakeConnection{ nullptr };
Connection* switchCommandListener{ nullptr };

void StopStreamProcesses()
{
    if (streamProcessInfo.hProcess != nullptr)
    {
        TerminateProcess(streamProcessInfo.hProcess, 1);
        CloseHandle(streamProcessInfo.hProcess);
        CloseHandle(streamProcessInfo.hThread);
        ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
    }
    if (audioProcessInfo.hProcess != nullptr)
    {
        TerminateProcess(audioProcessInfo.hProcess, 1);
        CloseHandle(audioProcessInfo.hProcess);
        CloseHandle(audioProcessInfo.hThread);
        ZeroMemory(&audioProcessInfo, sizeof(audioProcessInfo));
    }
}

BOOL WINAPI ConsoleWindowEventHandler(DWORD eventType)
{
    switch (eventType)
    {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
        case CTRL_CLOSE_EVENT:
            std::cout << "Cleaning up in event handler...\n";
            std::cout << "Terminating video and audio process\n";
            StopStreamProcesses();
            if (switchHandshakeConnection != nullptr)
            {
                std::cout << "terminating handshake connection\n";
                switchHandshakeConnection->Shutdown();
                switchHandshakeConnection->Close();
            }
            if (switchBroadcastListener != nullptr)
            {
                std::cout << "terminating broadcast listener ";
                switchBroadcastListener->Close();
                switchBroadcastListener->Shutdown();
            }
            if (switchCommandListener != nullptr)
            {
                std::cout << "terminating command receiver connection\n";
                switchCommandListener->Close();
                switchCommandListener->Shutdown();
            }
            Sleep(2000);
            return TRUE;

        default:
            return FALSE;
    }
}

bool WinsockReady()
{
    WSADATA wsaStateData;
    auto socketStartup = WSAStartup(MAKEWORD(2, 2), &wsaStateData);

    switch (socketStartup)
    {
        case WSASYSNOTREADY:
            std::cout << "Winsock Error - WSASYSNOTREADY: The underlying network subsystem is not ready for network communication\n";
            return false;

        case WSAVERNOTSUPPORTED:
            std::cout << "Winsock Error - WSAVERNOTSUPPORTED: The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation\n";
            return false;

        case WSAEINPROGRESS:
            std::cout << "Winsock Error - WSAEINPROGRESS: A blocking Windows Sockets 1.1 operation is in progress\n";
            return false;

        case WSAEPROCLIM:
            std::cout << "Winsock Error - WSAEPROCLIM: A limit on the number of tasks supported by the Windows Sockets implementation has been reached\n";
            return false;

        case WSAEFAULT:
            std::cout << "Winsock Error - WSAEFAULT: The lpWSAData parameter is not a valid pointer\n";
            return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    if (!WinsockReady())
    {
        std::cout << "Failed to initialise windows network sockets. Please check your firewall rules for ports and try again or try restarting your machine.\n";
        return -1;
    }

    std::cout << "Switch Remote Play Host \\(^.^)/ (PC Application version - " << applicationVersion << ")" << std::endl;

    auto initialMonitorSettings = DefaultMonitorInfo();
    auto const initialHeight = initialMonitorSettings.rcMonitor.bottom - initialMonitorSettings.rcMonitor.top;
    auto const initialWidth = initialMonitorSettings.rcMonitor.right - initialMonitorSettings.rcMonitor.left;

    std::string switchIP{};
    std::atomic_bool ipFound{ false };
    auto subnet = "192.168.0.255";

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

    // if beyond this point then we should have the switch IP and we've given
    // the host IP to the switch.
    // (But now that I've gotten it working I realise the host doesn't need to switch's IP...)

    std::atomic<bool> killStream = false;
    std::atomic<bool> gamepadActive = false;
    auto lastCommand = Command::IGNORE_COMMAND;
    auto lastPayload = CommandPayload{};

    
    ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
    ZeroMemory(&audioProcessInfo, sizeof(audioProcessInfo));
    SetConsoleCtrlHandler(ConsoleWindowEventHandler, TRUE);
    std::thread gamepadThread;

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

                        gamepadThread = StartGamepadListener(killStream, gamepadActive, gamepadPort);
                        if(IsWindowVisible(GetConsoleWindow()))
                            ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
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
