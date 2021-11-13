#include "MouseMenu.h"
#include "../utils/Colours.h"
#include "../system/SoftwareKeyboard.h"
#include "../system/ButtonWatch.h"
#include "../ScreenRenderer.h"
#include "../system/SystemSetup.h"
#include "srp/mouse/MouseConfiguration.h"
#include <algorithm>

MouseMenu::MouseMenu() : Menu(),
    selectionCursor{mouse::mouseParamsList},
    textElements{}, leftMouseBtnCursor{controller::mouseButtonOptions},
    rightMouseBtnCursor{controller::mouseButtonOptions},
    middleMouseBtnCursor{controller::mouseButtonOptions},
    mouseWheelAnalogCursor{controller::analogStickOptions},
    mouseOnConnect{true}, mouseSensitivity{mouse::defaultMouseSensitivity},
    mouseToggleKey{mouse::defaultMouseModeToggleKey},
    mouseToggleTime{mouse::defaultMouseModeToggleTime}
{
    title.y += 30;
    title.value = "Mouse Configuration";

    auto config = MouseConfiguration{};
    auto storedData = config.Data();

    cycleMap(leftMouseBtnCursor, storedData.leftClickButton);
    cycleMap(rightMouseBtnCursor, storedData.rightClickButton);
    cycleMap(middleMouseBtnCursor, storedData.middleClickButton);
    cycleMap(mouseWheelAnalogCursor, storedData.mouseWheelAnalog);

    mouseSensitivity = storedData.mouseSensitivity;
    mouseOnConnect = storedData.mouseOnConnect;
    mouseToggleKey = storedData.mouseModeToggleKey;
    mouseToggleTime = storedData.mouseModeToggleTime;

    SetupText();
}

void MouseMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);
    if(kDown & HidNpadButton_Down)
        ++selectionCursor;
    else if(kDown & HidNpadButton_Up)
        --selectionCursor;

    if(kDown & HidNpadButton_A)
    {
        ChangeParam(*selectionCursor, 1);
        UpdateUI(*selectionCursor);
    }
    else if(kDown & HidNpadButton_B)
    {
        ChangeParam(*selectionCursor, -1);
        UpdateUI(*selectionCursor);
    }
        
}

void MouseMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    for(auto const& item : textElements)
    {
        if(item.first == *selectionCursor)
            item.second.Render(renderer, font, highlightColour);
        else
            item.second.Render(renderer, font);
    }
}


mouse::MouseConfig const MouseMenu::Settings() const
{
    return mouse::MouseConfig{
        .leftClickButton = leftMouseBtnCursor.KeyPair().first,
        .rightClickButton = rightMouseBtnCursor.KeyPair().first,
        .middleClickButton = middleMouseBtnCursor.KeyPair().first,
        .mouseSensitivity = mouseSensitivity,
        .mouseWheelAnalog = mouseWheelAnalogCursor.KeyPair().first,
        .mouseModeToggleKey = mouseToggleKey,
        .mouseModeToggleTime = mouseToggleTime,
        .mouseOnConnect = mouseOnConnect
    };
}

bool MouseMenu::keyAssigned(HidNpadButton const desired, std::initializer_list<HidNpadButton> const taken)
{
    auto result = std::find(taken.begin(), taken.end(), desired);
    return result != taken.end();
}

void MouseMenu::cycleMouseBtnCursor(UnorderedMapCirculator<HidNpadButton, std::string> & cursor, int direction, std::initializer_list<HidNpadButton> const others)
{
    do
    {
        cursor += direction;
    } while(keyAssigned(cursor.KeyPair().first, others));
}

