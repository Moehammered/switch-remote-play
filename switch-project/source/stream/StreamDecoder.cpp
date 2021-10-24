#include "StreamDecoder.h"
#include <switch.h>
#include <iostream>

namespace
{
    void printAVError(int errorCode)
    {
        char errstr[100];
        av_strerror(errorCode, errstr, 100);
        std::cout << "AV Lib Error (" << errstr << ")" << std::endl;
    }

    void printAVDiscardMode(AVDiscard mode)
    {
        switch(mode)
        {
            case AVDISCARD_NONE:
                std::cout << "Discard nothing" << std::endl;
            break;

            case AVDISCARD_DEFAULT:
                std::cout << "Discard useless packets(empty packets for example)" << std::endl;
            break;

            case AVDISCARD_NONREF: //dunno what it means by non-reference
                std::cout << "Discard all non reference" << std::endl;
            break;

            // see https://en.wikipedia.org/wiki/Video_compression_picture_types
            case AVDISCARD_BIDIR: //discard frames that rely on previous and next frame to render properly
                std::cout << "Discard all bidirectional frames (b-frames)" << std::endl;
            break;

            // see https://en.wikipedia.org/wiki/Video_compression_picture_types
            case AVDISCARD_NONINTRA: //discard complete frames
                std::cout << "Discard all intra-coded frames (i-frames)" << std::endl;
            break;

            case AVDISCARD_NONKEY: //discard everything except keyframes
                std::cout << "Discard all frames except keyframes" << std::endl;
            break;

            case AVDISCARD_ALL: //discard everything
                std::cout << "Discard all frames" << std::endl;
            break;

            default:
                std::cout << "Unknown value (" << mode << ") not supported by ffmpeg" << std::endl;
            break;
        }
    }
}

StreamDecoder::StreamDecoder(AVCodecParameters const * const codecConfig)
    : codec{nullptr}, context{nullptr}, currentFrame{nullptr}
{
    if(codecConfig == nullptr)
        std::cout << "Codec parameters are not initialised" << std::endl;
    else
    {
        codec = avcodec_find_decoder(codecConfig->codec_id);
            
        if(codec == nullptr)
            std::cout << "Error looking for decoder codec with ID " << codecConfig->codec_id << std::endl;
        else
        {
            if(context != nullptr)
                avcodec_free_context(&context);

            context = avcodec_alloc_context3(codec);
            
            if(context == nullptr)
                std::cout << "Error allocating context for decoder" << std::endl;
            else
            {
                auto result = avcodec_parameters_to_context(context, codecConfig);
                if(result < 0)
                {
                    std::cout << "Error occurred copying codec info to decoder context" << std::endl;
                    printAVError(result);
                }
                else
                {
                    result = avcodec_open2(context, codec, nullptr);
                    if(result < 0)
                    {
                        std::cout << "Error occurred initialising decoder context to use the current codec (ID: " << codecConfig->codec_id << ")" << std::endl;
                        printAVError(result);
                    }
                    else
                        currentFrame = av_frame_alloc();
                }
            }
        }
    }
}

bool StreamDecoder::Initialised()
{
    return context != nullptr;
}

bool StreamDecoder::DecodeFramePacket(const AVPacket& packet)
{
    auto result = avcodec_send_packet(context, &packet);
    if(result < 0)
        return false;

    result = avcodec_receive_frame(context, currentFrame);
    if(result < 0)
        return false;

    return true;
}

const AVFrame& StreamDecoder::DecodedFrame()
{
    return *currentFrame;
}

/**
 * let the decoder flush any remaining packets. Will block until all pending frames are flushed out.
 */
void StreamDecoder::Flush()
{
    //send a null packet to instruct the decoder context to start flushing
    avcodec_send_packet(context, nullptr);
    auto result = 0;
    do
    {
        //flush out a frame (receive calls free on currentFrame)
        result = avcodec_receive_frame(context, currentFrame);
    } while (result != AVERROR_EOF);
}

void StreamDecoder::Cleanup()
{
    if(context != nullptr)
    {
        avcodec_close(context);
        avcodec_free_context(&context);
        context = nullptr;
    }
    if(currentFrame != nullptr)
    {
        av_frame_free(&currentFrame);
        currentFrame = nullptr;
    }
}