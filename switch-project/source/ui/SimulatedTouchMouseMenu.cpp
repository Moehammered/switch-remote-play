#include "SimulatedTouchMouseMenu.h"
#include <switch.h>

auto const simulatedTouchHelpText =
"Simulated Mouse mode allows the touch input to be interpreted as mouse input on the PC.\n\
DeadzoneRadius = how far a finger has to travel to register movement.\n\
\n\
/!\\ This mode will only work for the primary display monitor /!\\\n";

SimulatedTouchMouseMenu::SimulatedTouchMouseMenu() : Menu(), helpText{},
textElements{}, selected{touch::SimulatedTouchParamsList},
deadzoneRadius{touch::DefaultSimulatedMouseDeadzoneRadius}
{
    title.value = "Simulated Mouse Options";
    title.y += 30;

    helpText.y += 500;
    helpText.x = 30;
    helpText.colour = {200, 200, 50, 255};
    helpText.value = simulatedTouchHelpText;

    SetupText();
}

void SimulatedTouchMouseMenu::ProcessInput(PadState const & pad)
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

void SimulatedTouchMouseMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
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

touch::SimulatedTouchConfig const SimulatedTouchMouseMenu::Settings() const
{
    auto config = touch::SimulatedTouchConfig{};

    config.deadzoneRadius = deadzoneRadius;

    return config;
}

void SimulatedTouchMouseMenu::PromptValueInput(touch::SimulatedTouchMouseParameters param)
{
    switch(param)
    {
        case touch::SimulatedTouchMouseParameters::DeadzoneRadius:
        {
            deadzoneRadius = KeyboardValue(touch::MinSimulatedMouseDeadzoneRadius, touch::MaxSimulatedMouseDeadzoneRadius);
        }
        break;
    }
}

int16_t SimulatedTouchMouseMenu::KeyboardValue(int16_t minValue, int16_t maxValue)
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
        swkbdConfigSetRightOptionalSymbolKey(&kbd, "-");
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

void SimulatedTouchMouseMenu::UpdateUI(touch::SimulatedTouchMouseParameters param)
{
    auto prefix = touch::SimulatedTouchParamsDesc.at(param);
    switch(param)
    {
        case touch::SimulatedTouchMouseParameters::DeadzoneRadius:
        {
            auto str = std::to_string(deadzoneRadius);
            textElements[param].value = prefix + ": " + str;
        }
        break;
    }
}

void SimulatedTouchMouseMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = touch::SimulatedTouchParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}