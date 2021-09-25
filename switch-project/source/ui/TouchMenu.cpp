#include "TouchMenu.h"
#include "../touch/TouchConfiguration.h"

auto const pageMenuText = "Use D-Pad left or right to choose touch mode.";

TouchMenu::TouchMenu() : Menu(),
virtualTouchPage{}, simulatedTouchPage{},
touchMode{touch::TouchScreenModeStr},
touchModeText{}, pageText{}
{
    title.value = "Touch Configuration";
    title.y += 30;

    auto config = TouchConfiguration{};
    auto savedMode = config.Data().touchMode;
    cycleMap(touchMode, savedMode);

    touchModeText.centered = true;
    touchModeText.y = title.y + 30;
    touchModeText.x = title.x + 20;
    touchModeText.colour = {50, 180, 220, 255};
    touchModeText.value = *touchMode;

    pageText.centered = true;
    pageText.y = touchModeText.y + 30;
    pageText.x = title.x - 100;
    pageText.colour = {200, 200, 200, 255};
    pageText.value = pageMenuText;
}

void TouchMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);
    auto cycleKeys = HidNpadButton_Left | HidNpadButton_Right;
    if(kDown & cycleKeys)
    {
        ++touchMode;
        touchModeText.value = *touchMode;
    }
    
    switch(touchMode.KeyPair().first)
    {
        case touch::TouchScreenMode::VirtualTouch:
            virtualTouchPage.ProcessInput(pad);
            break;

        case touch::TouchScreenMode::SimulatedMouse:
            simulatedTouchPage.ProcessInput(pad);
            break;
    }
}

void TouchMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    pageText.Render(renderer, font);
    touchModeText.Render(renderer, font);

    auto const & selectedMode = touchMode.KeyPair().first;
    switch(selectedMode)
    {
        case touch::TouchScreenMode::VirtualTouch:
            virtualTouchPage.Render(renderer, font);
            break;

        case touch::TouchScreenMode::SimulatedMouse:
            simulatedTouchPage.Render(renderer, font);
            break;
    }
}

touch::TouchConfig const TouchMenu::Settings() const
{
    auto config = touch::TouchConfig{};
    config.touchMode = touchMode.KeyPair().first;

    switch(config.touchMode)
    {
        case touch::TouchScreenMode::VirtualTouch:
            config.virtualTouchSettings = VirtualTouchSettings();
            break;

        case touch::TouchScreenMode::SimulatedMouse:
            config.simulatedTouchMouseSettings = SimulatedTouchSettings();
            break;
    }

    return config;
}


touch::VirtualTouchConfig const TouchMenu::VirtualTouchSettings() const
{
    return virtualTouchPage.Settings();
}

touch::SimulatedTouchConfig const TouchMenu::SimulatedTouchSettings() const
{
    return simulatedTouchPage.Settings();
}
