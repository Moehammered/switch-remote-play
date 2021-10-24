#ifndef __VIDEOSTREAM__H_
#define __VIDEOSTREAM__H_

#include "../ScreenRenderer.h"
#include "srp/decoder/DecoderOptions.h"
#include <string>
#include <atomic>
extern "C"
{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/frame.h>
}

class VideoStream
{
    public:
        VideoStream();
        bool WaitForStream(DecoderData decoderSettings, uint16_t port);

        bool Read(AVPacket& output);

        AVStream const * const StreamInfo();

        void CloseStream();

        void Cleanup();

    private:
        AVMediaType const streamMediaType;
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
