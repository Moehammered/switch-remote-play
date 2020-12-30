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

// input scanning needs refactoring for libnx 4.0
void RunGamepadThread(std::string ip, uint16_t port)
{
    int padSocket{-1};
    if(ConnectTo(ip, port, padSocket))
    {
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);
        PadState defaultPad{0};
        padInitializeDefault(&defaultPad);

        HidTouchScreenState touchState {0};
        hidInitializeTouchScreen();

        HidAnalogStickState lStick{0};
        HidAnalogStickState rStick{0};

        SixAxisSensorValues sixaxis{0};

        const int dataSize = sizeof(GamepadDataPayload);
        auto inputData = GamepadDataPayload{0};
        for(auto& c : inputData.padding)
            c = 0;

        const double quitTimer = 3.0;
        const double NANO_TO_SECONDS = 1000000000.0;
        auto now = armTicksToNs(armGetSystemTick());
        auto last = now;
        auto delta = (now - last)/NANO_TO_SECONDS;
        auto quitHeldTime = 0.0;

        auto const sleepDuration = std::chrono::duration<int, std::milli>(10);

        auto retryCount = 10;
        while(appletMainLoop())
        {
            std::this_thread::sleep_for(sleepDuration); // sleep a tiny bit between inputs
            
            padUpdate(&defaultPad);
            now = armTicksToNs(armGetSystemTick());
            delta = (now - last)/NANO_TO_SECONDS;
            last = now;
            
            u32 kHeld = padGetButtons(&defaultPad);

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

            if(hidGetTouchScreenStates(&touchState, 1))
            {
                if(touchState.count > 0)
                    kHeld |= KEY_TOUCH;
            }
            inputData.keys = kHeld;

            lStick = padGetStickPos(&defaultPad, 0);
            rStick = padGetStickPos(&defaultPad, 1);

            inputData.ljx = lStick.x;
            inputData.ljy = lStick.y;
            inputData.rjx = rStick.x;
            inputData.rjy = rStick.y;

            // if(hidGetHandheldMode())
            //     hidSixAxisSensorValuesRead(&sixaxis, CONTROLLER_P1_AUTO, 1);
            // else
            hidSixAxisSensorValuesRead(&sixaxis, CONTROLLER_PLAYER_1, 1);
            
            inputData.accelerometer = sixaxis.accelerometer;
            inputData.gryo = sixaxis.gyroscope;

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
