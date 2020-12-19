#ifndef __FFMPEGCONFIGRENDERER_H__
#define __FFMPEGCONFIGRENDERER_H__

#include "../network/NetworkData.h"
#include <SDL2/SDL_render.h>
#include "Text.h"

class FfmpegConfigRenderer
{
    public:
        FfmpegConfigRenderer();

        void Config(FFMPEG_Config const & config);

        void Render(SDL_Renderer * const renderer, FC_Font * const font) const;

        FFMPEG_Config const & DisplayedConfig() const;

    private:
        FFMPEG_Config settings;
        Text framerate;
        Text videoResolution;
        Text videoScale;
        Text bitrate;
        Text vsync;

        void SetText(FFMPEG_Config const & config);

        std::string ResToString(int16_t x, int16_t y);
        std::string VsyncToString(int16_t vsyncMode);
};

#endif
