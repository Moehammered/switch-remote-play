#include "InputThread.h"
#include <switch.h>
#include <iostream>
#include <thread>
#include <atomic>
#include "network/CommandSender.h"
#include <sys/socket.h>

void RunStopCommandThread(std::string ip, uint16_t port)
{
    int commandSocket = -1;
    if(ConnectTo(ip, port, commandSocket))
    {
        if(SendCode(commandSocket, Command::CLOSE_SERVER))
            std::cout << "Sent Close server command payload" << std::endl;
        else
            std::cout << "Error sending close server command payload" << std::endl;
    }
    close(commandSocket);
}

void RunStartConfiguredStreamCommand(std::string ip, uint16_t port, FFMPEG_Config const config)
{
    int commandSocket = -1;
    if(ConnectTo(ip, port, commandSocket))
    {
        auto streamCommand = Command::START_STREAM;
        auto const payload = CommandPayload{
            .configData = config,
            .commandCode = streamCommand
        };
        
        if(!SendCommandPayload(commandSocket, payload))
            std::cout << "Error sending start payload with configuration" << std::endl;
    }
    close(commandSocket);
}

void RunGamepadThread(std::string ip, uint16_t port)
{
    int padSocket{-1};
    if(ConnectTo(ip, port, padSocket))
    {
        JoystickPosition lJoy;
        JoystickPosition rJoy;
        const int dataSize = sizeof(GamepadDataPayload);
        auto inputData = GamepadDataPayload();
        inputData.keys = 0;
        inputData.ljx = inputData.ljy = 0;
        inputData.rjx = inputData.rjy = 0;
        for(auto& c : inputData.padding)
            c = 0;

        const double quitTimer = 3.0;
        const double NANO_TO_SECONDS = 1000000000.0;
        auto now = armTicksToNs(armGetSystemTick());
        auto last = now;
        auto delta = (now - last)/NANO_TO_SECONDS;
        auto quitHeldTime = 0.0;

        auto const sleepDuration = std::chrono::duration<int, std::milli>(16);

        auto retryCount = 10;
        while(appletMainLoop())
        {
            std::this_thread::sleep_for(sleepDuration); // sleep a tiny bit between inputs
            hidScanInput();
            now = armTicksToNs(armGetSystemTick());
            delta = (now - last)/NANO_TO_SECONDS;
            last = now;
            //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
            //u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
            u32 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
            if(kHeld & KEY_PLUS)
            {
                quitHeldTime += delta;
                if(quitHeldTime > quitTimer)
                {
                    u32 exit = 0xFFFF;
                    inputData.keys = exit;
                    send(padSocket, (char*)&inputData, dataSize, 0);
                    break;
                }
            }
            else
            {
                quitHeldTime = 0;
            }
            

            inputData.keys = kHeld;

            hidJoystickRead(&lJoy, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);
            hidJoystickRead(&rJoy, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);
            inputData.ljx = lJoy.dx;
            inputData.ljy = lJoy.dy;
            inputData.rjx = rJoy.dx;
            inputData.rjy = rJoy.dy;

            auto result = send(padSocket, (char*)&inputData, dataSize, 0);
            if(result < 0)
            {
                std::cout << "Error sending pad data" << std::endl;
                if(--retryCount < 0)
                {
                    std::cout << "closing gamepad thread due to connection loss." << std::endl;
                    break;
                }
            }

            // reset the input data
            inputData.keys = 0;
            inputData.ljx = inputData.ljy = 0;
            inputData.rjx = inputData.rjy = 0;
        }
    }

    close(padSocket);
}