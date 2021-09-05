#include "MouseMenu.h"

MouseMenu::MouseMenu() : Menu(),
virtualTouchPage{}, simulatedTouchPage{},
// menus{
//     { mouse::TouchScreenMode::VirtualTouch, &virtualTouchPage }
// },
advancedOptions{false}, pageText{}
{
    title.y += 30;
}

void MouseMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);
    auto cycleKeys = HidNpadButton_Left | HidNpadButton_Right;
    if(kDown & cycleKeys)
        advancedOptions = !advancedOptions;
    
    if(advancedOptions)
        virtualTouchPage.ProcessInput(pad);
    else
        simulatedTouchPage.ProcessInput(pad);
}

void MouseMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    if(advancedOptions)
        virtualTouchPage.Render(renderer, font);
    else
        simulatedTouchPage.Render(renderer, font);
}

mouse::MouseConfig const MouseMenu::Settings() const
{
    return {};
}