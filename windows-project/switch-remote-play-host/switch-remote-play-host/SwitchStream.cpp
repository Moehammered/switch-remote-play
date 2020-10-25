#include "SwitchStream.h"

#include <processthreadsapi.h>
#include "SwitchControlsDefinitions.h"
#include "VirtualController.h"
#include "Connection.h"
#include "FFMPEGHelper.h"

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

std::thread StartGamepadListener(std::atomic_bool& killStream, std::atomic_bool& gamepadActive)
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
