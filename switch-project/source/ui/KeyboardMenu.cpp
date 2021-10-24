#include "KeyboardMenu.h"
#include "../system/ButtonWatch.h"
#include "../ScreenRenderer.h"
#include "../system/SystemSetup.h"
#include "../keyboard/KeyboardConfiguration.h"
#include <algorithm>

namespace
{
    auto constexpr itemsPerCol = 9;
}

KeyboardMenu::KeyboardMenu() : Menu(),
    selectionCursor{keyboard::keyParamsList},
    textElements{}, bindingMap{}
{
    title.y += 30;
    title.value = "Keyboard Configuration";

    auto const configData = KeyboardConfiguration().Data();
    for(auto const& binding : configData.bindings)
    {
        if(binding.button != 0)
            bindingMap[binding.key] = binding.button;
    }

    SetupText();
}

void KeyboardMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);
    if(kDown & HidNpadButton_Down)
        ++selectionCursor;
    else if(kDown & HidNpadButton_Up)
        --selectionCursor;
    else if(kDown & HidNpadButton_Left)
        selectionCursor -= itemsPerCol;
    else if(kDown & HidNpadButton_Right)
        selectionCursor += itemsPerCol;

    if(kDown & HidNpadButton_A)
    {
        ChangeParam(*selectionCursor, false);
        UpdateUI(*selectionCursor);
    }
    else if(kDown & HidNpadButton_B)
    {
        ChangeParam(*selectionCursor, true);
        UpdateUI(*selectionCursor);
    }
}

void KeyboardMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
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

void KeyboardMenu::ChangeParam(keyboard::KeyParameter param, bool clear)
{
    if(clear)
        bindingMap[param] = 0;
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
        titleText.value = "Keyboard Button Binding";

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

        auto keys = monitorButtons(renderer);

        bindingMap[param] = keys;
    }
}

keyboard::KeyboardConfig const KeyboardMenu::Settings() const
{
    auto config = keyboard::KeyboardConfig{};

    auto const maxKeyCount = std::min(keyboard::totalSupportedKeys, keyboard::keyParamsList.size());

    for(auto i = 0U; i < maxKeyCount; ++i)
    {
        auto keyBinding = keyboard::KeyboardBinding{};
        keyBinding.key = keyboard::keyParamsList[i];

        auto binding = bindingMap.find(keyBinding.key);
        if(binding != bindingMap.end())
            keyBinding.button = binding->second;
        else
            keyBinding.button = 0;

        config.bindings[i] = keyBinding;
    }

    return config;
}

void KeyboardMenu::UpdateUI(keyboard::KeyParameter param)
{
    auto prefix = keyboard::keyParamsDesc.at(param);
    auto binding = bindingMap.find(param);
    if(binding != bindingMap.end())
    {
        auto btns = (HidNpadButton)binding->second;
        auto btnStrings = controller::switchButtonsToString(btns);
        if(btnStrings.size() > 1)
        {
            auto str = btnStrings[0];
            for(auto i = 1U; i < btnStrings.size(); ++i)
                str += "+" + btnStrings[i];

            textElements[param].value = prefix + ": " + str;
        }
        else if(btnStrings.size() == 1)
        {
            auto btnStr = btnStrings[0];
            textElements[param].value = prefix + ": " + btnStr;
        }
        else
            textElements[param].value = prefix + ":";
    }
    else
        textElements[param].value = prefix + ":";
}

void KeyboardMenu::SetupText()
{
    const int settingTextX = 100;
    const int xOffset = 600;
    const int yOffset = title.y + 40;
    const int ySpace = 30;
    int counter = 0;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = keyboard::keyParamsList;
    for(auto& p : params)
    {
        auto col = counter / itemsPerCol;
        auto row = counter % itemsPerCol;
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX + xOffset * col;
        textElements[p].y = yOffset + ySpace * row;
        UpdateUI(p);
        counter++;
    }
}