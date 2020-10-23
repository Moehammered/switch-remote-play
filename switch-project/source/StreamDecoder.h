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
        StreamDecoder(AVCodecParameters const * const codecConfig, bool skipFrames);

        bool Initialised();

        bool DecodeFramePacket(const AVPacket& packet);

        void Cleanup();

        const AVFrame& DecodedFrame();

        void Flush();
        
    private:
        AVCodec* codec;
        AVCodecContext* context;
        AVFrame* currentFrame;

        uint64_t lastPacketTime;
        const double targetFrameTime = 1.0/60.0; //1 frame every 60th of a second
        const double NANO_TO_SECONDS = 1000000000.0;
        uint64_t frameSkipCount;
        bool skipFrames;
};

#endif
