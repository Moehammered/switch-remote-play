#include "SwitchStream.h"

#include <processthreadsapi.h>
#include "SwitchControlsDefinitions.h"
#include "VirtualController.h"
#include "Connection.h"
#include "FFMPEGHelper.h"

FFMPEG_Config receivedConfig = DEFAULT_CONFIGS[0];
std::atomic<bool> killStream;
std::atomic<bool> gamepadActive;

void ReadCommandsFromSwitch(SOCKET const & switchSocket)
{
    using namespace std;

    cout << "Expected size of command payload: " << COMMAND_PAYLOAD_SIZE << " bytes" << endl;
    char* readBuffer = new char[COMMAND_PAYLOAD_SIZE];
    CommandPayload* data = (CommandPayload*)readBuffer;
    int retryCount = 3;

    PROCESS_INFORMATION streamProcessInfo;
    ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
    bool started = false;
    thread gamepadThread;

    killStream.store(false, memory_order_release);

    auto lastCommand = Command::IGNORE_COMMAND;

    do
    {
        cout << "Reading command..." << endl;
        for (int i = 0; i < COMMAND_PAYLOAD_SIZE; ++i)
            readBuffer[i] = 0;

        auto result = recv(switchSocket, readBuffer, COMMAND_PAYLOAD_SIZE, 0);
        if (result == SOCKET_ERROR)
        {
            cout << "Failed to receive data" << endl;
            --retryCount;
            data->commandCode = retryCount < 0 ? Command::SHUTDOWN : Command::IGNORE_COMMAND;
        }
        else if (result == 0) //once the switch closes the command socket, this will hit
        {
            cout << "breaking as result returned back 0" << endl;
            break;
        }

        lastCommand = data->commandCode;

        switch (data->commandCode)
        {
        case Command::STOP_STREAM:
            cout << "Close stream" << endl;
            TerminateProcess(streamProcessInfo.hProcess, 1);

            break;

        case Command::UPDATE_FFMPEG_CONFIG:
            cout << "Receive ffmpeg config" << endl;
            receivedConfig = data->configData;
            cout << "Received config:" << endl << endl << ConfigToString(receivedConfig) << endl;

            break;

        case Command::CLOSE_SERVER:
            cout << "Close server application" << endl;
            TerminateProcess(streamProcessInfo.hProcess, 1);

            break;

        case Command::SHUTDOWN_PC:
            cout << "Shutdown host PC (Not implemented)" << endl;
            TerminateProcess(streamProcessInfo.hProcess, 1);

            break;

        case Command::START_STREAM:
            TerminateProcess(streamProcessInfo.hProcess, 1);

            receivedConfig = data->configData;
            cout << "Start stream with last received config from switch..." << endl;
            cout << "FFMPEG Configuration: " << endl << ConfigToString(receivedConfig) << endl;
            streamProcessInfo = StartStream(receivedConfig, started);
            gamepadThread = StartGamepadListener();

            break;

        case Command::START_STREAM_LOW_LATENCY_30FPS: // to be removed
            TerminateProcess(streamProcessInfo.hProcess, 1);

            cout << "Start stream [low latency 30fps]" << endl;
            cout << "FFMPEG Configuration: " << endl << ConfigToString(DEFAULT_CONFIGS[STREAM_MODE::LOW_LATENCY_30_FPS]) << endl;
            streamProcessInfo = StartStream(DEFAULT_CONFIGS[STREAM_MODE::LOW_LATENCY_30_FPS], started);
            gamepadThread = StartGamepadListener();

            break;

        case Command::START_STREAM_LOW_LATENCY_VFPS: // to be removed
            TerminateProcess(streamProcessInfo.hProcess, 1);

            cout << "Start stream [low latency variable fps]" << endl;
            cout << "FFMPEG Configuration: " << endl << ConfigToString(DEFAULT_CONFIGS[STREAM_MODE::LOW_LATENCY_V_FPS]) << endl;
            streamProcessInfo = StartStream(DEFAULT_CONFIGS[STREAM_MODE::LOW_LATENCY_V_FPS], started);
            gamepadThread = StartGamepadListener();

            break;

        case Command::START_STREAM_OK_LATENCY_60FPS: // to be removed
            TerminateProcess(streamProcessInfo.hProcess, 1);

            cout << "Start stream [OK latency 60 fps]" << endl;
            cout << "FFMPEG Configuration: " << endl << ConfigToString(DEFAULT_CONFIGS[STREAM_MODE::OK_LATENCY_60_FPS]) << endl;
            streamProcessInfo = StartStream(DEFAULT_CONFIGS[STREAM_MODE::OK_LATENCY_60_FPS], started);
            gamepadThread = StartGamepadListener();

            break;
        }
    } while (data->commandCode != Command::SHUTDOWN);

    cout << "waiting for gamepad thread to shutdown..." << endl;
    while (gamepadActive.load(memory_order_acquire))
        this_thread::sleep_for(chrono::duration<int, milli>(100));

    //beyond here the network on the switch side has died
    if (gamepadThread.joinable())
        gamepadThread.join();

    cout << "making sure to kill stream..." << endl;
    killStream.store(true, memory_order_release);
    cout << "terminating the FFMPEG process" << endl;
    TerminateProcess(streamProcessInfo.hProcess, 1);
}


