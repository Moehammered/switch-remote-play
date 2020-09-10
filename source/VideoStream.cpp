#include "VideoStream.h"

extern "C" 
{
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
}

#include <switch.h>
#include <iostream>
#include <vector>
#include <deque>

void VideoStream::Initialise()
{
    streamFormat = nullptr;
    streamMediaType = AVMEDIA_TYPE_VIDEO;
    decoder = nullptr;
    stream = nullptr;
    decoderContext = nullptr;
    frame = nullptr;
}

bool VideoStream::WaitForStream(std::string url)
{
    int ret = 0;

    // setting TCP input options
    AVDictionary* opts = 0;
    av_dict_set(&opts, "listen", "1", 0); // 1 means listen for any connection on the URL port, 0 means listen for connections to the URL
    av_dict_set(&opts, "probesize", "50000", 0);
    //av_dict_set(&opts, "timeout", "100", 0);//in Ms??? (sets fine, but doesn't stop the socket from blocking. Think it has to do more with the encoder/server)
    //av_dict_set(&opts, "recv_buffer_size", "1048576", 0);       // set option for size of receive buffer

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

/**
 * Read https://ffmpeg.org/doxygen/3.3/group__lavc__encdec.html for info on the decoding / encoding process for packets
 */
void VideoStream::StreamVideoQueue(ScreenRenderer& renderer)
{
    //initialise a packet object to read data retrieved from the stream that's connected
    AVPacket dataPacket;
    av_init_packet(&dataPacket);
    dataPacket.data = NULL;
    dataPacket.size = 0;

    int deltaFrameSample = 300;
    int ret = 0;
    frame = av_frame_alloc(); //allocate a frame to use and store the data retrieved from the decoder
    std::cout << "starting stream read of packets...\n" << std::endl;
    //read frames from the stream we've connected to and setup
    const double NANO_TO_SECONDS = 1000000000.0;
    uint64_t currTime, prevTime;
    currTime = armTicksToNs(armGetSystemTick());
    prevTime = currTime;
    uint64_t deltaTime = 0;

    int packetState = 0;
    std::deque<AVPacket> queuedPacks;
    int decodeQueueLimit = 3;
    
    while(packetState >= 0) //gather some data packets to render in batch
    {
        // std::cout << "storing packets" << std::endl;
        while(queuedPacks.size() < decodeQueueLimit && packetState >= 0)
        {
            // std::cout << "reading packets" << std::endl;
            packetState = av_read_frame(streamFormat, &dataPacket);
            if(dataPacket.stream_index == streamIndex)
            {
                queuedPacks.push_back(dataPacket);
            }
        }
        int decoderState = 0;
        while(queuedPacks.size() > 0) //start decoding the packets
        {
            // if(queuedPacks.size() > 3)
            //     std::cout << "Queue Length: " << queuedPacks.size() << std::endl;
            // std::cout << "decoding queued packets" << std::endl;
            AVPacket packet = queuedPacks.front();
            decoderState = avcodec_send_packet(decoderContext, &packet);
            int frameState = 0;
            if (decoderState < 0 && decoderState == AVERROR_EOF)
                decoderState = 0; //ignore reaching end of file/stream error

            if(decoderState >= 0)
            {
                // decoded frame available - get frames until no more then dispose of packet
                do
                {
                    // std::cout << "reading decoded frame" << std::endl;
                    frameState = avcodec_receive_frame(decoderContext, frame);
                    if(frameState >= 0)
                    {
                        if(decoderContext->width == frame->width 
                        && decoderContext->height == frame->height 
                        && decoderContext->pix_fmt == frame->format)
                        {
                            //render it here
                            auto screenTexture = renderer.GetScreenTexture();
                            auto region = renderer.GetRegion();
                            //render frame data - expecting YUV420 format (stride values represent space between horizontal lines across screen)
                            auto yPlane = frame->data[0];
                            auto yPlaneStride = frame->width;
                            auto uPlane = frame->data[1];
                            auto uPlaneStride = frame->width/2;
                            auto vPlane = frame->data[2];
                            auto vPlaneStride = frame->width/2;
                            SDL_UpdateYUVTexture(screenTexture, &region, yPlane, yPlaneStride, uPlane, uPlaneStride, vPlane, vPlaneStride);

                            // std::cout << "rendering frame" << std::endl;
                            renderer.RenderScreenTexture();
                            currTime = armTicksToNs(armGetSystemTick());
                            deltaTime = deltaTime + (currTime - prevTime);
                            prevTime = currTime;

                            if(--deltaFrameSample == 0)
                            {
                                deltaFrameSample = 300;
                                auto frameRate = deltaFrameSample / (deltaTime / NANO_TO_SECONDS);
                                std::cout << "FPS: " << frameRate << ", DeltaTimePassed: " << deltaTime/NANO_TO_SECONDS << std::endl;

                                deltaTime = 0;
                            }
                        }
                        else
                        {
                            //error format has changed from the stream we configured and connected to
                            std::cout << "Stream format has changed compared to what it was when setup initially." << std::endl;
                            break;
                        }
                        
                        packet.data += frame->pkt_size;
                        packet.size -= frame->pkt_size;
                    }
                } while(packet.size > 0);
            }
            //finished renderering it, dispose of the used packet
            av_packet_unref(&packet);
            queuedPacks.pop_front();
        }
    }

    //let the decoder flush any remaining packets
    //after instructing it to flush, the receive frame call will return EOF once buffers are cleared and no more frames remain
    avcodec_send_packet(decoderContext, nullptr);
    do
    {
        ret = avcodec_receive_frame(decoderContext, frame);
    } while (ret != AVERROR_EOF);

    std::cout << "Stream finished" << std::endl;
}

/**
 * Read https://ffmpeg.org/doxygen/3.3/group__lavc__encdec.html for info on the decoding / encoding process for packets
 */
void VideoStream::StreamVideo(ScreenRenderer& renderer)
{
    //initialise a packet object to read data retrieved from the stream that's connected
    AVPacket dataPacket;
    av_init_packet(&dataPacket);
    dataPacket.data = NULL;
    dataPacket.size = 0;

    int counter = 100;
    int deltaFrameSample = 300;
    int ret = 0;
    frame = av_frame_alloc(); //allocate a frame to use and store the data retrieved from the decoder
    std::cout << "starting stream read of packets...\n" << std::endl;
    //read frames from the stream we've connected to and setup
    const double NANO_TO_SECONDS = 1000000000.0;
    uint64_t currTime, prevTime;
    currTime = armTicksToNs(armGetSystemTick());
    prevTime = currTime;
    uint64_t deltaTime = 0;

    uint64_t streamTime, prevStreamTime, deltaStreamTime;
    streamTime = armTicksToNs(armGetSystemTick());
    prevStreamTime = streamTime;
    deltaStreamTime = 0;

    while (av_read_frame(streamFormat, &dataPacket) >= 0)
    {
        streamTime = armTicksToNs(armGetSystemTick());
        if(--counter <= 0)
        {
            std::cout << "reading frames from stream...\n" << std::endl;
            counter = 100;
        }
        do
        {
            if (dataPacket.stream_index == streamIndex)
            {
                //send incoming frame packet data to the decoder
                ret = avcodec_send_packet(decoderContext, &dataPacket);
                if (ret < 0 && ret == AVERROR_EOF)
                    ret = 0; //ignore reaching end of file/stream error
                //retrieve frames processed by the decoder (ret == amount of frames processed?)
                ret = avcodec_receive_frame(decoderContext, frame);
                if (ret < 0)
                    break;

                //check to see if the format matches our context's format
                if(decoderContext->width == frame->width 
                && decoderContext->height == frame->height 
                && decoderContext->pix_fmt == frame->format)
                {
                    //render it here
                    auto screenTexture = renderer.GetScreenTexture();
                    auto region = renderer.GetRegion();
                    //render frame data - expecting YUV420 format (stride values represent space between horizontal lines across screen)
                    auto yPlane = frame->data[0];
                    auto yPlaneStride = frame->width;
                    auto uPlane = frame->data[1];
                    auto uPlaneStride = frame->width/2;
                    auto vPlane = frame->data[2];
                    auto vPlaneStride = frame->width/2;
                    SDL_UpdateYUVTexture(screenTexture, &region, yPlane, yPlaneStride, uPlane, uPlaneStride, vPlane, vPlaneStride);

                    renderer.RenderScreenTexture();
                    currTime = armTicksToNs(armGetSystemTick());
                    deltaTime = deltaTime + (currTime - prevTime);
                    prevTime = currTime;

                    deltaStreamTime = deltaStreamTime + (streamTime - prevStreamTime);
                    if(--deltaFrameSample == 0)
                    {
                        deltaFrameSample = 300;
                        auto frameRate = deltaFrameSample / (deltaTime / NANO_TO_SECONDS);
                        std::cout << "FPS: " << frameRate << ", DeltaTimePassed: " << deltaTime/NANO_TO_SECONDS << std::endl;

                        deltaTime = 0;

                        auto streamRate = deltaFrameSample / (deltaStreamTime / NANO_TO_SECONDS);
                        std::cout << "PPS:" << streamRate << ", Est. Latency?: " << streamRate - frameRate << ", StreamTimePassed: " << deltaStreamTime/NANO_TO_SECONDS << std::endl;
                        deltaStreamTime = 0;
                    }
                }
                else
                {
                    //error format has changed from the stream we configured and connected to
                    std::cout << "Stream format has changed compared to what it was when setup initially." << std::endl;
                    break;
                }
                
                dataPacket.data += frame->pkt_size;
                dataPacket.size -= frame->pkt_size;
                
                if(dataPacket.size > 0)
                    std::cout << "remaining packet data size" << dataPacket.size << std::endl;
            }
            else
            {
                deltaStreamTime = deltaStreamTime + (streamTime - prevStreamTime);
            }
            
            prevStreamTime = streamTime;
        } while (dataPacket.size > 0);
        av_packet_unref(&dataPacket);
    }

    //let the decoder flush any remaining packets
    //after instructing it to flush, the receive frame call will return EOF once buffers are cleared and no more frames remain
    avcodec_send_packet(decoderContext, nullptr);
    do
    {
        ret = avcodec_receive_frame(decoderContext, frame);
    } while (ret != AVERROR_EOF);

    std::cout << "Stream finished" << std::endl;
}

void VideoStream::Cleanup()
{
    if(streamFormat != nullptr)
    {
        avformat_close_input(&streamFormat);
    }
    if(decoderContext != nullptr)
    {
        avcodec_close(decoderContext);
        avcodec_free_context(&decoderContext);
        decoderContext = nullptr;
    }
    if(videoData != nullptr)
    {
        av_freep(&videoData[0]);
    }
    if(frame != nullptr)
    {
        av_frame_free(&frame);
        frame = nullptr;
    }
}

// VideoStream::~VideoStream()
// {
    
// }