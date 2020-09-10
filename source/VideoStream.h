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

class VideoStream
{
    public:
        //~VideoStream();

        void Initialise();

        bool WaitForStream(std::string url);

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
