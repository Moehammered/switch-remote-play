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

void PrintChangeDisplayStatus(long result)
{
    switch (result)
    {
    case DISP_CHANGE_SUCCESSFUL:
        std::cout << "The settings change was successful" << "\n";
        break;

    case DISP_CHANGE_BADDUALVIEW:
        std::cout << "The settings change was unsuccessful because the system is DualView capable" << "\n";
        break;

    case DISP_CHANGE_BADFLAGS:
        std::cout << "An invalid set of flags was passed in" << "\n";
        break;

    case DISP_CHANGE_BADMODE:
        std::cout << "The graphics mode is not supported" << "\n";
        break;

    case DISP_CHANGE_BADPARAM:
        std::cout << "An invalid parameter was passed in. This can include an invalid flag or combination of flags" << "\n";
        break;

    case DISP_CHANGE_FAILED:
        std::cout << "The display driver failed the specified graphics mode" << "\n";
        break;

    case DISP_CHANGE_NOTUPDATED:
        std::cout << "Unable to write settings to the registry" << "\n";
        break;

    case DISP_CHANGE_RESTART:
        std::cout << "The computer must be restarted for the graphics mode to work" << "\n";
        break;
    }
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

void TestMonitorInfo()
{
    auto monitorInfo = DefaultMonitorInfo();

    std::cout << "Monitor Info: " << "\n";
    if (monitorInfo.dwFlags == MONITORINFOF_PRIMARY)
        std::cout << "    Primary Monitor" << "\n";
    else
        std::cout << "    Secondary Monitor" << "\n";

    auto charRep = std::wstring{ monitorInfo.szDevice };
    auto str = std::string(charRep.begin(), charRep.end());
    std::cout << "    " << str << "\n";
    auto width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    auto height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    std::cout << "    " << width << " x " << height << "\n";
    auto workWidth = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
    auto workHeight = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
    std::cout << "    " << workWidth << " x " << workHeight << "\n";

    auto deviceMode = DEVMODE{ 0 };

    deviceMode.dmSize = sizeof(deviceMode);
    deviceMode.dmBitsPerPel = 32;
    deviceMode.dmPelsWidth = 1280;
    deviceMode.dmPelsHeight = 720;
    deviceMode.dmDisplayFrequency = 60;
    deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    auto result = ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
    if (result == DISP_CHANGE_SUCCESSFUL)
    {
        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(5000));

        deviceMode.dmPelsWidth = width;
        deviceMode.dmPelsHeight = height;

        auto changeBackResult = ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
        PrintChangeDisplayStatus(changeBackResult);
    }
    else
    {
        PrintChangeDisplayStatus(result);
    }
}

int main(int argc, char* argv[])
{
    std::cout << "Switch Remote Play Host \\(^.^)/" << std::endl;

    //TestMonitorInfo();
    auto initialMonitorSettings = DefaultMonitorInfo();
    auto const initialHeight = initialMonitorSettings.rcMonitor.bottom - initialMonitorSettings.rcMonitor.top;
    auto const initialWidth = initialMonitorSettings.rcMonitor.right - initialMonitorSettings.rcMonitor.left;
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
                ChangeResolution(lastPayload.configData.videoX, lastPayload.configData.videoY);
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
        std::cout << "Resetting resolution" << std::endl;
        ChangeResolution(initialWidth, initialHeight);

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
