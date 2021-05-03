#ifndef __GENERICCODECMENU_H__
#define __GENERICCODECMENU_H__

#include "Menu.h"
#include "../codec/general/GenericCodec.h"
#include "../utils/ArrayCirculator.h"
#include "../utils/UnorderedMapCirculator.h"

class GenericCodecMenu : public Menu
{
    public:
        GenericCodecMenu();
        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        VideoData const VideoCodecSettings() const;

    private:
        std::unordered_map<VideoParameters, Text> textElements;
        GenericCodec codec;
        VideoParameters selected;

        void UpdateUI(VideoParameters param);

        void SetupText();
};

#endif
