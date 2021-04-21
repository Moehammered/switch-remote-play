#ifndef __ENCODERMENU_H__
#define __ENCODERMENU_H__

#include "Menu.h"
#include "../network/NetworkData.h"
#include <vector>
#include "../utils/ArrayCirculator.h"
#include "H264Menu.h"
#include "H264AmfMenu.h"
#include "../system/SystemSetup.h"
class EncoderMenu : public Menu
{
    public:
        EncoderMenu();
        
        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        EncoderConfig const Settings() const;

    private:
        H264Menu h264Menu;
        H264AmfMenu amdMenu;
        std::vector<Menu*> menus;
        ArrayCirculator<Menu*, std::vector> menuCursor;
};

#endif
