#include "ButtonWatch.h"
#include <chrono>
#include <thread>
#include <switch.h>

uint64_t MonitorKeys(std::function<void(std::string)> renderFunction)
{
    auto const frameTime = std::chrono::duration<long, std::milli>(100000/3000);
    auto lastTime = std::chrono::high_resolution_clock::now();

    auto key = 0ULL;
    auto state = 0;
    auto timer = 0LL;
    auto const secondToNano = 1000 * 1000 * 1000;
    auto const holdTime = 3LL * secondToNano;
    auto complete = false;
    auto pad = PadState{};
    padInitializeAny(&pad);
    padUpdate(&pad);
    std::this_thread::sleep_for(frameTime);

    auto const startupMessage = std::string{"Please let go of all keys"};
    auto const holdPrompt = std::string{"Please hold the keys you'd like for 3 seconds"};
    auto const finishPrompt = std::string{"Please let go of all keys"};
    do
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto delta = now - lastTime;
        padUpdate(&pad);
        auto frameKeys = padGetButtons(&pad);
        switch(state)
        {
            case 0:
            {
                if(frameKeys)
                    renderFunction(startupMessage);
                else
                    ++state;
            }
            break;

            case 1:
            {

                renderFunction(holdPrompt);
                if(frameKeys)
                    ++state;
            }
            break;

            case 2:
            {
                if(frameKeys)
                {
                    timer += delta.count();
                    auto remaining = holdTime - (double)timer;
                    if(remaining < 0)
                    {
                        if(key == 0)
                            key = frameKeys;

                        renderFunction(finishPrompt);
                    }
                    else
                    {
                        auto str = "Time remaining: " + std::to_string(remaining/secondToNano) + "s";
                        renderFunction(str);
                    }
                }
                else if(timer >= holdTime)
                {
                    ++state;
                    complete = true;
                }
                else
                {
                    --state;
                    timer = 0;
                    key = 0;
                }
            }
            break;
        }

        std::this_thread::sleep_for(frameTime);
        lastTime = now;
    } while (!complete);

    return key;
}
