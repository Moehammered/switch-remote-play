#ifndef __ENCODERMENU_H__
#define __ENCODERMENU_H__

#include "Menu.h"
#include "H264Menu.h"
#include "H264AmfMenu.h"
#include "GenericCodecMenu.h"
#include "srp/network/NetworkData.h"
#include <vector>
#include <unordered_map>

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
        GenericCodecMenu generalMenu;
        std::unordered_map<ffmpeg::VideoCodec, Menu*> const menus;
        bool advancedOptions;
        Text pageText;
};

#endif
