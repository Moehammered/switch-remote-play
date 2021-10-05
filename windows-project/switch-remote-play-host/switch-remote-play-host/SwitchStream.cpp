#include "SwitchStream.h"

#include <processthreadsapi.h>
#include "SwitchControlsDefinitions.h"
#include "X360Controller.h"
#include "DS4Controller.h"
#include "VirtualMouse.h"
#include "VirtualTouch.h"
#include "VirtualKeyboard.h"
#include "Connection.h"
#include "FFMPEGHelper.h"
#include "WSAHelper.h"
#include <memory>
#include <chrono>

namespace
{
    auto constexpr mouseToggleBtnCombo = HidNpadButton_ZL | HidNpadButton_ZR | HidNpadButton_B;
    auto constexpr mouseToggleNano = 3000000000;
    auto constexpr mouseFrameTimeNano = (long long)8e+6;
    auto constexpr maxRetries = 5;
    double constexpr joystickExtent = 0xFFFF / 2;

    auto constexpr keyboardFrameTimeNano = (long long)1.6e+7;
    auto constexpr buttonHeldTime = (long long)3e+8;
    auto constexpr taskManagerHotKey = HidNpadButton::HidNpadButton_Minus;
    //auto constexpr touchkeyboardHotKey = HidNpadButton::HidNpadButton_StickR;
    std::unordered_map<HidNpadButton, uint8_t> const keyboardMap
    {
        { HidNpadButton::HidNpadButton_A, VK_RETURN },
        { HidNpadButton::HidNpadButton_B, VK_BACK },
        { HidNpadButton::HidNpadButton_Up, VK_UP },
        { HidNpadButton::HidNpadButton_Down, VK_DOWN },
        { HidNpadButton::HidNpadButton_Left, VK_LEFT },
        { HidNpadButton::HidNpadButton_Right, VK_RIGHT },
        { HidNpadButton::HidNpadButton_X, VK_ESCAPE },
        { HidNpadButton::HidNpadButton_Y, VK_LWIN }
    };

    std::unordered_map<HidNpadButton, long long> buttonTimers{};

    template<typename numberType>
    std::string BitString(numberType val, int bitCount = 32)
    {
        auto bits = std::string{};

        for (auto i = 0; i < bitCount; ++i)
        {
            auto flag = 1 << i;
            auto bit = flag & val;
            auto bitStr = bit == 0 ? "0" : "1";
            bits = bitStr + bits;
        }

        return bits;
    }

    std::vector<std::unique_ptr<IVirtualController>> createVirtualControllers(controller::ControllerConfig const controllerConfig,
        int count, int& failCount)
    {
        using namespace std;
        auto controllers = vector<unique_ptr<IVirtualController>>(maxControllerCount);
        
        for (auto& controller : controllers)
        {
            switch (controllerConfig.controllerMode)
            {
            default:
            case controller::ControllerMode::X360:
                controller = make_unique<X360Controller>();
                break;

            case controller::ControllerMode::DS4:
                controller = make_unique<DS4Controller>();
                break;
            }

            if (controller->Create())
            {
                controller->MapFaceButtons(controllerConfig.controllerMap);
                controller->MapAnalogAxis(controllerConfig.leftAnalogMap, controllerConfig.rightAnalogMap);
            }
            else
                ++failCount;
        }

        return controllers;
    }

    void ReleaseMouseButtons(VirtualMouse& mouse)
    {
        mouse.Release(SupportedMouseButtons::Left);
        mouse.Commit();
        mouse.Release(SupportedMouseButtons::Right);
        mouse.Commit();
        mouse.Release(SupportedMouseButtons::Middle);
        mouse.Commit();
    }

    void ProcessMouseMovement(VirtualMouse& mouse, 
        GamepadDataPayload const& padData, 
        int16_t mouseSensitivity,
        controller::AnalogStick mouseWheelAnalog)
    {
        auto cursorAnalogX = 0;
        auto cursorAnalogY = 0;
        auto mouseWheelAnalogX = 0;
        auto mouseWheelAnalogY = 0;

        if (mouseWheelAnalog == controller::AnalogStick::Right)
        {
            cursorAnalogX = padData.ljx;
            cursorAnalogY = padData.ljy;
            mouseWheelAnalogX = padData.rjx;
            mouseWheelAnalogY = padData.rjy;
        }
        else
        {
            cursorAnalogX = padData.rjx;
            cursorAnalogY = padData.rjy;
            mouseWheelAnalogX = padData.ljx;
            mouseWheelAnalogY = padData.ljy;
        }

        auto x = cursorAnalogX % (int)joystickExtent;
        auto y = (-cursorAnalogY) % (int)joystickExtent;
        auto normalisedX = (long)(mouseSensitivity * (x / joystickExtent));
        auto normalisedY = (long)(mouseSensitivity * (y / joystickExtent));
        mouse.Move(normalisedX, normalisedY);

        auto const maxScrollSpeed = 0.5 * WHEEL_DELTA;
        auto normalisedScrX = mouseWheelAnalogX / joystickExtent;
        auto normalisedScrY = mouseWheelAnalogY / joystickExtent;
        auto scrDeltaX = (int)(normalisedScrX * maxScrollSpeed);
        auto scrDeltaY = (int)(normalisedScrY * maxScrollSpeed);
        mouse.ScrollWheel(scrDeltaX, scrDeltaY);
    }

