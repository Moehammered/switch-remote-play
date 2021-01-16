/**
 * Switch-remote-play host application for the PC to stream video and audio to a switch running the Switch-Remote-Play switch app.
 * Relies on ffmpeg.exe being located within the same directory as this executable.
*/

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include "SystemCalls.h"
#include "SwitchStream.h"
#include "VirtualController.h"
#include "AtomicTest.h"
#include "Connection.h"
#include "FFMPEGHelper.h"
#include "Broadcast.h"
#include <stdio.h>
#include <signal.h>
#include <chrono>
#include "Process.h"

auto constexpr applicationVersion = "0.8.1";

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
    
    if(argc == 1)
    {
        std::cout << "Screen index was not specified. Assuming screen index '0'.\n";
        std::cout << "Please use ffmpeg to figure out your screen's device index and then pass that value into switch-remote-play-host as an argument.\n\n";
        
        std::cout << "    run './ffmpeg -f avfoundation -list_devices true -i \"\"'\n";
        std::cout << "    then run './switch-remote-play-host 1' for example if your screen's index is 1\n\n";
        
        SetScreenIndex("0");
    }
    else
    {
        SetScreenIndex(argv[1]);
        std::cout << "Using screen index " << argv[1] << "\n";
    }
    
    auto originalDisplayProperties = GetCurrentDisplayAttributes();
    std::cout << "Screen W,H: " << originalDisplayProperties.width << " x " << originalDisplayProperties.height << "\n\n";

    std::cout << "Switch Remote Play Host \\(^.^)/ (PC Application version - " << applicationVersion << ")\n" << std::endl;

//    int const initialHeight = originalDisplayProperties.height;
//    int const initialWidth = originalDisplayProperties.width;

    std::string switchIP{};
    std::atomic_bool ipFound{ false };
    auto subnet = "192.168.0.255";

    uint16_t constexpr handshakePort = 19999;
    uint16_t constexpr broadcastPort = 20000;
    uint16_t constexpr hostCommandPort = 20001;
    uint16_t constexpr gamepadPort = 20002;
    uint16_t constexpr videoPort = 20003;
//    uint16_t constexpr audioPort = 20004;

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
                    std::cout << "Failed to send to switch: " << std::endl;
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
                    std::cout << "Error recv'ing: " << std::endl;
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

    std::atomic<bool> killStream {false};
    std::atomic<bool> gamepadActive {false};
    auto lastCommand = Command::IGNORE_COMMAND;
    auto lastPayload = CommandPayload{};

    std::thread gamepadThread{};
    Process streamProc{};
    
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
            if(connection.Close())
                std::cout << "Closed command socket\n\n";
            else
                std::cout << "Failed to close command socket!!!\n\n";
        }

        auto ffmpegStarted = false;
//        auto audioStarted = false;
        switch (lastCommand)
        {
            case Command::SHUTDOWN_PC:
                std::cout << "Shutdown host PC (Not implemented)" << std::endl;
                break;

            case Command::START_STREAM:
                
                if(streamProc.Running())
                    streamProc.Stop();

                if (ipFound)
                {
                    std::cout << "Start stream with last received config from switch..." << std::endl;
                    std::cout << "FFMPEG Configuration: " << std::endl << ConfigToString(lastPayload.configData) << std::endl;
                    ChangeResolution(lastPayload.configData.videoX, lastPayload.configData.videoY);
                    // make sure this function takes in the IP of the switch dynamically from the handshake
                    streamProc = StartStream(lastPayload.configData, switchIP, videoPort, ffmpegStarted);
//                    audioProcessInfo = StartAudio(switchIP, audioPort, audioStarted);

                    if (ffmpegStarted)
                    {
                        if (switchHandshakeConnection != nullptr)
                            switchHandshakeConnection->Shutdown();
                        
                        gamepadThread = StartGamepadListener(lastPayload.controllerData, killStream, gamepadActive, gamepadPort);
                    }
                }
                else
                    std::cout << "Switch IP has not been found. Please restart the application and switch application and try searching for the host PC on the switch.\n";
                break;
        }

        // wait here for the stream to change state
        std::cout << "inner loop -- waiting for gamepad thread to shutdown..." << std::endl;
        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(4000));
        while (gamepadActive.load(std::memory_order_acquire))
            std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(2000));

        if (gamepadThread.joinable())
            gamepadThread.join();

        std::cout << "inner loop -- making sure to kill stream..." << std::endl;
        killStream.store(true, std::memory_order_release);
        
        std::cout << "inner loop -- terminating the FFMPEG process" << std::endl;
        streamProc.Stop();
        streamProc.Wait();
        
        
//        std::cout << "inner loop -- Resetting resolution" << std::endl;
//        if(!ChangeResolution(initialWidth, initialHeight))
//            std::cout << "    Failed to reset resolution!!\n\n";


    } while (lastCommand != Command::CLOSE_SERVER && lastCommand != Command::SHUTDOWN);

    std::cout << "ending program -- making sure to kill stream..." << std::endl;
    killStream.store(true, std::memory_order_release);
    std::cout << "ending program -- terminating the FFMPEG process" << std::endl;
    streamProc.Stop();

    // wait here for the gamepad to close
    std::cout << "ending program -- waiting for gamepad thread to shutdown..." << std::endl;
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

    return 0;
}
