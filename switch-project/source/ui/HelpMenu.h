#ifndef __HELPMENU_H__
#define __HELPMENU_H__

#include "Menu.h"
#include <array>

enum HelpPageItems : int32_t
{
    P1,
    P2,
    HELP_PAGE_COUNT
};

enum HelpPageButtons : int32_t
{
    PREV,
    NEXT,
    HELP_BTN_COUNT
};

class HelpMenu : public Menu
{
    public:
        HelpMenu();

        void ProcessInput(PadState const & pad) override;

        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    private:
        std::array<Text, HelpPageItems::HELP_PAGE_COUNT> pages;
        std::array<Text, HELP_BTN_COUNT> buttons;

        int32_t selectedButton;
        int32_t selectedPage;
};

#endif
