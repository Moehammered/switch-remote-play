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
    CONTROLLER_BTN_MAP,
    MOUSE_SENSITIVITY,
    MOUSE_ON_CONNECT,
    CONTROLLER_MENU_COUNT
};

class ControllerMenu : public Menu
{
    public:
        ControllerMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        void IncreaseSetting();
        void DecreaseSetting();
        void SelectNext();
        void SelectPrevious();
        ControllerConfig const Settings();

    private:
        ControllerMenuItems selectedItem;

        std::array<int32_t, ControllerMenuItems::CONTROLLER_MENU_COUNT> settingsIndices;

        std::array<Text, ControllerMenuItems::CONTROLLER_MENU_COUNT> settingsText;

        void Update();
};

#endif
