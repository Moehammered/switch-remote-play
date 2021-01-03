#include "ControllerMenu.h"
#include <string>
#include <iostream>
#include "../system/Configuration.h"
#include "../dataHelpers/ControllerButtonMap.h"

SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};
SDL_Color constexpr backgroundColour {.r = 100, .g = 100, .b = 100, .a = 255};
constexpr int maxMouseSensitivity = 30;
constexpr int minMouseSensitivity = 3;


ControllerMenu::ControllerMenu() 
    : Menu(), selectedItem{}, settingsIndices{}, settingsText{}
{
    title.value = "Controller Configuration";
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

    settingsIndices[ControllerMenuItems::CONTROLLER_MODE] = controller.controllerMode;
    settingsIndices[ControllerMenuItems::CONTROLLER_BTN_MAP] = controller.controllerMap;
    settingsIndices[ControllerMenuItems::MOUSE_ON_CONNECT] = controller.mouseOnConnect;
    settingsIndices[ControllerMenuItems::MOUSE_SENSITIVITY] = controller.mouseSensitivity;

    Update();
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
    auto currInd = settingsIndices[selectedItem];

    switch(selectedItem)
    {
        case ControllerMenuItems::CONTROLLER_MODE:
            if(++currInd >= ControllerMode::CONTROLLER_MODE_COUNT)
                currInd = 0;
        break;

        case ControllerMenuItems::CONTROLLER_BTN_MAP:
            if(++currInd >= ControllerButtonMap::CONTROLLER_MAP_COUNT)
                currInd = 0;
        break;

        case ControllerMenuItems::MOUSE_ON_CONNECT:
            if(++currInd > 1)
                currInd = 0;
        break;

        case ControllerMenuItems::MOUSE_SENSITIVITY:
            if(++currInd > maxMouseSensitivity)
                currInd = minMouseSensitivity;
        break;
    }

    settingsIndices[selectedItem] = currInd;

    Update();
}

void ControllerMenu::DecreaseSetting()
{
    auto currInd = settingsIndices[selectedItem];

    switch(selectedItem)
    {
        case ControllerMenuItems::CONTROLLER_MODE:
            if(--currInd < 0)
                currInd = ControllerMode::CONTROLLER_MODE_COUNT - 1;
        break;

        case ControllerMenuItems::CONTROLLER_BTN_MAP:
            if(--currInd < 0)
                currInd = ControllerButtonMap::CONTROLLER_MAP_COUNT - 1;
        break;

        case ControllerMenuItems::MOUSE_ON_CONNECT:
            if(--currInd < 0)
                currInd = 1;
        break;

        case ControllerMenuItems::MOUSE_SENSITIVITY:
            if(--currInd < minMouseSensitivity)
                currInd = maxMouseSensitivity;
        break;
    }

    settingsIndices[selectedItem] = currInd;

    Update();
}

void ControllerMenu::SelectNext()
{
    auto selected = (int32_t)selectedItem;
    ++selected;
    if(selected >= ControllerMenuItems::CONTROLLER_MENU_COUNT)
        selected = 0;
    selectedItem = (ControllerMenuItems)selected;
}

void ControllerMenu::SelectPrevious()
{
    auto selected = (int32_t)selectedItem;
    --selected;
    if(selected < 0)
        selected = ControllerMenuItems::CONTROLLER_MENU_COUNT - 1;
    selectedItem = (ControllerMenuItems)selected;
}


void ControllerMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto i = 0U; i < settingsText.size(); ++i)
    {
        if(i != selectedItem)
            settingsText[i].Render(renderer, font);
        else
            settingsText[i].Render(renderer, font, highlightColour);
    }
}

Controller_Config const ControllerMenu::Settings()
{
    auto controllerMode = (ControllerMode)settingsIndices[ControllerMenuItems::CONTROLLER_MODE];
    auto const controllerMap = (ControllerButtonMap)settingsIndices[ControllerMenuItems::CONTROLLER_BTN_MAP];
    auto const mouseOnConnect = settingsIndices[ControllerMenuItems::MOUSE_ON_CONNECT] == 1;
    auto const sensitivity = settingsIndices[ControllerMenuItems::MOUSE_SENSITIVITY];

    return Controller_Config{
        .controllerMode = controllerMode,
        .controllerMap = controllerMap,
        .mouseSensitivity = sensitivity,
        .mouseOnConnect = mouseOnConnect
    };
}


void ControllerMenu::Update()
{
    const auto controllerMode = (ControllerMode)settingsIndices[ControllerMenuItems::CONTROLLER_MODE];
    settingsText[ControllerMenuItems::CONTROLLER_MODE].value = "Controller Mode:        " + ControllerModeDescription(controllerMode);

    auto const controllerMap = (ControllerButtonMap)settingsIndices[ControllerMenuItems::CONTROLLER_BTN_MAP];
    settingsText[ControllerMenuItems::CONTROLLER_BTN_MAP].value = "Controller Mapping:     " + ControllerButtonMapDescription(controllerMap);

    auto const mouseOnConnect = settingsIndices[ControllerMenuItems::MOUSE_ON_CONNECT] == 1;
    std::string parsedBool = mouseOnConnect ? "Yes" : "No";
    settingsText[ControllerMenuItems::MOUSE_ON_CONNECT].value = "Mouse on Connect:       " + parsedBool;

    auto const sensitivity = settingsIndices[ControllerMenuItems::MOUSE_SENSITIVITY];
    settingsText[ControllerMenuItems::MOUSE_SENSITIVITY].value = "Mouse Sensitivity:      " + std::to_string(sensitivity);
}
