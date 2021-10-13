#include "KeyboardMenu.h"

KeyboardMenu::KeyboardMenu() : Menu(),
    selectionCursor{keyboard::ParamsList},
    textElements{}, bindingMap{}
{
    title.y += 30;
    title.value = "Keyboard Configuration [Incomplete]";

    //config loads here
    auto const & loadedBindings = keyboard::DefaultBindings;
    for(auto const& binding : loadedBindings)
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

}

keyboard::KeyboardConfig const KeyboardMenu::Settings() const
{
    return {};
}

void KeyboardMenu::UpdateUI(keyboard::KeyParameter param)
{
    auto prefix = keyboard::ParamsDesc.at(param);
    auto binding = bindingMap.find(param);
    if(binding != bindingMap.end())
    {
        auto btn = (HidNpadButton)binding->second;
        auto btnStr = controller::SwitchButtonToString(btn);
        textElements[param].value = prefix + ": " + btnStr;
    }
    else
        textElements[param].value = prefix + ":";
}

void KeyboardMenu::SetupText()
{
    const int settingTextX = 100;
    const int xOffset = 600;
    const int yOffset = title.y + 30;
    const int ySpace = 30;
    auto const itemsPerCol = 9;
    int counter = 0;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = keyboard::ParamsList;
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