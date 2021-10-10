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

int32_t const KeyboardNumber(int32_t const minValue, int32_t const maxValue, int32_t const defaultValue)
{
    auto settings = KeyboardParserProperties<int32_t>{};
    settings.defaultValue = defaultValue;
    settings.parseMethod = [](std::string const text)
    {
        return std::atoi(text.c_str());
    };
    settings.predicate = [=](int32_t const value)
    {
        return value >= minValue && value <= maxValue;
    };

    auto minStr = std::to_string(minValue);
    auto maxStr = std::to_string(maxValue);
    auto header = "Value must be between " + minStr + " and " + maxStr;
    settings.keyboardConfig.displayMessage = header;
    settings.keyboardConfig.initialText = std::to_string(defaultValue);
    settings.keyboardConfig.keyboardLayout = SwkbdType::SwkbdType_NumPad;
    settings.keyboardConfig.inputLength = std::max(minStr.size(), maxStr.size());
    settings.keyboardConfig.optionalLeftSymbol = "-";

    return OpenKeyboard(settings);
}