    void ProcessMouseButtons(VirtualMouse& mouse,
        GamepadDataPayload const& padData,
        HidNpadButton leftClickBtn,
        HidNpadButton rightClickBtn,
        HidNpadButton middleClickBtn)
    {
        if (padData.keys & leftClickBtn)
            mouse.Press(SupportedMouseButtons::Left);
        else
            mouse.Release(SupportedMouseButtons::Left);

        if (padData.keys & rightClickBtn)
            mouse.Press(SupportedMouseButtons::Right);
        else
            mouse.Release(SupportedMouseButtons::Right);

        if (padData.keys & middleClickBtn)
            mouse.Press(SupportedMouseButtons::Middle);
        else
            mouse.Release(SupportedMouseButtons::Middle);
    }

    bool GamepadHasInput(GamepadDataPayload const& pad)
    {
        return pad.keys != 0x0 || (pad.ljx | pad.ljy | pad.rjx | pad.rjy) != 0;
    }

    void ProcessGamepad(std::unique_ptr<IVirtualController> const & controller, 
        GamepadDataPayload & padData,
        HidNpadButton const homeHeldBtn,
        HidNpadButton const homeBtn,
        long long const homeTriggerTimeNano,
        long long& homeTriggerTimer,
        long long const deltaTime)
    {
        if (GamepadHasInput(padData))
        {
            auto homeTriggerKeys = padData.keys & homeHeldBtn;
            if (homeTriggerKeys == homeHeldBtn)
            {
                homeTriggerTimer += deltaTime;
                if (homeTriggerTimer > homeTriggerTimeNano)
                {
                    auto keysCopy = padData.keys & ~homeHeldBtn;
                    padData.keys = keysCopy | homeBtn;
                }
            }
            else
                homeTriggerTimer = 0;

            controller->ConvertPayload(padData);
            //controller->Print();
            controller->UpdateState();
            controller->UpdateController();
        }
        else
        {
            homeTriggerTimer = 0;
            controller->ResetController();
            controller->UpdateController();
        }
    }

    void ProcessVirtualKeyboard(VirtualKeyboard& keyboard,
        GamepadDataPayload const& padData, long long const deltaTime)
    {
        auto pressed = std::vector<uint8_t>{};
        auto released = std::vector<uint8_t>{};

        for (auto const& entry : keyboardMap)
        {
            auto const heldTime = buttonTimers[entry.first];

            if (entry.first & padData.keys)
            {
                if(heldTime == 0 || heldTime > buttonHeldTime)
                    pressed.push_back(entry.second);

                buttonTimers[entry.first] += deltaTime;
            }
            else
            {
                if(heldTime != 0)
                    released.push_back(entry.second);
                buttonTimers[entry.first] = 0;
            }
        }

        if (padData.keys & taskManagerHotKey)
        {
            auto const heldTime = buttonTimers[taskManagerHotKey];
            if (heldTime == 0)
            {
                pressed.push_back(VK_LCONTROL);
                pressed.push_back(VK_LSHIFT);
                pressed.push_back(VK_ESCAPE);
                released.push_back(VK_LCONTROL);
                released.push_back(VK_LSHIFT);
                released.push_back(VK_ESCAPE);
            }
            buttonTimers[taskManagerHotKey] += deltaTime;
        }
        else
            buttonTimers[taskManagerHotKey] = 0;

        keyboard.Press(pressed);
        keyboard.Release(released);
    }
}

CommandPayload ReadPayloadFromSwitch(SOCKET const& switchSocket)
{
    using namespace std;

    cout << "Expected size of command payload: " << COMMAND_PAYLOAD_SIZE << " bytes" << endl;

    CommandPayload data{};
    char* readBuffer = (char*)(&data);
    int retryCount = 3;

    cout << "Reading command..." << endl;

    do
    {
        auto result = recv(switchSocket, readBuffer, COMMAND_PAYLOAD_SIZE, 0);
        if (result == SOCKET_ERROR)
        {
            cout << "Failed to receive data" << endl;
            --retryCount;
            data.commandCode = retryCount <= 0 ? Command::SHUTDOWN : Command::IGNORE_COMMAND;
        }
        else if (result == 0) //once the switch closes the command socket, this will hit
        {
            cout << "breaking as switch closed connection" << endl;
            return data;
        }
        else
            return data;
    } while (retryCount > 0);

    return data;
}

