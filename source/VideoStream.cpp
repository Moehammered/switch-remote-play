#include "VideoStream.h"

extern "C" 
{
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
}

//#include <switch.h>
#include <iostream>
#include "Decoder.h"

bool VideoStream::WaitForStream(std::string url)
{
    //default compiler generated constructor doesn't initialise these to null by default
    streamFormat = nullptr;
    decoder = nullptr;
    stream = nullptr;
    decoderContext = nullptr;
    frame = nullptr;

    int ret = 0;

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
void VideoStream::StreamVideoViaDecoder(ScreenRenderer& renderer, std::atomic_bool& streamOn)
{
    Decoder decoder;
    if(!decoder.Initialise(stream->codecpar, false))
    {
        std::cout << "Error occurred initialising the decoder." << std::endl;
        return;
    }
    
    AVPacket dataPacket;
    av_init_packet(&dataPacket);
    dataPacket.data = NULL;
    dataPacket.size = 0;

    // values to capture frame rate / time passage of renderer
    // int deltaFrameSample = 300;
    // const double NANO_TO_SECONDS = 1000000000.0;
    // uint64_t currTime, prevTime, deltaTime;
    // currTime = armTicksToNs(armGetSystemTick());
    // prevTime = currTime;
    // deltaTime = 0;

    std::cout << "starting stream read of packets...\n" << std::endl;
    //read frames from the stream we've connected to and setup
    while (av_read_frame(streamFormat, &dataPacket) >= 0)
    {
        if(decoder.DecodeFramePacket(dataPacket))
        {
            //something was decoded, render it
            auto screenTexture = renderer.GetScreenTexture();
            auto region = renderer.Region();
            // render frame data - expecting YUV420 format
            // (stride values represent space between horizontal lines across screen)
            auto decodedFrame = decoder.DecodedFrame();

            auto yPlane = decodedFrame.data[0];
            auto yPlaneStride = decodedFrame.width;
            auto uPlane = decodedFrame.data[1];
            auto uPlaneStride = decodedFrame.width/2;
            auto vPlane = decodedFrame.data[2];
            auto vPlaneStride = decodedFrame.width/2;

            // std::cout << "Updating SDL texture" << std::endl;
            SDL_UpdateYUVTexture(screenTexture, &region, 
                                yPlane, yPlaneStride,
                                uPlane, uPlaneStride, 
                                vPlane, vPlaneStride);

            renderer.RenderScreenTexture();
            
            dataPacket.data += decodedFrame.pkt_size; //think this is unnecessary
            dataPacket.size -= decodedFrame.pkt_size; //think this is unnecessary
        }
        // currTime = armTicksToNs(armGetSystemTick());
        // deltaTime += (currTime - prevTime);
        // prevTime = currTime;

        // if(--deltaFrameSample == -1)
        // {
        //     deltaFrameSample = 300;
        //     auto frameRate = deltaFrameSample / (deltaTime / NANO_TO_SECONDS);
        //     //std::cout << "FPS: " << frameRate << ", DeltaTimePassed: " << deltaTime/NANO_TO_SECONDS << std::endl;

        //     deltaTime = 0;
        // }
        // std::cout << "cleaning up used packet" << std::endl;
        av_packet_unref(&dataPacket);

        if(!streamOn.load(std::memory_order_acquire))
            break;
    }
    //try and close the stream
    avformat_close_input(&streamFormat);
    streamFormat = nullptr;
    decoder.Flush();
    decoder.Cleanup();
}

void VideoStream::Cleanup()
{
    if(streamFormat != nullptr)
        avformat_close_input(&streamFormat);
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