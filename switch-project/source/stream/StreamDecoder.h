#ifndef __STREAMDECODER_H__
#define __STREAMDECODER_H__

extern "C"
{
    #include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/frame.h>
}

class StreamDecoder
{
    public:
        StreamDecoder(AVCodecParameters const * const codecConfig);

        bool Initialised();

        bool DecodeFramePacket(const AVPacket& packet);

        void Cleanup();

        const AVFrame& DecodedFrame();

        void Flush();
        
    private:
        AVCodec* codec;
        AVCodecContext* context;
        AVFrame* currentFrame;
};

#endif
