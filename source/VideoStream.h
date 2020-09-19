#ifndef __VIDEOSTREAM__H_
#define __VIDEOSTREAM__H_

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/frame.h>
}
#include <string>
#include "ScreenRenderer.h"
#include <thread>
#include <mutex>

struct StreamConfigData
{
    int streamTechnique;
    int framerate;
    bool streamOn;
    bool useFrameSkip;
    bool quitApp;
    SDL_Color bgCol;
    SDL_Color textColour;
};

class VideoStream
{
    public:
        //~VideoStream();

        void Initialise();

        bool WaitForStream(std::string url);

        void StreamVideoViaDecoder(ScreenRenderer& renderer, const StreamConfigData& config);

        void StreamVideo(ScreenRenderer& renderer);

        void StreamVideoQueue(ScreenRenderer& renderer);

        void Cleanup();
    private:
        AVFormatContext* streamFormat;
        //AVPacket streamPacket;
        AVStream* stream;
        int streamIndex;
        AVCodec* decoder;
        AVCodecContext* decoderContext;
        AVMediaType streamMediaType;
        AVFrame* frame;

        uint8_t* videoData[4];
        int videoLinesizes[4];
};

#endif