void MouseMenu::ChangeParam(mouse::MouseParameters param, int value)
{
    switch(param)
    {
        case mouse::MouseParameters::LeftMouseButton:
        {
            auto otherKeys = {rightMouseBtnCursor.KeyPair().first, middleMouseBtnCursor.KeyPair().first};
            cycleMouseBtnCursor(leftMouseBtnCursor, value, otherKeys);
        }
        break;

        case mouse::MouseParameters::RightMouseButton:
        {
            auto otherKeys = {leftMouseBtnCursor.KeyPair().first, middleMouseBtnCursor.KeyPair().first};
            cycleMouseBtnCursor(rightMouseBtnCursor, value, otherKeys);
        }
        break;

        case mouse::MouseParameters::MiddleMouseButton:
        {
            auto otherKeys = {leftMouseBtnCursor.KeyPair().first, rightMouseBtnCursor.KeyPair().first};
            cycleMouseBtnCursor(middleMouseBtnCursor, value, otherKeys);
        }
        break;

        case mouse::MouseParameters::MouseWheelAnalog:
            mouseWheelAnalogCursor += value;
            break;

        case mouse::MouseParameters::MouseOnConnect:
            mouseOnConnect = !mouseOnConnect;
            break;

        case mouse::MouseParameters::MouseSensitivity:
            mouseSensitivity = keyboardNumber(mouse::minMouseSensitivity, mouse::maxMouseSensitivity, mouseSensitivity);
            break;

        case mouse::MouseParameters::MouseModeToggleKey:
        {
            if(value <= 0)
                mouseToggleKey = mouse::defaultMouseModeToggleKey;
            else
            {
                auto displayText = Text{};
                displayText.colour = {200, 140, 120, 255};
                displayText.x = 500;
                displayText.y = 360;
                displayText.centered = true;

                auto titleText = Text{};
                titleText.colour = {120, 120, 200, 255};
                titleText.x = 525;
                titleText.y = 300;
                titleText.centered = true;
                titleText.value = "Mouse Mode Toggle Binding";

                auto const screenRenderer = MainScreenRenderer();
                auto const rendererRef = screenRenderer->Renderer();
                auto const fontRef = mainSystemFont();
                
                auto renderer = [&](std::string str)
                {
                    screenRenderer->ClearScreen({0,0,0,255});

                    titleText.Render(rendererRef, fontRef);

                    displayText.value = str;
                    displayText.Render(rendererRef, fontRef);

                    screenRenderer->PresentScreen();
                };

                mouseToggleKey = monitorButtons(renderer);
            }
        }
        break;

        case mouse::MouseParameters::MouseModeToggleTime:
        {
            if(value <= 0)
                mouseToggleTime = mouse::defaultMouseModeToggleTime;
            else
            {
                auto const currentTime = timeutil::nanoToSecond(mouseToggleTime);
                auto const maxTime = timeutil::nanoToSecond(mouse::maxMouseModeToggleTime);
                auto const minTime = timeutil::nanoToSecond(mouse::minMouseModeToggleTime);
                auto const toggleTime = keyboardDecimal(minTime, maxTime, currentTime);

                mouseToggleTime = timeutil::secondToNano(toggleTime);
            }
        }
        break;
    }
}

void MouseMenu::UpdateUI(mouse::MouseParameters param)
{
    auto prefix = mouse::mouseParamsDesc.at(param);

    auto updateElementText = [&](auto const & str) {
        textElements[param].value = prefix + ": " + str;
    };

    switch(param)
    {
        case mouse::MouseParameters::LeftMouseButton:
        {
            auto str = *leftMouseBtnCursor;
            updateElementText(str);
        }
        break;

        case mouse::MouseParameters::RightMouseButton:
        {
            auto str = *rightMouseBtnCursor;
            updateElementText(str);
        }
        break;

        case mouse::MouseParameters::MiddleMouseButton:
        {
            auto str = *middleMouseBtnCursor;
            updateElementText(str);
        }
        break;

        case mouse::MouseParameters::MouseWheelAnalog:
        {
            auto str = *mouseWheelAnalogCursor;
            updateElementText(str);
        }
        break;

        case mouse::MouseParameters::MouseOnConnect:
        {
            auto str = mouseOnConnect ? "yes" : "no";
            updateElementText(str);
        }
        break;

        case mouse::MouseParameters::MouseSensitivity:
        {
            auto str = std::to_string(mouseSensitivity);
            updateElementText(str);
        }
        break;

        case mouse::MouseParameters::MouseModeToggleKey:
        {
            auto btns = controller::switchButtonsToString(mouseToggleKey);
            if(btns.size() != 0)
            {
                auto str = btns[0];
                for(auto i = 1U; i < btns.size(); ++i)
                    str += "+" + btns[i];
                
                updateElementText(str);
            }
            else
                updateElementText("");
        }
        break;

        case mouse::MouseParameters::MouseModeToggleTime:
        {
            auto seconds = timeutil::nanoToSecond(mouseToggleTime);
            auto str = std::to_string(seconds) + "s";
            updateElementText(str);
        }
        break;
    }
}

void MouseMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;

    auto params = mouse::mouseParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = colours::white;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}