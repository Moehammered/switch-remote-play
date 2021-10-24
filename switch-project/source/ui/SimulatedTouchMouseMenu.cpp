#include "SimulatedTouchMouseMenu.h"
#include "../system/SoftwareKeyboard.h"
#include "srp/touch/SimulatedMouseConfiguration.h"
#include <switch.h>

auto const simulatedTouchHelpText = 
"Simulated Mouse mode allows the touch input to be interpreted as mouse input on the PC.\n\
DeadzoneRadius = how far a finger has to travel to register movement.\n\
Behaviour = how the touch screen input maps to the mouse behaviour and position.\n\
Double Tap Time = effects click and drag behaviour when quick touches occur.";

SimulatedTouchMouseMenu::SimulatedTouchMouseMenu() : Menu(), helpText{},
    textElements{}, selected{touch::simulatedTouchParamsList},
    deadzoneRadius{touch::defaultSimulatedMouseDeadzoneRadius},
    behaviourCursor{touch::simulatedMouseBehaviourParamsDesc},
    doubleTapTime{touch::defaultDoubleTapTime},
    trackpadSensitivity{touch::defaultTrackpadSensitivityPercentage}
{
    title.value = "Simulated Mouse Options";
    title.y += 130;

    auto config = SimulatedMouseConfiguration{};
    auto savedData = config.Data();
    deadzoneRadius = savedData.deadzoneRadius;
    cycleMap(behaviourCursor, savedData.behaviour);
    doubleTapTime = savedData.doubleTapTime;
    trackpadSensitivity = savedData.trackpadSensitivityPercentage;

    helpText.y += 520;
    helpText.x = 30;
    helpText.colour = { 255, 190, 90, 255 };
    helpText.value = simulatedTouchHelpText;

    SetupText();
}

void SimulatedTouchMouseMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        --selected;
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        ++selected;

    if(kDown & HidNpadButton::HidNpadButton_A)
        PromptValueInput(*selected, 1);
    else if(kDown & HidNpadButton::HidNpadButton_B)
        PromptValueInput(*selected, -1);
    
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
    config.behaviour = behaviourCursor.KeyPair().first;
    config.doubleTapTime = doubleTapTime;
    config.trackpadSensitivityPercentage = trackpadSensitivity;

    return config;
}

void SimulatedTouchMouseMenu::PromptValueInput(touch::SimulatedTouchMouseParameters param, int value)
{
    switch(param)
    {
        case touch::SimulatedTouchMouseParameters::DeadzoneRadius:
        {
            if(value <= 0)
                deadzoneRadius = touch::defaultSimulatedMouseDeadzoneRadius;
            else
            {
                deadzoneRadius = keyboardNumber(touch::minSimulatedMouseDeadzoneRadius,
                                                touch::maxSimulatedMouseDeadzoneRadius,
                                                deadzoneRadius);
            }
        }
        break;

        case touch::SimulatedTouchMouseParameters::Behaviour:
        {
            behaviourCursor += value;
        }
        break;

        case touch::SimulatedTouchMouseParameters::DoubleTapTime:
        {
            if(value <= 0)
                doubleTapTime = touch::defaultDoubleTapTime;
            else
            {
                auto const currentTime = timeutil::nanoToSecond(doubleTapTime);
                auto const maxTime = timeutil::nanoToSecond(touch::maxDoubleTapTime);
                auto const minTime = timeutil::nanoToSecond(touch::minDoubleTapTime);
                auto const toggleTime = keyboardDecimal(minTime, maxTime, currentTime);

                doubleTapTime = timeutil::secondToNano(toggleTime);
            }
        }
        break;

        case touch::SimulatedTouchMouseParameters::TrackpadSensitivityPercentage:
        {
            if(value <= 0)
                trackpadSensitivity = touch::defaultTrackpadSensitivityPercentage;
            else
            {
                trackpadSensitivity = (int16_t)keyboardNumber(touch::minTrackpadSensitivityPercentage, 
                                                                touch::maxTrackpadSensitivityPercentage,
                                                                trackpadSensitivity);
            }
        }
        break;
    }
}

void SimulatedTouchMouseMenu::UpdateUI(touch::SimulatedTouchMouseParameters param)
{
    auto prefix = touch::simulatedTouchParamsDesc.at(param);
    auto updateElement = [&](auto str)
    {
        textElements[param].value = prefix + ": " + str;
    };

    switch(param)
    {
        case touch::SimulatedTouchMouseParameters::DeadzoneRadius:
        {
            auto str = std::to_string(deadzoneRadius);
            updateElement(str);
        }
        break;

        case touch::SimulatedTouchMouseParameters::Behaviour:
        {
            updateElement(*behaviourCursor);
        }
        break;

        case touch::SimulatedTouchMouseParameters::DoubleTapTime:
        {
            auto seconds = timeutil::nanoToSecond(doubleTapTime);
            auto str = std::to_string(seconds) + "s";
            updateElement(str);
        }
        break;

        case touch::SimulatedTouchMouseParameters::TrackpadSensitivityPercentage:
        {
            auto str = std::to_string(trackpadSensitivity) + "%%";
            updateElement(str);
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

    auto params = touch::simulatedTouchParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}