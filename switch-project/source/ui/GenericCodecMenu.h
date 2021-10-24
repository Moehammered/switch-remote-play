#ifndef __GENERICCODECMENU_H__
#define __GENERICCODECMENU_H__

#include "Menu.h"
#include "srp/codec/general/GenericCodec.h"
#include "srp/utils/ArrayCirculator.h"
#include "srp/utils/UnorderedMapCirculator.h"

class GenericCodecMenu : public Menu
{
    public:
        GenericCodecMenu();
        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        codec::VideoData const VideoCodecSettings() const;

    private:
        std::unordered_map<codec::VideoParameters, Text> textElements;
        GenericCodec codec;
        codec::VideoParameters selected;

        void UpdateUI(codec::VideoParameters param);

        void SetupText();
};

#endif
