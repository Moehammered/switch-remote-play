#include "SimulatedTouchMouseMenu.h"
#include "../touch/SimulatedMouseConfiguration.h"
#include "../system/SoftwareKeyboard.h"
#include <switch.h>

auto const simulatedTouchHelpText =
"/!\\ This mode will only work for the primary display monitor /!\\\n\
\n\
Simulated Mouse mode allows the touch input to be interpreted as mouse input on the PC.\n\
DeadzoneRadius = how far a finger has to travel to register movement.\n\
\n\
/!\\ This mode will only work for the primary display monitor /!\\\n";

SimulatedTouchMouseMenu::SimulatedTouchMouseMenu() : Menu(), helpText{},
textElements{}, selected{touch::SimulatedTouchParamsList},
deadzoneRadius{touch::DefaultSimulatedMouseDeadzoneRadius}
{
    title.value = "Simulated Mouse Options";
    title.y += 150;

    auto config = SimulatedMouseConfiguration{};
    auto savedData = config.Data();
    deadzoneRadius = savedData.deadzoneRadius;

    helpText.y += 500;
    helpText.x = 30;
    helpText.colour = { 255, 190, 90, 255 };
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
            deadzoneRadius = KeyboardNumber(touch::MinSimulatedMouseDeadzoneRadius,
                                            touch::MaxSimulatedMouseDeadzoneRadius,
                                            deadzoneRadius);
            break;
    }
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