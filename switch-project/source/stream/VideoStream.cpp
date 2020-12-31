#include "VideoStream.h"

extern "C" 
{
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
}

//#include <switch.h>
#include <iostream>
#include "StreamDecoder.h"
#include <unordered_map>

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

std::string HWAccelFlagToString(uint32_t flags)
{
    std::string setFlags{};
    std::unordered_map<int, std::string> flagToString{
        {AV_HWACCEL_CODEC_CAP_EXPERIMENTAL,  "EXPERIMENTAL"},
        {AV_HWACCEL_FLAG_IGNORE_LEVEL,  "IGNORE HW SUPPORT LEVEL"},
        {AV_HWACCEL_FLAG_ALLOW_HIGH_DEPTH,  "OUTPUT YUV GREATER THAN 4:2:0"},
        {AV_HWACCEL_FLAG_ALLOW_PROFILE_MISMATCH,  "ATTEMPT HW ACCEL EVEN IF MISMATCHED"}
    };

    for(auto item : flagToString)
    {
        if(flags & item.first)
            setFlags += "    " + item.second + "\n";
    }

    if(setFlags.size() == 0)
        setFlags += "   NONE";

    return setFlags;
}

/*
    Flag propeties and examples
    https://ffmpeg.org/doxygen/4.0/group__lavc__core.html#ga1a6a486e686ab6c581ffffcb88cb31b3
*/
std::string DecoderFlag2ToString(uint32_t flags)
{
    std::string setFlags{};
    std::unordered_map<int, std::string> flagToString{
        {AV_CODEC_FLAG2_FAST,  "FAST - ENABLE SPEEDUPS"},
        {AV_CODEC_FLAG2_NO_OUTPUT,  "SKIP BITSTREAM ENCODE"},
        {AV_CODEC_FLAG2_LOCAL_HEADER,  "GLOBAL HEADERS IN KEYFRAME"},
        {AV_CODEC_FLAG2_DROP_FRAME_TIMECODE,  "DROP FRAME FORMAT - DEPRECATED"},
        {AV_CODEC_FLAG2_CHUNKS,  "TRUNCATED FRAMES"},
        {AV_CODEC_FLAG2_IGNORE_CROP,  "IGNORE CROP INFO"},
        {AV_CODEC_FLAG2_SHOW_ALL,  "SHOW ALL BEFORE 1ST KEYFRAME"},
        {AV_CODEC_FLAG2_EXPORT_MVS,  "EXPORT MOTION VECTOR SIDEDATA"},
        {AV_CODEC_FLAG2_SKIP_MANUAL,  "EXPORT SKIP INFO AS SIDE DATA"},
        {AV_CODEC_FLAG2_RO_FLUSH_NOOP,  "DONT RESET ASS ON FLUSH (SUBTITLES)"}
    };

    for(auto item : flagToString)
    {
        if(flags & item.first)
            setFlags += "    " + item.second + "\n";
    }

    if(setFlags.size() == 0)
        setFlags += "   NONE";

    return setFlags;
}

