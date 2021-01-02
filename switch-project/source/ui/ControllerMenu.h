#ifndef __CONTROLLERSCREEN_H__
#define __CONTROLLERSCREEN_H__

#include <string>
#include <array>
#include "Menu.h"
#include "Text.h"
#include "../network/NetworkData.h"

enum ControllerMenuItems : int32_t
{
    CONTROLLER_MODE = 0,
    CONFIG_COUNT
};

class ControllerMenu : Menu
{
    public:
        ControllerMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        void IncreaseSetting();
        void DecreaseSetting();
        void SelectNext();
        void SelectPrevious();
        Controller_Config const Settings();

    private:
        int settingIndex;

        std::array<int, ControllerMenuItems::CONFIG_COUNT> settingsIndices;

        std::array<Text, ControllerMenuItems::CONFIG_COUNT> settingsText;

        void UpdateControllerMode();
};

#endif
