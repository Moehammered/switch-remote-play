#include "VideoStream.h"
#include "StreamDecoder.h"
#include <iostream>
#include <unordered_map>
extern "C" 
{
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
}

namespace
{
    auto constexpr streamURL = "tcp://0.0.0.0";

    std::string SkipLoopFilterToString(AVDiscard discard)
    {
        switch(discard)
        {
            case AVDISCARD_NONE:
                return "Discard None\n";

            case AVDISCARD_DEFAULT: 
                return "discard useless packets like 0 size packets in avi\n";

            case AVDISCARD_NONREF:
                return "discard all non reference\n";

            case AVDISCARD_BIDIR:
                return "discard all bidirectional frames\n";
            
            case AVDISCARD_NONINTRA: 
                return "discard all non intra frames\n";

            case AVDISCARD_NONKEY:
                return "discard all frames except keyframes\n";

            case AVDISCARD_ALL:
                return "discard all\n";

            default:
                return "unknown";
        }
    }

    void PrintDecoderContextSettings(AVCodecContext const & decoderContext)
    {
        std::cout << "Skip Loop Filter: " << SkipLoopFilterToString(decoderContext.skip_loop_filter);
        std::cout << "\nThread Count: " << decoderContext.thread_count << "\n";
        std::cout << "Threading Type\n";
        if(decoderContext.thread_type == FF_THREAD_SLICE)
            std::cout << "    SLICE: Decode more slices of a SINGLE frame at once\n\n";
        else if(decoderContext.thread_type == FF_THREAD_FRAME)
            std::cout << "    FRAME: Decode more than one frame at once\n\n";
        else
            std::cout << "    Unknown thread_type: " << decoderContext.thread_type << "\n\n";
    }
}

VideoStream::VideoStream()
    : streamMediaType{AVMEDIA_TYPE_VIDEO},
    streamFormat{nullptr}, stream{nullptr}, decoder{nullptr},
    decoderContext{nullptr}, frame{nullptr}
{   
}

bool VideoStream::WaitForStream(DecoderData decoderSettings, uint16_t port)
{
    int ret = 0;

    auto url = std::string{streamURL} + ":" + std::to_string(port);

    // setting TCP input options
    AVDictionary* opts = 0;
    av_dict_set(&opts, "listen", "1", 0); // 1 means listen for any connection on the URL port, 0 means listen for connections to the URL
    av_dict_set(&opts, "probesize", "5000", 0);
    // av_dict_set(&opts, "buffer_size", "16384", 0);

    //open input file, and allocate format context
    auto entry = av_dict_get(opts, "timeout", nullptr, 0);
    if(entry != nullptr)
        std::cout << "Dict[" << entry->key << "]: " << entry->value << std::endl;
    av_dict_set(&opts, "framerate", "60", 0);
    
    std::cout << "listening for connection now...\n" << std::endl;
    ret = avformat_open_input(&streamFormat, url.c_str(), 0, &opts);
    
    if (ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);
        std::cout << "Error opening stream: " << errbuf << std::endl;
        return false;
    }

    std::cout << "connection found\n" << std::endl;

    // Retrieve stream information
    std::cout << "getting stream info...\n" << std::endl;
    ret = avformat_find_stream_info(streamFormat, NULL);
    if(ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);
        std::cout << "Error getting stream info: " << errbuf << std::endl;
        return false;
    }

    std::cout << "Video input opened with " << streamFormat->nb_streams << " stream...\n\n";

    ret = av_find_best_stream(streamFormat, streamMediaType, -1, -1, NULL, 0);
    if (ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);
        std::cout << "Error looking for '" << av_get_media_type_string(streamMediaType) << "' stream to open: " << errbuf << std::endl;
        return false;
    }

    streamIndex = ret;

    stream = streamFormat->streams[streamIndex];

    decoder = avcodec_find_decoder(stream->codecpar->codec_id);
    if(decoder == nullptr)
    {
        std::cout << "Error looking for '" << av_get_media_type_string(streamMediaType) << "' codec" << std::endl;
        return false;
    }

    decoderContext = avcodec_alloc_context3(decoder);
    if(decoderContext == nullptr)
    {
        std::cout << "Error allocating context for decoder." << std::endl;
        return false;
    }

    decoderContext->flags = decoderSettings.flags1;
    decoderContext->flags2 = decoderSettings.flags2;
    decoderContext->hwaccel_flags = decoderSettings.accelFlags;
    decoderContext->skip_loop_filter = decoderSettings.discardFilter;
    decoderContext->thread_type = decoderSettings.threadType;
    decoderContext->thread_count = decoderSettings.threadCount;

    PrintDecoderContextSettings(*decoderContext);

    std::cout << "Decoder settings after alteration\n";
    PrintDecoderContextSettings(*decoderContext);

    ret = avcodec_parameters_to_context(decoderContext, stream->codecpar);
    if(ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);
        std::cout << "Error copying codec info for '" << av_get_media_type_string(streamMediaType) << "' to decoder: " << errbuf << std::endl;
        return false;
    }

    ret = avcodec_open2(decoderContext, decoder, NULL);
    if(ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);
        std::cout << "Failed opening decoder for '" << av_get_media_type_string(streamMediaType) << "': " << errbuf << std::endl;
        return false;
    }
    
    ret = av_image_alloc(videoData, videoLinesizes, decoderContext->width, decoderContext->height, decoderContext->pix_fmt, 1);
    if(ret < 0)
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);
        std::cout << "Failed to allocate image data: " << errbuf << std::endl;
        return false;
    }

    std::cout << "Video input format: " << streamFormat->iformat->name << "\n\n";
    
    av_dict_free(&opts);

    return true;
}

bool VideoStream::Read(AVPacket& output)
{
    av_init_packet(&output);
    output.data = NULL;
    output.size = 0;

    return av_read_frame(streamFormat, &output) >= 0;
}

AVStream const * const VideoStream::StreamInfo()
{
    return stream;
}

void VideoStream::CloseStream()
{
    if(streamFormat != nullptr)
    {
        avformat_close_input(&streamFormat);
        streamFormat = nullptr;
    }
}

void VideoStream::Cleanup()
{
    if(streamFormat != nullptr)
    {
        avformat_close_input(&streamFormat);
        streamFormat = nullptr;
    }
    if(decoderContext != nullptr)
    {
        avcodec_close(decoderContext);
        avcodec_free_context(&decoderContext);
        decoderContext = nullptr;
    }
    av_freep(&videoData[0]);
    if(frame != nullptr)
    {
        av_frame_free(&frame);
        frame = nullptr;
    }
}