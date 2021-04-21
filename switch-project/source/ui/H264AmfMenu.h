#ifndef __H264AMFMENU_H__
#define __H264AMFMENU_H__

#include "Menu.h"
#include "../codec/h264_amf/H264AmfCodec.h"
#include "../utils/ArrayCirculator.h"
#include "../utils/UnorderedMapCirculator.h"

class H264AmfMenu : public Menu
{
    public:
        H264AmfMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    private:
        std::unordered_map<h264amf::Parameters, Text> textElements;
        H264AmfCodec codec;
        h264amf::Parameters selected;

        void UpdateUI(h264amf::Parameters param);

        void SetupText();
};

#endif
