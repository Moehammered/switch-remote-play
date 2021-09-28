#include "MouseMenu.h"
#include "../system/SoftwareKeyboard.h"
#include "../mouse/MouseConfiguration.h"
#include <algorithm>

MouseMenu::MouseMenu() : Menu(),
selectionCursor{mouse::ParamsList},
textElements{}, leftMouseBtnCursor{controller::mouseButtonOptions},
rightMouseBtnCursor{controller::mouseButtonOptions},
middleMouseBtnCursor{controller::mouseButtonOptions},
mouseWheelAnalogCursor{controller::analogStickOptions},
mouseOnConnect{true}, mouseSensitivity{mouse::DefaultMouseSensitivity}
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
        .mouseSensitivity = (int16_t)mouseSensitivity,
        .mouseWheelAnalog = mouseWheelAnalogCursor.KeyPair().first,
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

void MouseMenu::ChangeParam(mouse::Parameters param, int value)
{
    switch(param)
    {
        case mouse::Parameters::LeftMouseButton:
        {
            auto otherKeys = {rightMouseBtnCursor.KeyPair().first, middleMouseBtnCursor.KeyPair().first};
            cycleMouseBtnCursor(leftMouseBtnCursor, value, otherKeys);
        }
        break;

        case mouse::Parameters::RightMouseButton:
        {
            auto otherKeys = {leftMouseBtnCursor.KeyPair().first, middleMouseBtnCursor.KeyPair().first};
            cycleMouseBtnCursor(rightMouseBtnCursor, value, otherKeys);
        }
        break;

        case mouse::Parameters::MiddleMouseButton:
        {
            auto otherKeys = {leftMouseBtnCursor.KeyPair().first, rightMouseBtnCursor.KeyPair().first};
            cycleMouseBtnCursor(middleMouseBtnCursor, value, otherKeys);
        }
        break;

        case mouse::Parameters::MouseWheelAnalog:
            mouseWheelAnalogCursor += value;
        break;

        case mouse::Parameters::MouseOnConnect:
            mouseOnConnect = !mouseOnConnect;
        break;

        case mouse::Parameters::MouseSensitivity:
            mouseSensitivity = KeyboardNumber(mouse::MinMouseSensitivity, mouse::MaxMouseSensitivity);
        break;
    }
}

void MouseMenu::UpdateUI(mouse::Parameters param)
{
    auto prefix = mouse::ParamsDesc.at(param);

    auto updateElementText = [&](auto const & str) {
        textElements[param].value = prefix + ": " + str;
    };

    switch(param)
    {
        case mouse::Parameters::LeftMouseButton:
        {
            auto str = *leftMouseBtnCursor;
            updateElementText(str);
        }
        break;

        case mouse::Parameters::RightMouseButton:
        {
            auto str = *rightMouseBtnCursor;
            updateElementText(str);
        }
        break;

        case mouse::Parameters::MiddleMouseButton:
        {
            auto str = *middleMouseBtnCursor;
            updateElementText(str);
        }
        break;

        case mouse::Parameters::MouseWheelAnalog:
        {
            auto str = *mouseWheelAnalogCursor;
            updateElementText(str);
        }
        break;

        case mouse::Parameters::MouseOnConnect:
        {
            auto str = mouseOnConnect ? "yes" : "no";
            updateElementText(str);
        }
        break;

        case mouse::Parameters::MouseSensitivity:
        {
            auto str = std::to_string(mouseSensitivity);
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
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = mouse::ParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}