#include "InputThread.h"
#include <switch.h>
#include <iostream>
#include <thread>
// #include <atomic>
#include "network/CommandSender.h"
#include <sys/socket.h>

#include "utils/pad/PadUtility.h"
#include "utils/pad/JoyConUtility.h"

namespace
{
    void updateController(PadState & controller, GamepadDataPayload & payload)
    {
        padUpdate(&controller);

        if(padutility::IsLeftJoyCon(controller))
        {
            controller = joyconutility::TranslateJoyConPad(controller, joyconutility::DefaultLeftJoyConMapping);
        }
        else if(padutility::IsRightJoyCon(controller))
        {
            controller = joyconutility::TranslateJoyConPad(controller, joyconutility::DefaultRightJoyConMapping);
        }

        u32 kHeld = padGetButtons(&controller);
        payload.keys = kHeld;

        auto lStick = padGetStickPos(&controller, padutility::leftAnalogIndex);
        auto rStick = padGetStickPos(&controller, padutility::rightAnalogIndex);

        payload.ljx = lStick.x;
        payload.ljy = lStick.y;
        payload.rjx = rStick.x;
        payload.rjy = rStick.y;

        // if(hidGetHandheldMode())
        //     hidSixAxisSensorValuesRead(&sixaxis, CONTROLLER_P1_AUTO, 1);
        // else
        // hidSixAxisSensorValuesRead(&sixaxis, CONTROLLER_PLAYER_1, 1);
        
        // inputData.accelerometer = sixaxis.accelerometer;
        // inputData.gryo = sixaxis.gyroscope;
    }
}

void RunStartConfiguredStreamCommand(std::string ip, uint16_t port, 
    EncoderConfig const config, 
    controller::ControllerConfig const controllerConfig,
    mouse::MouseConfig const mouseConfig,
    touch::TouchConfig const touchConfig)
{
    int commandSocket = -1;
    if(ConnectTo(ip, port, commandSocket))
    {
        auto streamCommand = Command::START_STREAM;
        auto const payload = CommandPayload{
            .encoderData = config,
            .controllerData = controllerConfig,
            .mouseData = mouseConfig,
            .touchData = touchConfig,
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
        auto controllers = padutility::InitialisePads(maxControllerCount);
        // padConfigureInput(1, HidNpadStyleSet_NpadStandard); //de
        // PadState defaultPad{0}; //de
        // padInitializeDefault(&defaultPad); //de

        HidTouchScreenState touchState {0};
        hidInitializeTouchScreen();

        // HidAnalogStickState lStick{0}; //de
        // HidAnalogStickState rStick{0}; //de

        // SixAxisSensorValues sixaxis{0};

        int const dataSize = InputDataPayloadSize;//sizeof(GamepadDataPayload);
        auto gamepadPayloads = std::vector<GamepadDataPayload>(controllers.size());
        for(auto & payload : gamepadPayloads)
        {
            payload = GamepadDataPayload{};
            for(auto & c : payload.padding)
                c = 0;
        }
        // auto inputData = GamepadDataPayload{0}; //de
        // for(auto& c : inputData.padding) //de
        //     c = 0; //de

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
        auto const controllerCount = controllers.size() < maxControllerCount ? controllers.size() : maxControllerCount;
        auto const payloadBuffer = (char*)&inputPayload;
        while(appletMainLoop())
        {
            std::this_thread::sleep_for(sleepDuration); // sleep a tiny bit between inputs
            now = armTicksToNs(armGetSystemTick());
            delta = (now - last)/NANO_TO_SECONDS;
            last = now;
            
            for(auto i = 0U; i < controllerCount; ++i)
            {
                auto& pad = controllers[i];
                auto& payload = inputPayload.gamepads[i];

                updateController(pad, payload);

                if(i == 0) //player 1's controller / first initialised pad
                {
                    auto kHeld = padGetButtons(&pad);
                    if(kHeld & HidNpadButton_Plus)
                    {
                        quitHeldTime += delta;
                        if(quitHeldTime > quitTimer)
                        {
                            u32 exit = 0xFFFF;
                            payload.keys = exit;
                            send(padSocket, (char*)&payload, dataSize, 0);
                            break;
                        }
                    }
                    else
                    {
                        quitHeldTime = 0;
                    }
                }
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
            }
            else
            {
                inputPayload.touchScreen.count = 0;
                touchPollTime = 0;
            }

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
            for(auto& gamepad : inputPayload.gamepads)
            {
                gamepad.keys = 0;
                gamepad.ljx = gamepad.ljy = 0;
                gamepad.rjx = gamepad.rjy = 0;
            }
        }
    }

    close(padSocket);
}
