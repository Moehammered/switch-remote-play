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
#include "../ScreenRenderer.h"
#include "../dataHelpers/DecoderFlags.h"

class VideoStream
{
    public:
        VideoStream();
        bool WaitForStream(DecoderConfiguration decoderSettings, uint16_t port);

        bool Read(AVPacket& output);

        AVStream const * const StreamInfo();

        void CloseStream();

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
