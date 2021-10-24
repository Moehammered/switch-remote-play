#ifndef __SOFTWAREKEYBOARD_H__
#define __SOFTWAREKEYBOARD_H__

#include <string>
#include <functional>
#include <vector>

extern "C"
{
    #include <switch/applets/swkbd.h>
}

struct SoftwareKeyboardProperties
{
    std::string displayMessage;
    uint8_t inputLength = 10;
    SwkbdType keyboardLayout = SwkbdType::SwkbdType_Normal;
    std::string optionalLeftSymbol;
    std::string optionalRightSymbol;
    std::string initialText;
};

template<typename returnType>
struct KeyboardParserProperties
{
    SoftwareKeyboardProperties keyboardConfig;

    returnType defaultValue;
    std::function<bool(returnType const)> predicate = [](returnType const a){ return true; };
    std::function<returnType(std::string const)> parseMethod = [](std::string const a){ return returnType{}; };
};

bool createKeyboard(SoftwareKeyboardProperties const & configuration, SwkbdConfig & kbd);

template<typename returnType>
returnType openKeyboard(KeyboardParserProperties<returnType> const properties)
{
    auto kbd = SwkbdConfig{};
    if(createKeyboard(properties.keyboardConfig, kbd))
    {
        auto parsedInput = properties.defaultValue;
        auto const inputLength = kbd.arg.arg.arg.stringLenMax;
        auto buffer = std::vector<char>(inputLength+1);
        
        do
        {
            auto confirmed = swkbdShow(&kbd, buffer.data(), buffer.size());
            if(R_SUCCEEDED(confirmed))
                parsedInput = properties.parseMethod(std::string(buffer.begin(), buffer.end()));
            else
            {
                parsedInput = properties.defaultValue;
                break;
            }
        } while(!properties.predicate(parsedInput));

        return parsedInput;
    }
    else
        return properties.defaultValue;
}

int32_t const keyboardNumber(int32_t const minValue, int32_t const maxValue, int32_t const defaultValue);

double const keyboardDecimal(double const minValue, double const maxValue, double const defaultValue);

#endif