#ifndef __TOUCHMENU_H__
#define __TOUCHMENU_H__

#include "Menu.h"
#include "VirtualTouchMenu.h"
#include "SimulatedTouchMouseMenu.h"
#include "srp/utils/UnorderedMapCirculator.h"

class TouchMenu : public Menu
{
    public:
        TouchMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        touch::TouchConfig const Settings() const;

        touch::VirtualTouchConfig const VirtualTouchSettings() const;
        touch::SimulatedTouchConfig const SimulatedTouchSettings() const;

    private:
        VirtualTouchMenu virtualTouchPage;
        SimulatedTouchMouseMenu simulatedTouchPage;
        UnorderedMapCirculator<touch::TouchScreenMode, std::string> touchMode;
        Text touchModeText;
        Text pageText;
};

#endif
