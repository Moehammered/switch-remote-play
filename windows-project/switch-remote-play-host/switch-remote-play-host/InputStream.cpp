#include "InputStream.h"
#include "VirtualFinger.h"
#include "srp/network/NetworkData.h"
#include "WSAHelper.h"
#include <chrono>
#include <thread>
#include <WinSock2.h>
#include <iostream>

InputStream::InputStream(TouchStream& touchStream,
                        MouseStream& mouseStream,
                        KeyboardStream& keyboardStream,
                        ControllerStream& controllerStream,
                        mouse::MouseConfig const mouseConfig,
                        int64_t mouseFrameNanoTime,
                        int64_t keyboardFrameNanoTime)
    : touchStream{ touchStream },
    mouseStream{ mouseStream },
    keyboardStream{ keyboardStream },
    controllerStream{ controllerStream },
    mouseMode{ mouseConfig.mouseOnConnect },
    mouseFrameNanoTime{ mouseFrameNanoTime },
    keyboardFrameNanoTime{ keyboardFrameNanoTime },
    mouseToggleNanoTime{ mouseConfig.mouseModeToggleTime },
    mouseToggleBtnCombo{ mouseConfig.mouseModeToggleKey }
{
}

void InputStream::Run(uint64_t const socket, std::atomic_bool& gamepadActive, int32_t const maxRetries)
{
    InputDataPayload inputData{ 0 };
    for (auto& gamepad : inputData.gamepads)
    {
        for (auto& c : gamepad.padding)
            c = 0;
    }

    auto& gamepadPayloads = inputData.gamepads;
    auto const& firstGamepad = gamepadPayloads[0];
    auto const inputDataBuffer = (char*)&inputData;

    auto keyboardPollTime = 0;
    auto mousePollTime = 0;

    auto active = true;
    auto retryCount = maxRetries;

    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    auto currentFrameTime = lastFrameTime;

    auto lastTimeMouseToggle = lastFrameTime;

    do
    {
        active = gamepadActive.load(std::memory_order_acquire);
        for (auto& gamepad : gamepadPayloads)
        {
            gamepad.keys = 0;
            gamepad.ljx = gamepad.ljy = 0;
            gamepad.rjx = gamepad.rjy = 0;
        }
        auto bytesRead = ReadStream(socket, inputDataBuffer, inputDataPayloadSize);

        currentFrameTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = currentFrameTime - lastFrameTime;

        if (bytesRead == SOCKET_ERROR)
        {
            std::cout << "Failed to receive data for gamepad stream" << std::endl;
            std::cout << "Winsock error: \n    " << WSAErrorToStr(WSAGetLastError()) << "\n";
            std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1000));
            --retryCount;
            if (retryCount == 0)
            {
                active = false;
            }
        }
        else if (bytesRead == inputDataPayloadSize)
        {
            if (firstGamepad.keys == 0xFFFF)
            {
                std::cout << "Received kill signal" << std::endl;
                active = false;
            }
            else if (mouseMode)
            {
                mousePollTime += deltaTime.count();
                if (mousePollTime >= mouseFrameNanoTime)
                {
                    mouseStream.Process(inputData, deltaTime.count());
                    mousePollTime = 0;
                }

                keyboardPollTime += deltaTime.count();
                if (keyboardPollTime >= keyboardFrameNanoTime)
                {
                    keyboardStream.Process(inputData, deltaTime.count());
                    keyboardPollTime = 0;
                }
            }
            else
            {
                // process gamepads now
                controllerStream.Process(inputData, deltaTime.count());
            }

            // figure out if we should toggle mouse mode
            if (firstGamepad.keys == mouseToggleBtnCombo)
            {
                auto timePassed = std::chrono::high_resolution_clock::now() - lastTimeMouseToggle;
                if (timePassed.count() >= mouseToggleNanoTime)
                {
                    controllerStream.Reset();

                    mouseMode = !mouseMode;

                    if (mouseMode)
                        MessageBeep(MB_OK);
                    else
                        MessageBeep(MB_ICONERROR);

                    mouseStream.Reset();

                    lastTimeMouseToggle = std::chrono::high_resolution_clock::now();
                }
            }
            else
                lastTimeMouseToggle = std::chrono::high_resolution_clock::now();

            touchStream.Process(inputData, deltaTime.count());
        }
        else if (bytesRead == 0)
        {
            // connection closed, cleanup
            active = false;
        }

        lastFrameTime = currentFrameTime;
    } while (firstGamepad.keys != 0xFFFF && active);

    gamepadActive.store(true, std::memory_order_release);
}

int32_t InputStream::ReadStream(uint64_t const socket, char* const buffer, size_t const size)
{
    auto bytesRead = 0;

    do
    {
        auto received = recv(socket, buffer + bytesRead, size - bytesRead, 0);
        if (received == SOCKET_ERROR || received == 0)
        {
            bytesRead = received;
            break;
        }
        else
            bytesRead += received;
    } while (bytesRead < size);

    return bytesRead;
}
