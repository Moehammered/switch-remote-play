#include "InputThread.h"
#include <switch.h>
#include <iostream>
#include <thread>
#include <atomic>
#include "network/CommandSender.h"
#include <sys/socket.h>

void RunStartConfiguredStreamCommand(std::string ip, uint16_t port, EncoderConfig const config, controller::ControllerConfig const controllerConfig)
{
    int commandSocket = -1;
    if(ConnectTo(ip, port, commandSocket))
    {
        auto streamCommand = Command::START_STREAM;
        auto const payload = CommandPayload{
            .encoderData = config,
            .controllerData = controllerConfig,
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

        // SixAxisSensorValues sixaxis{0};

        const int dataSize = InputDataPayloadSize;//sizeof(GamepadDataPayload);
        auto inputData = GamepadDataPayload{0};
        for(auto& c : inputData.padding)
            c = 0;

        const double quitTimer = 3.0;
        const double NANO_TO_SECONDS = 1000000000.0;
        auto now = armTicksToNs(armGetSystemTick());
        auto last = now;
        auto delta = (now - last)/NANO_TO_SECONDS;
        auto quitHeldTime = 0.0;
        auto const touchPollTimer = 1.0/60.0;
        auto touchPollTime = 0.0;

        auto const sleepDuration = std::chrono::duration<int, std::milli>(5);

        auto retryCount = 10;
        auto inputPayload = InputDataPayload{0};
        auto const payloadBuffer = (char*)&inputPayload;
        while(appletMainLoop())
        {
            std::this_thread::sleep_for(sleepDuration); // sleep a tiny bit between inputs
            
            padUpdate(&defaultPad);
            now = armTicksToNs(armGetSystemTick());
            delta = (now - last)/NANO_TO_SECONDS;
            last = now;
            
            u32 kHeld = padGetButtons(&defaultPad);

            if(kHeld & HidNpadButton_Plus)
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
                touchPollTime += delta;
                if(touchPollTime > touchPollTimer)
                {
                    inputPayload.touchScreen.count = touchState.count;
                    for(auto i = 0; i < touchState.count && i < 5; ++i)
                    {
                        auto const & touch = touchState.touches[i];
                        auto& target = inputPayload.touchScreen.touches[i];
                        target.fingerID = touch.finger_id;
                        target.x = touch.x;
                        target.y = touch.y;
                    }
                    
                    touchPollTime = 0;
                }

                if(touchState.count > 0)
                    kHeld |= HidNpadButton_Palma; // this needs to go
            }
            else
            {
                inputPayload.touchScreen.count = 0;
                touchPollTime = 0;
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
            // hidSixAxisSensorValuesRead(&sixaxis, CONTROLLER_PLAYER_1, 1);
            
            // inputData.accelerometer = sixaxis.accelerometer;
            // inputData.gryo = sixaxis.gyroscope;

            inputPayload.gamepad = inputData;

            auto sent = 0;
            do
            {
                sent = send(padSocket, payloadBuffer, dataSize - sent, 0);
            } while (sent > 0 && sent < dataSize);
            
            if(sent < 0)
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
