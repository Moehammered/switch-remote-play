#include "ControllerMenu.h"
#include "../utils/EnumMapper.h"
#include "../controller/ControllerConfiguration.h"
#include "../system/SoftwareKeyboard.h"
#include "../system/ButtonWatch.h"
#include "../ScreenRenderer.h"
#include "../system/SystemSetup.h"

ControllerMenu::ControllerMenu() : Menu(),
    textElements{}, paramCursor{controller::ParamsList},
    modeCursor{controller::controllerModeDesc},
    buttonMapCursor{controller::controlMapDesc},
    leftAnalogMapCursor{controller::analogMapDesc},
    rightAnalogMapCursor{controller::analogMapDesc},
    controllerCount{controller::DefaultControllerCount},
    homeButton{controller::DefaultHomeButton},
    homeButtonTriggerTime{controller::DefaultHomeButtonTriggerTime}
{
    title.value = "Controller Menu";
    title.y += 30;

    auto config = ControllerConfiguration{};
    auto settings = config.Data();
    cycleMap(modeCursor, settings.controllerMode);
    cycleMap(buttonMapCursor, settings.controllerMap);
    cycleMap(leftAnalogMapCursor, settings.leftAnalogMap);
    cycleMap(rightAnalogMapCursor, settings.rightAnalogMap);

    controllerCount = settings.controllerCount;
    homeButton = settings.homeButton;
    homeButtonTriggerTime = settings.homeButtonTriggerTime;

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
        .rightAnalogMap = rightAnalogMapCursor.KeyPair().first,
        .homeButton = homeButton,
        .homeButtonTriggerTime = homeButtonTriggerTime,
        .controllerCount = controllerCount
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

        case controller::Parameters::HomeButton:
        {
            if(direction <= 0)
                homeButton = controller::DefaultHomeButton;
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
                titleText.value = "Home Button Binding";

                auto const screenRenderer = MainScreenRenderer();
                auto const rendererRef = screenRenderer->Renderer();
                auto const fontRef = MainSystemFont();
                
                auto renderer = [&](std::string str)
                {
                    screenRenderer->ClearScreen({0,0,0,255});

                    titleText.Render(rendererRef, fontRef);

                    displayText.value = str;
                    displayText.Render(rendererRef, fontRef);

                    screenRenderer->PresentScreen();
                };

                homeButton = MonitorKeys(renderer);
            }
        }
        break;

        case controller::Parameters::HomeButtonTriggerTime:
        {
            if(direction <= 0)
                homeButtonTriggerTime = controller::DefaultHomeButtonTriggerTime;
            else
            {
                auto const currentTime = timeutil::nanoToSecond(homeButtonTriggerTime);
                auto const maxTime = timeutil::nanoToSecond(controller::MaxHomeButtonTriggerTime);
                auto const minTime = timeutil::nanoToSecond(controller::MinHomeButtonTriggerTime);
                auto const toggleTime = KeyboardDecimal(minTime, maxTime, currentTime);

                homeButtonTriggerTime = timeutil::secondToNano(toggleTime);
            }
        }
        break;

        case controller::Parameters::ControllerCount:
            controllerCount = (int16_t)KeyboardNumber(controller::MinControllerCount, controller::MaxControllerCount, controllerCount);
        break;
    }
}

void ControllerMenu::UpdateUI(controller::Parameters param)
{
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

        case controller::Parameters::HomeButton:
        {
            auto btns = controller::SwitchButtonsToString(homeButton);
            if(btns.size() != 0)
            {
                auto str = btns[0];
                for(auto i = 1U; i < btns.size(); ++i)
                    str += "+" + btns[i];
                
                element.value = desc + ": " + str;
            }
            else
                element.value = desc + ":";
        }
        break;
        
        case controller::Parameters::HomeButtonTriggerTime:
        {
            auto seconds = timeutil::nanoToSecond(homeButtonTriggerTime);
            auto str = std::to_string(seconds) + "s";
            element.value = desc + ": " + str;
        }
        break;

        case controller::Parameters::ControllerCount:
            element.value = desc + ": " + std::to_string(controllerCount);
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