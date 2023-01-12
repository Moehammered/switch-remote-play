#ifndef __FFMPEGAUDIOSTREAM_H__
#define __FFMPEGAUDIOSTREAM_H__

#include <string>
extern "C"
{
    #include <libavutil/frame.h>
    #include <libavformat/avformat.h>
}

namespace ffmpeg
{
    std::string const avErrorToString(int errCode);

    class AudioStream
    {
        public:
            AudioStream();

            bool openStream(std::string const& uri);
            void closeStream();

            bool nextPacket();
            AVFrame const * const nextFrame();

            bool isOpen() const;
            AVCodecContext const * const CodecContext() const;

        private:
            AVFormatContext* formatContext;
            AVCodec*    streamCodec;
            AVCodecContext* codecContext;
            AVPacket* dataPacket;
            AVFrame* dataFrame;
            
            void cleanup();
    };
}

#endif
