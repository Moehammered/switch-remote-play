#include "StreamDecoder.h"
#include <switch.h>
#include <iostream>

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

StreamDecoder::StreamDecoder(AVCodecParameters const * const codecConfig, bool skipFrames)
    : codec{nullptr}, context{nullptr}, currentFrame{nullptr}, 
    frameSkipCount{0}, skipFrames{skipFrames}
{
    std::cout << "Using frame skip? " << (skipFrames ? "yes" : "no") << std::endl;

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
    
    lastPacketTime = armTicksToNs(armGetSystemTick());
    // std::cout << "default decoder context skip frame mode: ";
    // printAVDiscardMode(context->skip_frame);
}

bool StreamDecoder::Initialised()
{
    return context != nullptr;
}

/**
 * note: for frame skipping having a packet with skip_frame set will instruct the decoder
 * to skip decoding the frame supplied by the packet. (ffmpeg docs - avcodec_send_packet)
 * https://www.ffmpeg.org/doxygen/3.4/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
 * 
 * Current implementation of frame skip just simply ignores packets to try and catch up. 
 * However the encoder (ffmpeg) will save data by only encoding and sending the changes in frames (not the entire screen).
 * This manifests itself when a transition in colour results in a larger packet size or when the 
 * buffer size and stream quality becomes too large to store in one packet. What results is a 
 * packet containing partial frames is discarded to catch up, and then the next frame retrieved is a partial frame. When rendered will show weird colours.
 * This will correct itself once the decoder has caught up with the encoder or when a full frame is retrieved from a packet.
 * 
 * A better implementation of frame skip would either use the built in skip, or use the 
 * time stamps of the packets and keep track of the time and see if the decoder falls behind realtime by a certain margin.
 */ 
bool StreamDecoder::DecodeFramePacket(const AVPacket& packet)
{
    /** frame skipping implementation 1: dump/ignore packets
     * Pros:
     *  + skipping barely noticeable 
     *  + matches acceptable latency almost exactly and maintains pace well
     *  + maintains same picture quality and latency from 2mb ~ 10mb stream bitrates
     * Cons:
     *  - artifacting from b-frames can occur
     *  - can cause a rythme of the decoder being '1 off' and receiving b-frames only until another skip occurs or until an i-frame is received
     *  - image quality seems to be less than method 2 (but I believe this has to do with the b-frames more than the skipping itself)
     */ 
    // if(skipFrames) //if frame skip is enabled
    // {
    //     //check if we should skip frames by checking the time since the last packet was received and
    //     //rendered and the packet that is received now.

    //     const double acceptableLatency = 0.030; //30 ms (1 second = 1000 milliseconds)
    //     auto now = armTicksToNs(armGetSystemTick());
    //     auto delta = (now - lastPacketTime)/NANO_TO_SECONDS;
    //     lastPacketTime = now;
    //     if(delta > (targetFrameTime+acceptableLatency))
    //     {
    //         std::cout << "skipping packet. Frame time exceeds acceptable latency (" << acceptableLatency*1000 << "ms)" << std::endl;
    //         return false; // throw the packet away
    //     }
    // }
    
    /** frame skipping implementation 2: instruct decoder to discard frames (need to experiment with built in skip frame method)
     * Pros:
     *  + leads to almost perfect picture quality
     *  + matches acceptable latency well and keeps up with real-time
     *  + automatically re-adjusts when stream starts to drift / get out of sync
     *  + great for high buffer/high bitrate streams
     *  + no artifacting
     * Cons:
     *  - skipping is very very noticeable in high speed action (Nier Automata for example)
     *  - skipping can be made consistent at the cost of sacrficing quality
     */
    if(skipFrames)
    {
        // skipping is common when trying to stay within 30 milliseconds. 50 milliseconds seems to be reasonable but lag can be noticeable when comparing next to source.
        const double acceptableLatency = 0.030; //30 ms (1 second = 1000 milliseconds)
        auto now = armTicksToNs(armGetSystemTick());
        auto delta = (now - lastPacketTime)/NANO_TO_SECONDS;
        lastPacketTime = now;
        if(delta > targetFrameTime+acceptableLatency)
        {
            context->skip_frame = AVDISCARD_BIDIR;
            ++frameSkipCount;
            // std::cout << "Skipping frame. Current frame skip count " << frameSkipCount << std::endl;
            // std::cout << "Approx delay from 60fps target: " << delta - targetFrameTime << " seconds" << std::endl;
            std::cout << "skipping packet. Frame time exceeds acceptable latency (" << acceptableLatency*1000 << "ms)" << std::endl;
        }
        else
        {
            context->skip_frame = AVDISCARD_DEFAULT;
            frameSkipCount = 0;
        }
    }

    // std::cout << "sending packet to decoder" << std::endl;
    auto result = avcodec_send_packet(context, &packet);
    if(result < 0)
        return false;

    // std::cout << "getting decoded frame from decoder" << std::endl;
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