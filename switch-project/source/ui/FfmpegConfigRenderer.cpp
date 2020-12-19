#include "FfmpegConfigRenderer.h"

FfmpegConfigRenderer::FfmpegConfigRenderer()
    : settings{}, framerate{}, videoResolution{},
    videoScale{}, bitrate{}, vsync{}
{
    int constexpr x = 100;
    int constexpr y = 350;
    framerate.x = x;
    framerate.y = y;

    videoResolution.x = x;
    videoResolution.y = y + 50;
    videoScale.x = x;
    videoScale.y = y + 100;

    bitrate.x = x;
    bitrate.y = y + 150;

    vsync.x = x;
    vsync.y = y + 200;

    SDL_Color constexpr white {255, 255, 255, 255};
    framerate.colour = white;
    videoResolution.colour = white;
    videoScale.colour = white;
    bitrate.colour = white;
    vsync.colour = white;
}

void FfmpegConfigRenderer::Config(FFMPEG_Config const & config)
{
    settings = config;
    SetText(settings);
}

FFMPEG_Config const & FfmpegConfigRenderer::DisplayedConfig() const
{
    return settings;
}

void FfmpegConfigRenderer::Render(SDL_Renderer * const renderer, FC_Font * const font) const
{
    framerate.Render(renderer, font);
    videoResolution.Render(renderer, font);
    videoScale.Render(renderer, font);
    bitrate.Render(renderer, font);
    vsync.Render(renderer, font);
}

void FfmpegConfigRenderer::SetText(FFMPEG_Config const & config)
{
    framerate.value =       "Desired Framerate:  " + std::to_string(config.desiredFrameRate);
    videoResolution.value = "Desktop Resolution: " + ResToString(config.videoX, config.videoY);
    videoScale.value =      "Switch Resolution:  " + ResToString(config.scaleX, config.scaleY);
    bitrate.value =         "Bitrate(KB/s):      " + std::to_string(config.bitrateKB);
    vsync.value =           "Vsync Mode:         " + VsyncToString(config.vsyncMode);
}

std::string FfmpegConfigRenderer::ResToString(int16_t x, int16_t y)
{
    return std::to_string(x) + "x" + std::to_string(y);
}

std::string FfmpegConfigRenderer::VsyncToString(int16_t vsyncMode)
{
    switch(vsyncMode)
    {
        case -1:
            return "auto (generally bad - ffmpeg decides.)";

        case 0:
            return "passthrough (OK - render from timestamps)";

        case 1:
            return "constant frame rate (force fps - latency)";

        case 2:
            return "variable frame rate";

        case 3:
            return "drop time (great - render ignoring timestamps)";

        default:
            return "unknown";
    }
}