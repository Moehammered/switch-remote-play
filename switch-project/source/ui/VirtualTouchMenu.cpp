#include "VirtualTouchMenu.h"
#include <switch.h>

auto const virtualTouchHelpText =
"Virtual Touch mode allows the touch input to be interpreted as touch input on the PC.\n\
DeadzoneRadius = how far a finger has to travel to register movement.\n\
MaxFingerCount = how many fingers are monitored for touch input.";

VirtualTouchMenu::VirtualTouchMenu() : Menu(), helpText{},
textElements{}, selected{touch::VirtualTouchParamsList},
deadzoneRadius{touch::DefaultVirtualTouchDeadzoneRadius},
maxFingerCount{touch::DefaultMaxFingerCount}
{
    title.value = "Vitual Touch Options";
    title.y += 30;

    helpText.y += 500;
    helpText.x = 30;
    helpText.colour = {200, 200, 50, 255};
    helpText.value = virtualTouchHelpText;

    SetupText();
}

void VirtualTouchMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        ++selected;
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        --selected;

    if(kDown & HidNpadButton::HidNpadButton_A)
        PromptValueInput(*selected);
    
    UpdateUI(*selected);
}

void VirtualTouchMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    helpText.Render(renderer, font);

    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto const & t : textElements)
    {
        if(t.first == *selected)
            t.second.Render(renderer, font, highlightColour);
        else
            t.second.Render(renderer, font);
    }
}

touch::VirtualTouchConfig const VirtualTouchMenu::Settings() const
{
    auto config = touch::VirtualTouchConfig{};

    config.deadzoneRadius = deadzoneRadius;
    config.maxFingerCount = maxFingerCount;

    return config;
}

void VirtualTouchMenu::PromptValueInput(touch::VirtualTouchParameters param)
{
    switch(param)
    {
        case touch::VirtualTouchParameters::DeadzoneRadius:
        {
            deadzoneRadius = KeyboardValue(touch::MinVirtualTouchDeadzoneRadius, touch::MaxVirtualTouchDeadzoneRadius);
        }
        break;

        case touch::VirtualTouchParameters::MaxFingerCount:
        {
            maxFingerCount = KeyboardValue(touch::MinFingerCount, touch::MaxFingerCount);
        }
        break;
    }
}

int16_t VirtualTouchMenu::KeyboardValue(int16_t minValue, int16_t maxValue)
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

void VirtualTouchMenu::UpdateUI(touch::VirtualTouchParameters param)
{
    auto prefix = touch::VirtualTouchParamsDesc.at(param);
    switch(param)
    {
        case touch::VirtualTouchParameters::DeadzoneRadius:
        {
            auto str = std::to_string(deadzoneRadius);
            textElements[param].value = prefix + ": " + str;
        }
        break;

        case touch::VirtualTouchParameters::MaxFingerCount:
        {
            auto str = std::to_string(maxFingerCount);
            textElements[param].value = prefix + ": " + str;
        }
        break;
    }
}

void VirtualTouchMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = touch::VirtualTouchParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}