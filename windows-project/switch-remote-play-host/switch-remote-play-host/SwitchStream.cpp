#include "SwitchStream.h"
#include "Connection.h"
#include "DisplayDeviceService.h"
#include "VirtualDesktop.h"
#include "ControllerStream.h"
#include "MouseStream.h"
#include "TouchStream.h"
#include "KeyboardStream.h"
#include "InputStream.h"
#include <processthreadsapi.h>
#include <memory>
#include <chrono>
#include <algorithm>

namespace
{
    auto constexpr mouseFrameTimeNano = (long long)8e+6;
    auto constexpr maxRetries = 5;
    double constexpr joystickExtent = 0xFFFF / 2;

    auto constexpr keyboardFrameTimeNano = (long long)1.6e+7;
    auto constexpr buttonHeldTime = (long long)3e+8;

    void CenterMouse(VirtualDesktop const desktop, DisplayDeviceInfo const targetDisplay)
    {
        auto center = Point<int64_t>{
            targetDisplay.x + targetDisplay.width / 2,
            targetDisplay.y + targetDisplay.height / 2
        };

        auto const virtualCoordinates = TransformCursorToVirtual(desktop, center);

        auto mouse = VirtualMouse();
        mouse.Move(SupportedMouseMovement::Absolute, (int)virtualCoordinates.x, (int)virtualCoordinates.y);
        mouse.Commit();
    }

}

CommandPayload ReadPayloadFromSwitch(SOCKET const& switchSocket, Log& logger)
{
    auto const commandSizeStr = std::to_string(commandPayloadSize);
    logger.Write("Expected size of command payload: " + commandSizeStr + " bytes\n", LogImportance::Low);

    CommandPayload data{};
    char* readBuffer = (char*)(&data);
    int retryCount = 3;

    logger.Write("Reading command...\n", LogImportance::Low);

    do
    {
        auto result = recv(switchSocket, readBuffer, commandPayloadSize, 0);
        if (result == SOCKET_ERROR)
        {
            logger.Write("Failed to receive data\n", LogImportance::High);
            --retryCount;
            data.commandCode = retryCount <= 0 ? Command::SHUTDOWN : Command::IGNORE_COMMAND;
        }
        else if (result == 0) //once the switch closes the command socket, this will hit
        {
            logger.Write("breaking as switch closed connection\n", LogImportance::Low);
            return data;
        }
        else
            return data;
    } while (retryCount > 0);

    return data;
}

//session resolution needed to clamp and normalise simulated absolute mouse movement
std::thread StartGamepadListener(DisplayDeviceInfo sessionDisplay,
    VirtualDesktop const desktop,
    Resolution const switchResolution,
    controller::ControllerConfig const controllerConfig, 
    mouse::MouseConfig const mouseConfig,
    keyboard::KeyboardConfig const keyboardConfig,
    touch::TouchConfig const touchConfig,
    std::atomic_bool& killStream, 
    std::atomic_bool& gamepadActive, 
    uint16_t const gamepadPort)
{
    if (gamepadActive.load(std::memory_order_acquire))
        gamepadActive.store(false, std::memory_order_release);

    auto streamMethod = [=, &gamepadActive, &killStream]
    {
        while (gamepadActive.load(std::memory_order_acquire)) //don't let it continue if a previous gamepadThread is running
            std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(100)); //wait a bit to let previous gamepad thread cleanup

        auto connection = Connection(gamepadPort);
        if (connection.Ready())
        {
            if (connection.WaitForConnection())
            {
                CenterMouse(desktop, sessionDisplay);
                auto controllerStream = ControllerStream{ controllerConfig };
                auto keyboardStream = KeyboardStream{ keyboardConfig, buttonHeldTime };
                auto mouseStream = MouseStream{ mouseConfig, joystickExtent, WHEEL_DELTA };
                auto touchStream = TouchStream{ touchConfig, desktop, switchResolution, sessionDisplay };

                auto inputStream = InputStream{ touchStream,
                    mouseStream,
                    keyboardStream,
                    controllerStream,
                    mouseConfig,
                    mouseFrameTimeNano,
                    keyboardFrameTimeNano };

                gamepadActive.store(true, std::memory_order_release);
                //blocks here while stream is active
                inputStream.Run(connection.ConnectedSocket(), gamepadActive, maxRetries);

                controllerStream.Reset();
                keyboardStream.Reset();
                mouseStream.Reset();
                touchStream.Reset();

                controllerStream.DisconnectControllers();
            }
        }

        connection.Close();

        killStream.store(true, std::memory_order_release);
        gamepadActive.store(false, std::memory_order_release);
    };

    return std::thread(streamMethod);
}
