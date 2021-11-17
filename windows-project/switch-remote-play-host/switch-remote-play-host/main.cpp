/**
 * Switch-remote-play host application for the PC to stream video and audio to a switch running the Switch-Remote-Play switch app.
 * Relies on ffmpeg.exe being located within the same directory as this executable.
*/

#include "SystemCalls.h"
#include "SwitchStream.h"
#include "Connection.h"
#include "FFMPEGHelper.h"
#include "Broadcast.h"
#include "MasterVolume.h"
#include "NetworkAdapter.h"
#include "FfmpegOutputConfiguration.h"
#include "NetworkConfiguration.h"
#include "DisplayDeviceService.h"
#include "VirtualDesktop.h"
#include "Log.h"
#include <iostream>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

auto constexpr applicationVersion = "1.0.0";

PROCESS_INFORMATION streamProcessInfo{ 0 };
PROCESS_INFORMATION audioProcessInfo{ 0 };
Broadcast* switchBroadcastListener{ nullptr };
Connection* switchHandshakeConnection{ nullptr };
Connection* switchCommandListener{ nullptr };

std::string ExtractParentDirectory(const char* executablePath)
{
    std::string fullPath{ executablePath };

    for (auto i = fullPath.size() - 1; i > 0; --i)
    {
        if (fullPath[i] == '\\' || fullPath[i] == '/')
            return fullPath.substr(0, i);
    }

    return fullPath;
}

