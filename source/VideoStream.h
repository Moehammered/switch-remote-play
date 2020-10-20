#ifndef __VIDEOSTREAM__H_
#define __VIDEOSTREAM__H_

extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/frame.h>
}
#include <string>
#include <atomic>
#include "ScreenRenderer.h"

class VideoStream
{
    public:
        bool WaitForStream(std::string url);

        //need to update this to work interatively instead of blocking
        void StreamVideoViaDecoder(ScreenRenderer& renderer, std::atomic_bool& streamOn);

        void Cleanup();

    private:
        const AVMediaType streamMediaType = AVMEDIA_TYPE_VIDEO;
        AVFormatContext* streamFormat;
        AVStream* stream;
        int streamIndex;
        AVCodec* decoder;
        AVCodecContext* decoderContext;
        AVFrame* frame;

        uint8_t* videoData[4];
        int videoLinesizes[4];
};

#endif