//session resolution needed to clamp and normalise simulated absolute mouse movement
std::thread StartGamepadListener(DisplayDeviceInfo sessionDisplay,
    Resolution switchResolution,
    controller::ControllerConfig controllerConfig, 
    mouse::MouseConfig mouseConfig,
    touch::TouchConfig touchConfig,
    std::atomic_bool& killStream, 
    std::atomic_bool& gamepadActive, 
    uint16_t gamepadPort)
{
    using namespace std;
    thread workerThread{};

    if (gamepadActive.load(memory_order_acquire))
    {
        gamepadActive.store(false, memory_order_release);
    }

    auto const resolutionWidthRatio = sessionDisplay.width / (double)switchResolution.width;
    auto const resolutionHeightRatio = sessionDisplay.height / (double)switchResolution.height;

    workerThread = thread([=, &gamepadActive, &killStream] {

        while (gamepadActive.load(memory_order_acquire)) //don't let it continue if a previous gamepadThread is running
        {
            this_thread::sleep_for(chrono::duration<int, milli>(100)); //wait a bit to let previous gamepad thread cleanup
        }

        auto connection = Connection(gamepadPort);
        if (connection.Ready())
        {
            if (connection.WaitForConnection())
            {
                //can use the switch socket now to listen for input sent over
                InputDataPayload inputData{0};
                for (auto& gamepad : inputData.gamepads)
                {
                    for (auto& c : gamepad.padding)
                        c = 0;
                }

                auto & gamepadPayloads = inputData.gamepads;
                auto const& firstGamepad = gamepadPayloads[0];
                auto const inputDataBuffer = (char*)&inputData;

                auto failedControllerCount = 0;
                auto controllers = createVirtualControllers(controllerConfig, maxControllerCount, failedControllerCount);
                auto controllerCount = controllers.size() < maxControllerCount ? controllers.size() : maxControllerCount;
                if(failedControllerCount == controllers.size())
                {
                    std::cout << "Error creating virtual controllers \n";
                    std::cout << "Error: Virtual Controllers could not be created. Please make sure the Virtual Controller driver is installed correctly.\n";
                    std::cout << "    Stream will shutdown now...\n\n";
                }
                else if (failedControllerCount != 0)
                {
                    std::cout << "Error: Failed to create "<< failedControllerCount << " virtual controllers \n";
                }

                auto keyboard = VirtualKeyboard{};
                auto keyboardPollTime = 0;
                auto mouse = VirtualMouse{};
                auto mousePollTime = 0;
                auto maxFingers = touchConfig.touchMode == touch::TouchScreenMode::VirtualTouch
                    ? touchConfig.virtualTouchSettings.maxFingerCount
                    : touch::MaxFingerCount;
                maxFingers = min(touch::MaxFingerCount, maxFingers);

                auto deadZoneRad = touchConfig.touchMode == touch::TouchScreenMode::VirtualTouch
                    ? touchConfig.virtualTouchSettings.deadzoneRadius
                    : touchConfig.simulatedTouchMouseSettings.deadzoneRadius;
                auto touch = VirtualTouch(deadZoneRad, 2);
                    
                auto streamDead = killStream.load(memory_order_acquire);
                gamepadActive.store(true, memory_order_release);
                auto active = true;
                auto retryCount = maxRetries;

                auto mouseMode{ mouseConfig.mouseOnConnect };
                auto mouseSensitivity{ mouseConfig.mouseSensitivity };

                auto leftClickBtn = mouseConfig.leftClickButton;
                auto rightClickBtn = mouseConfig.rightClickButton;
                auto middleClickBtn = mouseConfig.middleClickButton;
                auto mouseWheelAnalog = mouseConfig.mouseWheelAnalog;

                auto const homeBtn = HidNpadButton::HidNpadButton_Palma;
                auto const homeHeldBtn = HidNpadButton::HidNpadButton_StickL;

                auto const homeTriggerTimeNano = 1000000000LL;
                auto homeTriggerTimers = std::vector<long long>(controllers.size());

                auto lastFrameTime = std::chrono::high_resolution_clock::now();
                auto currentFrameTime = lastFrameTime;

                auto lastTimeMouseToggle = lastFrameTime;
                do
                {
                    active = gamepadActive.load(memory_order_acquire);
                    for (auto& gamepad : gamepadPayloads)
                    {
                        gamepad.keys = 0;
                        gamepad.ljx = gamepad.ljy = 0;
                        gamepad.rjx = gamepad.rjy = 0;
                    }
                    auto bytesRead = 0;
                    do
                    {
                        auto received = recv(connection.ConnectedSocket(), inputDataBuffer + bytesRead, InputDataPayloadSize - bytesRead, 0);
                        if (received == SOCKET_ERROR || received == 0)
                        {
                            bytesRead = received;
                            break;
                        }
                        else
                            bytesRead += received;
                    } while (bytesRead < InputDataPayloadSize);

                    currentFrameTime = chrono::high_resolution_clock::now();
                    auto deltaTime = currentFrameTime - lastFrameTime;

                    if (bytesRead == SOCKET_ERROR)
                    {
                        cout << "Failed to receive data for gamepad stream" << endl;
                        cout << "Winsock error: \n    " << WSAErrorToStr(WSAGetLastError()) << "\n";
                        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1000));
                        --retryCount;
                        if (retryCount == 0)
                        {
                            killStream.store(true, memory_order_release);
                            active = false;
                            streamDead = true;
                        }
                    }
                    else if (bytesRead == InputDataPayloadSize)
                    {
                        if (firstGamepad.keys == 0xFFFF)
                        {
                            cout << "Received kill signal" << endl;
                            killStream.store(true, memory_order_release);
                            active = false;
                            streamDead = true;
                        }
                        else if (mouseMode)
                        {
                            mousePollTime += deltaTime.count();
                            if (mousePollTime >= mouseFrameTimeNano)
                            {
                                ProcessMouseMovement(mouse, firstGamepad, mouseSensitivity, mouseWheelAnalog);
                                ProcessMouseButtons(mouse, firstGamepad, leftClickBtn, rightClickBtn, middleClickBtn);
                                mouse.Commit();
                                mousePollTime = 0;
                            }

                            keyboardPollTime += deltaTime.count();
                            if (keyboardPollTime >= keyboardFrameTimeNano)
                            {
                                ProcessVirtualKeyboard(keyboard, firstGamepad, deltaTime.count());
                                keyboard.Commit();
                                keyboardPollTime = 0;
                            }
                        }
                        else
                        {
                            // process gamepads now
                            for (auto i = 0U; i < controllerCount; ++i)
                            {
                                auto const& controller = controllers[i];
                                auto& padData = gamepadPayloads[i];
                                auto& homeTriggerTimer = homeTriggerTimers[i];
                                
                                ProcessGamepad(controller, padData,
                                    homeHeldBtn, homeBtn,
                                    homeTriggerTimeNano, homeTriggerTimer,
                                    deltaTime.count());
                            }
                        }
                        
                        // figure out if we should toggle mouse mode
                        if (firstGamepad.keys == mouseToggleBtnCombo)
                        {
                            auto timePassed = std::chrono::high_resolution_clock::now() - lastTimeMouseToggle;
                            if (timePassed.count() >= mouseToggleNano)
                            {
                                for (auto& controller : controllers)
                                {
                                    controller->ResetController();
                                    controller->UpdateController();
                                }

                                mouseMode = !mouseMode;

                                if (mouseMode)
                                    MessageBeep(MB_OK);
                                else
                                    MessageBeep(MB_ICONERROR);

                                ReleaseMouseButtons(mouse);

                                lastTimeMouseToggle = std::chrono::high_resolution_clock::now();
                            }
                        }
                        else
                            lastTimeMouseToggle = std::chrono::high_resolution_clock::now();

                        //testing touch stuff here
                        auto const& switchContactInfo = inputData.touchScreen;
                        if (switchContactInfo.count > 0)
                        {
                            auto fingers = std::vector<VirtualFinger>{};
                            for (auto i = 0; i < switchContactInfo.count && i < maxFingers; ++i)
                            {
                                auto finger = switchContactInfo.touches[i];
                                auto reinterp = reinterpret_cast<VirtualFinger*>(&finger);
                                fingers.emplace_back(*reinterp);
                                auto x = fingers.back().x * resolutionWidthRatio;
                                auto y = fingers.back().y * resolutionHeightRatio;
                                fingers.back().x = x + sessionDisplay.x;
                                fingers.back().y = y + sessionDisplay.y;
                            }
                            touch.Press(fingers);
                            touch.Move(fingers);
                            touch.Commit();
                        }
                        else
                        {
                            touch.Release();
                            touch.Commit();
                        }
                        //testing touch stuff here
                    }
                    else if (bytesRead == 0)
                    {
                        // connection closed, cleanup
                        killStream.store(true, memory_order_release);
                        active = false;
                        streamDead = true;
                    }

                    lastFrameTime = currentFrameTime;
                } while (firstGamepad.keys != 0xFFFF && !streamDead && active);

                // make sure there aren't accidental junk key presses when stream dies
                ReleaseMouseButtons(mouse);

                for (auto& controller : controllers)
                {
                    controller->ResetController();
                    controller->UpdateController();
                    controller->Disconnect();
                }
            }
        }

        connection.Close();

        killStream.store(true, memory_order_release);
        gamepadActive.store(false, memory_order_release);
        });

    return workerThread;
}