/*
    Flag propeties and examples
    https://ffmpeg.org/doxygen/4.0/group__lavc__core.html#ga1a6a486e686ab6c581ffffcb88cb31b3
*/
std::string DecoderFlag1ToString(uint32_t flags)
{
    std::string setFlags{};
    std::unordered_map<int, std::string> flagToString{
        {AV_CODEC_FLAG_UNALIGNED,  "UNALIGNED FRAMES"},
        {AV_CODEC_FLAG_QSCALE,  "FIXED QSCALE"},
        {AV_CODEC_FLAG_4MV,  "H263 PREDICTION"},
        {AV_CODEC_FLAG_OUTPUT_CORRUPT,  "OUTPUT CORRUPT FRAMES"},
        {AV_CODEC_FLAG_QPEL,  "QPEL MC"},
        {AV_CODEC_FLAG_DROPCHANGED,  "DROP DIFFERING FRAMES"},
        {AV_CODEC_FLAG_PASS1,  "2PASS in 1st PASS MODE"},
        {AV_CODEC_FLAG_PASS2,  "2PASS in 2nd PASS MODE"},
        {AV_CODEC_FLAG_LOOP_FILTER,  "LOOP FILTER"},
        {AV_CODEC_FLAG_GRAY,  "GRAYSCALE ONLY"},
        {AV_CODEC_FLAG_PSNR,  "ERRORS SET IN ENCODING"},
        {AV_CODEC_FLAG_TRUNCATED,  "TRUNCATED FRAMES"},
        {AV_CODEC_FLAG_INTERLACED_DCT,  "INTERLACTED DCT"},
        {AV_CODEC_FLAG_LOW_DELAY,  "FORCE LOW DELAY"},
        {AV_CODEC_FLAG_GLOBAL_HEADER,  "GLOBAL HEADER IN EXTRADATA"},
        {AV_CODEC_FLAG_BITEXACT,  "ONLY BITEXACT STUFF"},
        {AV_CODEC_FLAG_AC_PRED,  "H263 ADV INTRACODING / PREDICITION"},
        {AV_CODEC_FLAG_INTERLACED_ME,  "INTERLACED MOTION EST."},
        {AV_CODEC_FLAG_CLOSED_GOP,  "CLOSED GOP"}
    };

    for(auto item : flagToString)
    {
        if(flags & item.first)
            setFlags += "    " + item.second + "\n";
    }

    if(setFlags.size() == 0)
        setFlags += "   NONE";

    return setFlags;
}

void PrintDecoderContextSettings(AVCodecContext const & decoderContext)
{
    std::cout << "HW Accel Flags\n";
    {
        auto hwaccelFlags = HWAccelFlagToString(decoderContext.hwaccel_flags);
        std::cout << hwaccelFlags << "\n";
    }
    std::cout << "Flag 1\n";
    {
        auto flagsSet = DecoderFlag1ToString(decoderContext.flags);
        std::cout << flagsSet << "\n";
    }
    std::cout << "Flag 2\n";
    {
        auto flags2Set = DecoderFlag2ToString(decoderContext.flags2);
        std::cout << flags2Set << "\n";
    }
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

auto constexpr streamURL = "tcp://0.0.0.0";

VideoStream::VideoStream()
    : streamFormat{nullptr}, stream{nullptr}, decoder{nullptr},
    decoderContext{nullptr}, frame{nullptr}
{   
}

bool VideoStream::WaitForStream(uint16_t port)
{
    int ret = 0;

    auto url = std::string{streamURL} + ":" + std::to_string(port);

    // setting TCP input options
    AVDictionary* opts = 0;
    av_dict_set(&opts, "listen", "1", 0); // 1 means listen for any connection on the URL port, 0 means listen for connections to the URL
    av_dict_set(&opts, "probesize", "5000", 0);

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

    std::cout << "Found Decoder default settings\n";
    // I NEED TO MAKE THIS CONFIGURABLE FOR THE USER!! :D 
    PrintDecoderContextSettings(*decoderContext);

    if(decoderContext->flags & AV_CODEC_FLAG_LOW_DELAY)
        std::cout << "Decoder already flagged for low delay\n";
    else
	    decoderContext->flags |= AV_CODEC_FLAG_LOW_DELAY;

    if(decoderContext->flags2 & AV_CODEC_FLAG2_FAST)
        std::cout <<"Decoder already flagged for fast\n";
    else
	    decoderContext->flags2 |= AV_CODEC_FLAG2_FAST;
    
    /*
        https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html#a7651614f4309122981d70e06a4b42fcb
        Considering we never supply more than 1 frame at a time, we should set it to SLICE
        as FRAME incurs a 1 frame delay per thread
    */
    decoderContext->thread_type = FF_THREAD_SLICE;
    decoderContext->thread_count = 4;

	decoderContext->skip_loop_filter = AVDISCARD_ALL;

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
    if(videoData != nullptr)
        av_freep(&videoData[0]);
    if(frame != nullptr)
    {
        av_frame_free(&frame);
        frame = nullptr;
    }
}