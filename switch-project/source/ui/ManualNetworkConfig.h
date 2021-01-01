#ifndef __MANUALNETWORKCONFIG_H__
#define __MANUALNETWORKCONFIG_H__

#include "Menu.h"
#include <string>
#include <array>
#include "Text.h"

enum NetworkMenuItems : int32_t
{
    IP_SEG_1 = 0,
    IP_SEG_2,
    IP_SEG_3,
    IP_SEG_4,
    SAVE_BUTTON,
    MANUAL_TOGGLE,
    NETWORK_MENU_ITEM_COUNT
};

class NetworkMenu : Menu
{
    public:
        NetworkMenu();

        void ProcessInput(PadState const & pad) override;

        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        bool UseManualIP();

        std::string const ManualIPAddress();

    private:
        Text warningText;
        std::array<Text, NetworkMenuItems::NETWORK_MENU_ITEM_COUNT> textElements;
        std::array<int16_t, 4> ip;
        bool useManualIP;

        std::array<int16_t, 4> LoadManualIP();
        void ProcessIncrease();
        void ProcessDecrease();

        NetworkMenuItems selectedItem;
};

#endif