int main(int argc, char* argv[])
{
    SetParentDirectory(ExtractParentDirectory(argv[0]));

    auto logger = Log(std::wcout, LogImportance::Low);
    auto const stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    auto const defaultColour = 0 | FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN;

    if (!SocketsReady())
    {
        SetConsoleTextAttribute(stdHandle, FOREGROUND_RED);

        auto message = "Failed to initialise windows network sockets. Please check your firewall rules for ports and try again or try restarting your machine.\n";
        logger.Write(message, LogImportance::High, true);
        Sleep(2000);
        return -1;
    }

    {
        auto appver = transformString(applicationVersion);
        auto welcomeMessage = "Switch Remote Play Host \\(^.^)/ (PC Application version - " + appver + ")\n\n";
        logger.Write(welcomeMessage, LogImportance::High);
    }
    
    StartupTouchContext(logger);

    auto initialMonitorSettings = DefaultMonitorInfo();
    auto const initialHeight = initialMonitorSettings.rcMonitor.bottom - initialMonitorSettings.rcMonitor.top;
    auto const initialWidth = initialMonitorSettings.rcMonitor.right - initialMonitorSettings.rcMonitor.left;
    SetConsoleTextAttribute(stdHandle, FOREGROUND_GREEN);
    {
        auto tempDispServ = DisplayDeviceService{};
        logger << LogImportance::Low;
        logger << "\n---- Monitor ----\n";
        logger << "Printing active display devices found...\n";
        tempDispServ.PrintDisplays(logger);
    }
    
    auto broadcastAddress = std::string{ "192.168.0.255" };
    SetConsoleTextAttribute(stdHandle, FOREGROUND_GREEN | FOREGROUND_RED);
    logger << LogImportance::High;
    logger << "\n---- Network ----\n";
    logger << "Looking for active network interfaces...\n\n";
    if (ScanNetworkConnections(broadcastAddress, logger))
    {
        SetConsoleTextAttribute(stdHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

        logger << "Broadcast address for network discovery is: " << transformString(broadcastAddress) << "\n\n";
        logger << "Please add the line below to the switch application's config file if you wish to use the network discovery feature:\n";
        logger << "Broadcast Address=" << transformString(broadcastAddress) << "; <-- Do not forget the semi-colon! (;)\n\n";
        logger << "The switch configuration file is located at 'switch/switch-remote-play/network.ini'. If it doesn't exist, create it.\n";
    }
    else
    {
        SetConsoleTextAttribute(stdHandle, FOREGROUND_RED);
        logger << LogImportance::High;
        logger << "Couldn't determine broadcast address. If you cannot connect, please use Manual IP Mode in the Switch app.\n";
    }

    SetConsoleTextAttribute(stdHandle, FOREGROUND_GREEN);
    logger << LogImportance::Low;
    if (VirtualControllerDriverAvailable(logger))
        logger << "\n    Virtual Controller driver seems to be installed correctly.\n\n";
    else
    {
        SetConsoleTextAttribute(stdHandle, FOREGROUND_RED);
        logger << LogImportance::High;
        logger << "\n\n!!! Virtual Controller driver seems to be having issues. Please make sure it is installed correctly. !!!\n";
        logger << "If the problem persists, please try uninstalling the driver and installing the latest virtual controller driver\n\n";
        logger << "\n\n==== Press enter to close the program... ====\n\n";

        std::cin.get();
        return -1;
    }
    SetConsoleTextAttribute(stdHandle, defaultColour);
    logger << LogImportance::Low;

    std::string switchIP{};
    std::atomic_bool ipFound{ false };

    auto const startupNetworkSettings = NetworkConfiguration().Data();

    auto handshakeProcedure = [&] {
        auto const replyKey = std::string{ "let-me-play" };

        //handshake method
        auto handshake = Connection(startupNetworkSettings.handshakePort);
        switchHandshakeConnection = &handshake;
        logger.Write("starting handshake...\n", LogImportance::Low);

        if (ipFound && handshake.Ready())
        {
            logger << "Connecting to: " << transformString(switchIP) << std::endl;
            if (handshake.ConnectTo(switchIP))
            {
                auto response = send(handshake.ConnectedSocket(), replyKey.c_str(), replyKey.length(), 0);
                if (response != replyKey.length())
                {
                    auto const errNo = std::to_string(WSAGetLastError());
                    logger.Write("Failed to send to switch: " + errNo + "\n", LogImportance::High, true);
                }
                else
                    logger.Write("Sent reply to switch: " + transformString(replyKey) + "\n", LogImportance::Low);
            }
        }
        handshake.Close();
        switchHandshakeConnection = nullptr;
    };

    auto receiverProcedure = [&] {
        auto broadcaster = Broadcast(broadcastAddress, startupNetworkSettings.broadcastPort);
        switchBroadcastListener = &broadcaster;

        if (broadcaster.ReadyToRecv())
        {
            logger.Write("waiting to receive a broadcast...\n", LogImportance::Low);
            auto const replyKey = std::string{ "let-me-play" };
            auto const waitTime = std::chrono::duration<int, std::milli>(400);
            while (broadcaster.ReadyToRecv())
            {
                auto receivedKey = std::string{};
                if (!broadcaster.Recv(receivedKey))
                {
                    auto const errNo = std::to_string(WSAGetLastError());
                    logger.Write("Error recv'ing: " + errNo + "\n", LogImportance::High, true);
                }
                else if (receivedKey == replyKey)
                {
                    switchIP = broadcaster.ReceivedIP();
                    ipFound = true;

                    handshakeProcedure();
                }
                else if (receivedKey != replyKey)
                {
                    auto errorMessage = "Handshake failed. Key didn't match. Received: " + transformString(receivedKey) + "\n";
                    logger.Write(errorMessage, LogImportance::High);
                }

                std::this_thread::sleep_for(waitTime * 2);
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

    logger << LogImportance::Low;
    logger << "---- Connection ----\n";
    do
    {
        killStream.store(false, std::memory_order_release);

        auto connection = Connection(startupNetworkSettings.commandPort);
        switchCommandListener = &connection;
        if (connection.Ready())
        {
            if (connection.WaitForConnection())
            {
                lastPayload = ReadPayloadFromSwitch(connection.ConnectedSocket(), logger);
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

        auto muteOnConnect = false;
        auto ffmpegStarted = false;
        auto audioStarted = false;
        auto displayService = DisplayDeviceService(true);
        auto displays = displayService.ActiveDisplays();
        auto const defaultDisplayInfo = DisplayDeviceInfo{ 0, 0, 0, initialWidth, initialHeight };
        auto currentDisplay = defaultDisplayInfo;

        switch (lastCommand)
        {
        case Command::SHUTDOWN_PC:
            logger.Write("\nShutdown host PC (Not implemented)\n", LogImportance::Low);
            StopStreamProcesses();

            break;

        case Command::START_STREAM:
            StopStreamProcesses();

            if (ipFound)
            {
                logger.Write("Start stream with last received config from switch...\n\n", LogImportance::Low);
                auto const encoderConfigData = lastPayload.encoderData;
                logger.Write("FFMPEG Configuration: \n" + transformString(ConfigToString(encoderConfigData)) + "\n", LogImportance::Low);

                auto desiredDesktopResolution = encoderConfigData.commonSettings.desktopResolution;
                auto const monitorIndex = encoderConfigData.commonSettings.monitorNumber;
                auto currentDesktopResolution = desiredDesktopResolution;
                if (displays.size())
                {
                    auto display = monitorIndex < displays.size() ? displays[monitorIndex] : displays[0];
                    auto resolutionResult = ChangeResolution(display.monitorSystemName, desiredDesktopResolution.width, desiredDesktopResolution.height);
                    if (!ResolutionChangeSuccessful(resolutionResult))
                    {
                        SetConsoleTextAttribute(stdHandle, FOREGROUND_RED);
                        logger.Write(ResolutionChangeResultString(resolutionResult), LogImportance::High, true);
                        SetConsoleTextAttribute(stdHandle, defaultColour);
                        currentDesktopResolution.height = display.height;
                        currentDesktopResolution.width = display.width;
                    }
                    currentDisplay = display;
                    currentDisplay.width = currentDesktopResolution.width;
                    currentDisplay.height = currentDesktopResolution.height;
                }
                else
                {
                    currentDisplay = defaultDisplayInfo;
                    auto resolutionResult = ChangeResolution(desiredDesktopResolution.width, desiredDesktopResolution.height);
                    if (!ResolutionChangeSuccessful(resolutionResult))
                    {
                        SetConsoleTextAttribute(stdHandle, FOREGROUND_RED);
                        logger.Write(ResolutionChangeResultString(resolutionResult), LogImportance::High, true);
                        SetConsoleTextAttribute(stdHandle, defaultColour);
                        currentDesktopResolution.height = defaultDisplayInfo.height;
                        currentDesktopResolution.width = defaultDisplayInfo.width;
                    }
                    currentDisplay.width = currentDesktopResolution.width;
                    currentDisplay.height = currentDesktopResolution.height;
                }

                // make sure this function takes in the IP of the switch dynamically from the handshake
                auto const ffmpegOutputConfig = FfmpegOutputConfiguration{};
                auto const outputSettings = ffmpegOutputConfig.Data();
                streamProcessInfo = StartStream(currentDisplay, 
                                                encoderConfigData, 
                                                switchIP, 
                                                startupNetworkSettings.videoPort, 
                                                outputSettings.showEncoderOutput,
                                                logger,
                                                ffmpegStarted);

                audioProcessInfo = StartAudio(switchIP, 
                                                startupNetworkSettings.audioPort, 
                                                outputSettings.showAudioOutputWindow, 
                                                logger,
                                                audioStarted);

                if (ffmpegStarted)
                {
                    if (switchHandshakeConnection != nullptr)
                        switchHandshakeConnection->Shutdown();

                    auto tempService = DisplayDeviceService{};
                    auto virtualDesktop = MapVirtualDesktop(tempService.ActiveDisplays(), 65535, 65535);

                    gamepadThread = StartGamepadListener(currentDisplay,
                        virtualDesktop,
                        encoderConfigData.commonSettings.switchResolution,
                        lastPayload.controllerData,
                        lastPayload.mouseData,
                        lastPayload.keyboardData,
                        lastPayload.touchData,
                        killStream,
                        gamepadActive,
                        startupNetworkSettings.gamepadPort);
#ifdef RELEASE
                    if (IsWindowVisible(GetConsoleWindow()))
                        ShowWindow(GetConsoleWindow(), SW_MINIMIZE);
#endif
                    if (muteOnConnect)
                    {
                        originalMuteState = masterVolume.IsMuted();
                        masterVolume.Mute(true);
                    }
                }
            }
            else
            {
                auto missingIPMessage = "Switch IP has not been found. Please restart the application and switch application and try searching for the host PC on the switch.\n";
                logger.Write(missingIPMessage, LogImportance::High, true);
            }
            break;
        }

        // wait here for the stream to change state
        logger.Write("Waiting for gamepad thread to shutdown...\n", LogImportance::Low, true);
        while (gamepadActive.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(500));

        if (gamepadThread.joinable())
            gamepadThread.join();

        logger.Write("Making sure to kill stream...\n", LogImportance::Low, true);
        killStream.store(true, std::memory_order_release);

        logger.Write("Terminating the FFMPEG process\n", LogImportance::Low, true);
        StopStreamProcesses();

        logger.Write("Stream stopped\n", LogImportance::Low);

        logger.Write("Resetting resolution\n", LogImportance::Low);
        ChangeResolution(currentDisplay.monitorSystemName, initialWidth, initialHeight);

        logger.Write("Restoring audio mute state\n", LogImportance::Low);
        masterVolume.Mute(originalMuteState);

    } while (lastCommand != Command::CLOSE_SERVER && lastCommand != Command::SHUTDOWN);

    logger.Write("Making sure to kill stream...\n", LogImportance::Low, true);
    killStream.store(true, std::memory_order_release);

    logger.Write("Terminating the FFMPEG process\n", LogImportance::Low, true);
    StopStreamProcesses();

    // wait here for the gamepad to close
    logger.Write("Waiting for gamepad thread to shutdown...\n", LogImportance::Low, true);
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
