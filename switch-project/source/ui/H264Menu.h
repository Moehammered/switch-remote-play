#ifndef __H264MENU_H__
#define __H264MENU_H__

#include "Menu.h"
#include "srp/codec/h264/H264Codec.h"
#include "srp/utils/ArrayCirculator.h"
#include "srp/utils/UnorderedMapCirculator.h"

class H264Menu : public Menu
{
    public:
        H264Menu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        h264::H264Data const Settings() const;

    private:
        std::unordered_map<h264::H264Parameters, Text> textElements;
        H264Codec codec;
        h264::H264Parameters selected;

        void UpdateUI(h264::H264Parameters param);

        void SetupText();
};

#endif
