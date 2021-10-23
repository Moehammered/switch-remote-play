#include "VirtualTouchMenu.h"
#include "../touch/VirtualTouchConfiguration.h"
#include "../system/SoftwareKeyboard.h"
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
    title.value = "Virtual Touch Options";
    title.y += 130;

    auto config = VirtualTouchConfiguration{};
    auto savedData = config.Data();
    deadzoneRadius = savedData.deadzoneRadius;
    maxFingerCount = savedData.maxFingerCount;

    helpText.y += 520;
    helpText.x = 30;
    helpText.colour = { 255, 190, 90, 255 };
    helpText.value = virtualTouchHelpText;

    SetupText();
}

void VirtualTouchMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        --selected;
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        ++selected;

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
            deadzoneRadius = KeyboardNumber(touch::MinVirtualTouchDeadzoneRadius, 
                                            touch::MaxVirtualTouchDeadzoneRadius,
                                            deadzoneRadius);
            break;

        case touch::VirtualTouchParameters::MaxFingerCount:
            maxFingerCount = KeyboardNumber(touch::MinFingerCount,
                                            touch::MaxFingerCount,
                                            maxFingerCount);
            break;
    }
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