#include "ControllerMenu.h"
#include "../utils/EnumMapper.h"
#include "../controller/ControllerConfiguration.h"

ControllerMenu::ControllerMenu() : Menu(),
textElements{}, paramCursor{controller::ParamsList},
modeCursor{controller::controllerModeDesc},
buttonMapCursor{controller::controlMapDesc},
leftAnalogMapCursor{controller::analogMapDesc},
rightAnalogMapCursor{controller::analogMapDesc}
{
    title.value = "Controller Menu";
    title.y += 30;

    auto config = ControllerConfiguration{};
    auto settings = config.Data();
    cycleMap(modeCursor, settings.controllerMode);
    cycleMap(buttonMapCursor, settings.controllerMap);
    cycleMap(leftAnalogMapCursor, settings.leftAnalogMap);
    cycleMap(rightAnalogMapCursor, settings.rightAnalogMap);

    SetupText();
}

void ControllerMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton::HidNpadButton_Up)
        --paramCursor;
    else if (kDown & HidNpadButton::HidNpadButton_Down)
        ++paramCursor;

    if(kDown & HidNpadButton::HidNpadButton_A)
    {
        UpdateSetting(*paramCursor, 1);
        UpdateUI(*paramCursor);
    }
    else if(kDown & HidNpadButton::HidNpadButton_B)
    {
        UpdateSetting(*paramCursor, -1);
        UpdateUI(*paramCursor);
    }
}

void ControllerMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);

    //use this later to render a rect behind the text elements
    //SDL_SetRenderDrawColor(renderer, backgroundColour.r, backgroundColour.g, backgroundColour.b, backgroundColour.a);
    //sdl fill rect here
    for(auto const & t : textElements)
    {
        if(t.first == *paramCursor)
            t.second.Render(renderer, font, highlightColour);
        else
            t.second.Render(renderer, font);
    }
}

controller::ControllerConfig const ControllerMenu::Settings() const
{
    return controller::ControllerConfig{
        .controllerMode = modeCursor.KeyPair().first,
        .controllerMap = buttonMapCursor.KeyPair().first,
        .leftAnalogMap = leftAnalogMapCursor.KeyPair().first,
        .rightAnalogMap = rightAnalogMapCursor.KeyPair().first
    };
}

void ControllerMenu::UpdateSetting(controller::Parameters param, int direction)
{
    switch(param)
    {
        case controller::Parameters::Mode:
            modeCursor += direction;
        break;

        case controller::Parameters::ButtonMapping:
            buttonMapCursor += direction;
        break;

        case controller::Parameters::LeftAnalogMapping:
            leftAnalogMapCursor += direction;
        break;
              
        case controller::Parameters::RightAnalogMapping:
            rightAnalogMapCursor += direction;
        break;
    }
}

void ControllerMenu::UpdateUI(controller::Parameters param)
{
    auto boolToStr = [](bool val)
    {
        return val == true ? "yes" : "no";
    };

    auto& element = textElements[param];
    auto desc = enumToStr(controller::ParamsDesc, param);
    switch(param)
    {
        case controller::Parameters::Mode:
            element.value = desc + ": " + *modeCursor;
        break;

        case controller::Parameters::ButtonMapping:
            element.value = desc + ": " + *buttonMapCursor;
        break;

        case controller::Parameters::LeftAnalogMapping:
            element.value = desc + ": " + *leftAnalogMapCursor;
        break;

        case controller::Parameters::RightAnalogMapping:
            element.value = desc + ": " + *rightAnalogMapCursor;
        break;
    }
}

void ControllerMenu::SetupText()
{
    const int settingTextX = 100;
    const int yOffset = title.y + 15;
    const int ySpace = 45;
    int counter = 1;
    SDL_Color constexpr textColour {.r = 255, .g = 255, .b = 255, .a = 255};

    auto params = controller::ParamsList;
    for(auto& p : params)
    {
        textElements[p] = Text{};
        textElements[p].colour = textColour;
        textElements[p].x = settingTextX;
        textElements[p].y = yOffset + ySpace * counter++;
        UpdateUI(p);
    }
}