std::thread StartGamepadListener()
{
    using namespace std;
    thread workerThread;

    if (gamepadActive.load(memory_order_acquire))
    {
        gamepadActive.store(false, memory_order_release);
    }

    workerThread = thread([&] {

        while (gamepadActive.load(memory_order_acquire)) //don't let it continue if a previous gamepadThread is running
        {
            this_thread::sleep_for(chrono::duration<int, milli>(100)); //wait a bit to let previous gamepad thread cleanup
        }

        auto gamepadPort = 20002;
        auto connection = Connection(gamepadPort);
        if(connection.Ready())
        {
            if(connection.WaitForConnection())
            {
                //can use the switch socket now to listen for input sent over
                GamepadDataPayload padData = {};
                for (auto& c : padData.padding)
                    c = 0;
                VirtualController controller;
                if (controller.Create())
                {
                    auto streamDead = killStream.load(memory_order_acquire);
                    gamepadActive.store(true, memory_order_release);
                    auto active = true;
                    do
                    {
                        active = gamepadActive.load(memory_order_acquire);
                        padData.keys = 0;
                        padData.ljx = padData.ljy = 0;
                        padData.rjx = padData.rjy = 0;

                        auto result = recv(connection.ConnectedSocket(), (char*)&padData, GamepadPayloadSize, 0);
                        if (result == SOCKET_ERROR)
                        {
                            cout << "Failed to receive data for gamepad stream" << endl;
                        }
                        else if (padData.keys == 0xFFFF)
                        {
                            cout << "Received kill signal" << endl;
                            killStream.store(true, memory_order_release);
                            active = false;
                            streamDead = true;
                        }
                        else if (padData.keys != 0x0 || (padData.ljx | padData.ljy | padData.rjx | padData.rjy) != 0)
                        {
                            //update controller
                            auto ds4input = ConvertToDS4(padData);
                            //PrintDS4Conversion(ds4input);
                            controller.SetButtons(ds4input.buttons);
                            controller.SetDpad(ds4input.directions);
                            controller.SetAnalogAxis(ds4input.lx, ds4input.ly, ds4input.rx, ds4input.ry);
                            controller.SetShoulderTriggers(ds4input.lt, ds4input.rt);

                            controller.UpdateController();
                        }
                        else
                        {
                            controller.ResetController();
                            controller.UpdateController();
                        }
                    } while (padData.keys != 0xFFFF && !streamDead && active);

                    controller.ResetController();
                    controller.UpdateController();
                    controller.Disconnect();
                }
            }
        }

        connection.Close();

        killStream.store(true, memory_order_release);
        gamepadActive.store(false, memory_order_release);
    });

    return workerThread;
}
