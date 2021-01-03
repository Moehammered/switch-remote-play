#ifndef __HELPMENU_H__
#define __HELPMENU_H__

#include "Menu.h"
#include <array>

enum HelpPageItems : int32_t
{
    NETWORK_PAGE,
    STREAM_PAGE,
    HELP_PAGE_COUNT
};
class HelpMenu : public Menu
{
    public:
        HelpMenu();

        void ProcessInput(PadState const & pad) override;

        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    private:
        std::array<Text, HelpPageItems::HELP_PAGE_COUNT> pages;
        Text currentPageHeader;
        int32_t selectedPage;

        void Update();
};

#endif
