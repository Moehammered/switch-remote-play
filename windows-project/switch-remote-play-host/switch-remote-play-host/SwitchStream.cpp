#include "SwitchStream.h"

#include <processthreadsapi.h>
#include "SwitchControlsDefinitions.h"
#include "X360Controller.h"
#include "DS4Controller.h"
#include "VirtualMouse.h"
#include "Connection.h"
#include "FFMPEGHelper.h"
#include "WSAHelper.h"
#include <memory>
#include <chrono>

auto constexpr mouseToggleBtnCombo = HidNpadButton_ZL | HidNpadButton_ZR | HidNpadButton_B;
auto constexpr mouseToggleNano = 3000000000;
auto constexpr maxRetries = 5;
double constexpr joystickExtent = 0xFFFF / 2;

void ReleaseMouseButtons(VirtualMouse& mouse)
{
    mouse.Release(SupportedMouseButtons::Left);
    mouse.Commit();
    mouse.Release(SupportedMouseButtons::Right);
    mouse.Commit();
    mouse.Release(SupportedMouseButtons::Middle);
    mouse.Commit();
}

void ProcessMouseMovement(VirtualMouse& mouse, GamepadDataPayload const& padData, int16_t mouseSensitivity)
{
    auto x = (padData.ljx) % (int)joystickExtent;
    auto y = (-padData.ljy) % (int)joystickExtent;
    auto normalisedX = (long)(mouseSensitivity * (x / joystickExtent));
    auto normalisedY = (long)(mouseSensitivity * (y / joystickExtent));
    mouse.Move(normalisedX, normalisedY);

    auto const maxScrollSpeed = 0.1 * WHEEL_DELTA;
    auto normalisedScrX = padData.rjx / joystickExtent;
    auto normalisedScrY = padData.rjy / joystickExtent;
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

std::thread StartGamepadListener(controller::ControllerConfig controllerConfig, std::atomic_bool& killStream, std::atomic_bool& gamepadActive, uint16_t port)
{
    using namespace std;
    thread workerThread{};

    if (gamepadActive.load(memory_order_acquire))
    {
        gamepadActive.store(false, memory_order_release);
    }

    workerThread = thread([&, gamepadPort = port, inputConfig = controllerConfig] {

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
                GamepadDataPayload padData = {};
                for (auto& c : padData.padding)
                    c = 0;

                std::unique_ptr<IVirtualController> controller{};

                switch (inputConfig.controllerMode)
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
                    auto mouse = VirtualMouse{0};
                    auto streamDead = killStream.load(memory_order_acquire);
                    gamepadActive.store(true, memory_order_release);
                    auto active = true;
                    auto retryCount = maxRetries;

                    controller->MapFaceButtons(inputConfig.controllerMap);
                    controller->MapAnalogAxis(inputConfig.leftAnalogMap, inputConfig.rightAnalogMap);

                    auto mouseMode{ inputConfig.mouseOnConnect };
                    auto mouseSensitivity{ inputConfig.mouseSensitivity };

                    auto leftClickBtn = inputConfig.leftClickButton;
                    auto rightClickBtn = inputConfig.rightClickButton;
                    auto middleClickBtn = HidNpadButton::HidNpadButton_StickR;

                    auto lastTime = std::chrono::high_resolution_clock::now();
                    do
                    {
                        active = gamepadActive.load(memory_order_acquire);
                        padData.keys = 0;
                        padData.ljx = padData.ljy = 0;
                        padData.rjx = padData.rjy = 0;
                        auto bytesRead = 0;
                        do
                        {
                            auto received = recv(connection.ConnectedSocket(), ((char*)&padData) + bytesRead, GamepadDataPayloadSize - bytesRead, 0);
                            if (received == SOCKET_ERROR || received == 0)
                            {
                                bytesRead = received;
                                break;
                            }
                            else
                                bytesRead += received;
                        } while (bytesRead < GamepadDataPayloadSize);
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
                        else if (bytesRead == GamepadDataPayloadSize)
                        {
                            if (padData.keys == 0xFFFF)
                            {
                                cout << "Received kill signal" << endl;
                                killStream.store(true, memory_order_release);
                                active = false;
                                streamDead = true;
                            }
                            else if (mouseMode)
                            {
                                ProcessMouseMovement(mouse, padData, mouseSensitivity);
                                ProcessMouseButtons(mouse, padData, leftClickBtn, rightClickBtn, middleClickBtn);
                                mouse.Commit();
                            }
                            else if (padData.keys != 0x0 || (padData.ljx | padData.ljy | padData.rjx | padData.rjy) != 0)
                            {
                                controller->ConvertPayload(padData);
                                //controller->Print();
                                controller->UpdateState();
                                controller->UpdateController();
                            }
                            else
                            {
                                controller->ResetController();
                                controller->UpdateController();
                            }

                            // figure out if we should toggle mouse mode
                            if (padData.keys == mouseToggleBtnCombo || padData.keys & HidNpadButton::HidNpadButton_Palma)
                            {
                                auto timePassed = std::chrono::high_resolution_clock::now() - lastTime;
                                if (timePassed.count() >= mouseToggleNano)
                                {
                                    controller->ResetController();
                                    controller->UpdateController();

                                    mouseMode = !mouseMode;

                                    if (mouseMode)
                                        MessageBeep(MB_OK);
                                    else
                                        MessageBeep(MB_ICONERROR);

                                    ReleaseMouseButtons(mouse);

                                    lastTime = std::chrono::high_resolution_clock::now();
                                }
                            }
                            else
                                lastTime = std::chrono::high_resolution_clock::now();
                        }
                        else if (bytesRead == 0)
                        {
                            // connection closed, cleanup
                            killStream.store(true, memory_order_release);
                            active = false;
                            streamDead = true;
                        }
                    } while (padData.keys != 0xFFFF && !streamDead && active);

                    // make sure there aren't accidental junk key presses when stream dies
                    ReleaseMouseButtons(mouse);

                    controller->ResetController();
                    controller->UpdateController();
                    controller->Disconnect();
                }
                else
                {
                    std::cout << "Error: Virtual Controller could not be created. Please make sure the Virtual Controller driver is installed correctly.\n";
                    std::cout << "    Stream will shutdown now...\n\n";
                }
            }
        }

        connection.Close();

        killStream.store(true, memory_order_release);
        gamepadActive.store(false, memory_order_release);
        });

    return workerThread;
}
