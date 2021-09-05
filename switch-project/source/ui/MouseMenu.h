#ifndef __MOUSEMENU_H__
#define __MOUSEMENU_H__

#include "Menu.h"
#include "../mouse/MouseOptions.h"
#include "VirtualTouchMenu.h"
#include "SimulatedTouchMouseMenu.h"

class MouseMenu : public Menu
{
    public:
        MouseMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        mouse::MouseConfig const Settings() const;

    private:
        VirtualTouchMenu virtualTouchPage;
        SimulatedTouchMouseMenu simulatedTouchPage;
        // GenericMouseMenu generalMenu;
        // std::unordered_map<touch::TouchScreenMode, Menu*> const menus;
        bool advancedOptions;
        Text pageText;
};

#endif
