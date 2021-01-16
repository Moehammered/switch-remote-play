#include "SwitchStream.h"

//#include <processthreadsapi.h>
#include "SwitchControlsDefinitions.h"
//#include "X360Controller.h"
//#include "DS4Controller.h"
#include "Connection.h"
#include "FFMPEGHelper.h"
#include <memory>
#include <chrono>
#ifdef __APPLE__
#include "VirtualMouse.h"
#endif

CommandPayload ReadPayloadFromSwitch(SOCKET const& switchSocket)
{
   using namespace std;

   cout << "Expected size of command payload: " << COMMAND_PAYLOAD_SIZE << " bytes" << endl;
   cout << FFMPEG_CONFIG_SIZE << " + " << CONTROLLER_CONFIG_SIZE << " + " << COMMAND_CODE_SIZE << "\n\n";
   CommandPayload data{};
   char* readBuffer = (char*)(&data);
   int retryCount = 3;

   cout << "Reading command..." << endl;

   do
   {
      auto result = recv(switchSocket, readBuffer, COMMAND_PAYLOAD_SIZE, 0);
      if (result < 0)
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
      {
          cout << "received " << result << " bytes\n\n";
         return data;
      }
   } while (retryCount > 0);

   return data;
}

std::thread StartGamepadListener(ControllerConfig controllerConfig, std::atomic_bool& killStream, std::atomic_bool& gamepadActive, uint16_t port)
{
    using namespace std;
    thread workerThread{};

    if (gamepadActive.load(memory_order_acquire))
        gamepadActive.store(false, memory_order_release);

    workerThread = thread([&, gamepadPort = port, inputConfig = controllerConfig] {

        while (gamepadActive.load(memory_order_acquire)) //don't let it continue if a previous gamepadThread is running
            this_thread::sleep_for(chrono::duration<int, milli>(100)); //wait a bit to let previous gamepad thread cleanup

        auto connection = Connection(gamepadPort);
        if (connection.Ready())
        {
            if (connection.WaitForConnection())
            {
                //can use the switch socket now to listen for input sent over
                GamepadDataPayload padData = {};
                for (auto& c : padData.padding)
                    c = 0;
                
                auto streamDead = killStream.load(memory_order_acquire);
                gamepadActive.store(true, memory_order_release);
                auto active = true;
                auto const maxRetries = 5;
                auto retryCount = maxRetries;

                auto mouseMode {true};
                auto mouseSensitivity{ inputConfig.mouseSensitivity };
                auto leftClickBtn = inputConfig.leftClickButton;
                auto rightClickBtn = inputConfig.rightClickButton;

                auto vmouse = VirtualMouse{};
                vmouse.SetLeftClickFlag(leftClickBtn);
                vmouse.SetRightClickFlag(rightClickBtn);

                double constexpr joystickExtent = 0xFFFF / 2;
                do
                {
                    active = gamepadActive.load(memory_order_acquire);
                    padData.keys = 0;
                    padData.ljx = padData.ljy = 0;
                    padData.rjx = padData.rjy = 0;
                    
                    auto result = recv(connection.ConnectedSocket(), (char*)&padData, GamepadDataPayloadSize, 0);
                    if (result < 0)
                    {
                        cout << "Failed to receive data for gamepad stream" << endl;
                        std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1000));
                        --retryCount;
                        if (retryCount == 0)
                        {
                            killStream.store(true, memory_order_release);
                            active = false;
                            streamDead = true;
                        }
                    }
                    else if (result > 0)
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
                            auto x = (padData.rjx + padData.ljx) % (int)joystickExtent;
                            auto y = (-padData.rjy + -padData.ljy) % (int)joystickExtent;
                            auto dx = (int)(mouseSensitivity * (x / joystickExtent));
                            auto dy = (int)(mouseSensitivity * (y / joystickExtent));

//                            if((dx-dy) != 0)
//                                std::cout << "should move " << dx << " x " << dy << "\n";
//
//                            if(padData.keys & leftClickBtn)
//                                std::cout << "Left click flag found\n\n";
                            
                            vmouse.Move(dx, dy);
                            vmouse.SetButtons(padData.keys & (leftClickBtn | rightClickBtn));
                            vmouse.Update();
                        }
                    }
                    else if (result == 0)
                    {
                        // connection closed, cleanup
                        killStream.store(true, memory_order_release);
                        active = false;
                        streamDead = true;
                    }
                } while (padData.keys != 0xFFFF && !streamDead && active);

                vmouse.ResetButtons();
            }
        }

        connection.Close();

        killStream.store(true, memory_order_release);
        gamepadActive.store(false, memory_order_release);
    });

   return workerThread;
}
