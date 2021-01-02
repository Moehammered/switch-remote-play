#include "ControllerScreen.h"
#include <string>
#include <iostream>
#include "../system/Configuration.h"

SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};
SDL_Color constexpr backgroundColour {.r = 100, .g = 100, .b = 100, .a = 255};

std::string ControllerModeToString(ControllerMode mode)
{
    switch(mode)
    {
        case ControllerMode::X360:
            return "X360";
        case ControllerMode::DS4:
            return "DS4";

        default:
            return "unknown";
    }
}

ControllerMenu::ControllerMenu() 
    : Menu(), settingIndex(0), settingsIndices{}, settingsText{}
{
    const int settingTextX = 100;
    const int yOffset = 200;
    const int ySpace = 45;
    int counter = 1;
    for(auto &t : settingsText)
    {
        t.colour = textColour;
        t.x = settingTextX;
        t.y = yOffset + ySpace * counter++;
    }

    auto config = Configuration{};
    auto controller = config.ControllerData();

    settingsIndices[ControllerMenu::CONTROLLER_MODE] = controller.controllerMode;

    UpdateControllerMode();
}

void ControllerMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & KEY_DUP)
        SelectPrevious();
    else if(kDown & KEY_DDOWN)
        SelectNext();

    if(kDown & KEY_A)
        IncreaseSetting();
    else if(kDown & KEY_B)
        DecreaseSetting();
}

void ControllerMenu::IncreaseSetting()
{
    auto currInd = settingsIndices[settingIndex];

    switch(settingIndex)
    {
        case ControllerMenuItems::CONTROLLER_MODE:
            if(++currInd >= ControllerMode::CONTROLLER_MODE_COUNT)
                currInd = 0;
        break;
    }

    settingsIndices[settingIndex] = currInd;

    UpdateControllerMode();
}

void ControllerMenu::DecreaseSetting()
{
    auto currInd = settingsIndices[settingIndex];

    switch(settingIndex)
    {
        case ControllerMenuItems::CONTROLLER_MODE:
            if(--currInd < 0)
                currInd = ControllerMode::CONTROLLER_MODE_COUNT - 1;
        break;
    }

    settingsIndices[settingIndex] = currInd;

    UpdateControllerMode();
}

void ControllerMenu::SelectNext()
{
    ++settingIndex;
    if(settingIndex >= settingsIndices.size())
        settingIndex = 0;
}

void ControllerMenu::SelectPrevious()
{
    --settingIndex;
    if(settingIndex < 0)
        settingIndex = settingsIndices.size() - 1;
}


void ControllerMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto i = 0; i < settingsText.size(); ++i)
    {
        if(i != settingIndex)
            settingsText[i].Render(renderer, font);
        else
            settingsText[i].Render(renderer, font, highlightColour);
    }
}

Controller_Config const ControllerMenu::Settings()
{
    auto controllerMode = (ControllerMode)settingsIndices[ControllerMenuItems::CONTROLLER_MODE];

    return Controller_Config{
        .controllerMode = controllerMode,
    };
}


void ControllerMenu::UpdateControllerMode()
{
    const auto controllerMode = (ControllerMode)settingsIndices[ControllerMenuItems::CONTROLLER_MODE];
    settingsText[ControllerMenuItems::CONTROLLER_MODE].value = "ControllerMode:            " + ControllerModeToString(controllerMode);
}
