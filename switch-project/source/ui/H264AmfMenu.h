#ifndef __H264AMFMENU_H__
#define __H264AMFMENU_H__

#include "Menu.h"
#include "srp/codec/h264_amf/H264AmfCodec.h"
#include "srp/utils/ArrayCirculator.h"
#include "srp/utils/UnorderedMapCirculator.h"

class H264AmfMenu : public Menu
{
    public:
        H264AmfMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        h264amf::H264AmfData const Settings() const;

    private:
        std::unordered_map<h264amf::H264AmfParameters, Text> textElements;
        H264AmfCodec codec;
        h264amf::H264AmfParameters selected;

        void UpdateUI(h264amf::H264AmfParameters param);

        void SetupText();
};

#endif
