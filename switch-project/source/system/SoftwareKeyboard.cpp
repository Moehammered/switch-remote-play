#include "SoftwareKeyboard.h"
#include <switch.h>
#include <string>
#include <vector>

bool CreateKeyboard(SoftwareKeyboardProperties const & configuration, SwkbdConfig & kbd)
{
    kbd = SwkbdConfig{};
    auto created = swkbdCreate(&kbd, 0);
    if(R_SUCCEEDED(created))
    {
        swkbdConfigMakePresetDefault(&kbd);
        swkbdConfigSetType(&kbd, configuration.keyboardLayout);
        
        if(!configuration.displayMessage.empty())
            swkbdConfigSetHeaderText(&kbd, configuration.displayMessage.c_str());
        else
            swkbdConfigSetHeaderText(&kbd, "Please enter some text and press enter when done.");
        
        auto maxLen = configuration.inputLength < 1 ? 1 : configuration.inputLength;
        swkbdConfigSetStringLenMax(&kbd, maxLen);
        swkbdConfigSetStringLenMin(&kbd, 0);

        swkbdConfigSetInitialText(&kbd, configuration.initialText.c_str());
        
        if(!configuration.optionalLeftSymbol.empty())
            swkbdConfigSetLeftOptionalSymbolKey(&kbd, configuration.optionalLeftSymbol.c_str());

        if(!configuration.optionalRightSymbol.empty())
            swkbdConfigSetRightOptionalSymbolKey(&kbd, configuration.optionalRightSymbol.c_str());

        return true;
    }
    else
        return false;
}

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