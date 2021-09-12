#include "SoftwareKeyboard.h"
#include <switch.h>
#include <string>
#include <vector>

int KeyboardNumber(int minValue, int maxValue)
{
    auto minStr = std::to_string(minValue);
    auto maxStr = std::to_string(maxValue);
    auto validationMsg = "Value must be between " + minStr + " and " + maxStr;

    //open keyboard here
    auto buffer = std::vector<char>(validationMsg.size());
    auto kbd = SwkbdConfig{};
    auto libRes = swkbdCreate(&kbd, 0);

    auto inputInvalid = [](auto a, auto b, auto c)
    {
        return c < a || c > b;
    };

    if(R_SUCCEEDED(libRes))
    {
        swkbdConfigMakePresetDefault(&kbd);
        swkbdConfigSetType(&kbd, SwkbdType_NumPad);
        swkbdConfigSetHeaderText(&kbd, validationMsg.c_str());
        swkbdConfigSetStringLenMax(&kbd, buffer.size());
        swkbdConfigSetStringLenMin(&kbd, 1);

        int16_t numberInput = 0;
        do
        {
            auto revealed = swkbdShow(&kbd, buffer.data(), buffer.size());
            if(R_SUCCEEDED(revealed))
                numberInput = std::atoi(buffer.data());
            else
            {
                numberInput = minValue;
                break;
            }
        } while (inputInvalid(minValue, maxValue, numberInput));
        
        swkbdClose(&kbd);
        return numberInput;
    }
    else
        return minValue;
}