#include "VideoStream.h"
extern "C" 
{
    #include <libavutil/imgutils.h>
    #include <libavutil/avutil.h>
}
#include <iostream>

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
    av_dict_set(&opts, "listen", "1", 0); // set option for listening
    av_dict_set(&opts, "probesize", "50000", 0);
    av_dict_set(&opts, "timeout", "10000", 0);//in Ms
//    av_dict_set(&opts, "recv_buffer_size", "5000", 0);       // set option for size of receive buffer

    //open input file, and allocate format context
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
    
    return true;
}

void VideoStream::StreamVideo(ScreenRenderer& renderer)
{
    //initialise a packet object to read data retrieved from the stream that's connected
    AVPacket pkt;
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    int counter = 100;
    int ret = 0;
    frame = av_frame_alloc(); //allocate a frame to use and store the data retrieved from the decoder
    std::cout << "starting stream read of packets...\n" << std::endl;
    //read frames from the stream we've connected to and setup
    while (av_read_frame(streamFormat, &pkt) >= 0)
    {
        if(--counter <= 0)
        {
            std::cout << "reading frames from stream...\n" << std::endl;
            counter = 100;
        }
        do
        {
            if (pkt.stream_index == streamIndex)
            {
                //send incoming frame packet data to the decoder
                ret = avcodec_send_packet(decoderContext, &pkt);
                if (ret < 0 && ret == AVERROR_EOF)
                    ret = 0; //ignore reaching end of file/stream error
                //retrieve frames processed by the decoder (ret == amount of frames processed?)
                ret = avcodec_receive_frame(decoderContext, frame);
                if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
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
                }
                else
                {
                    //error format has changed from the stream we configured and connected to
                    std::cout << "Stream format has changed compared to what it was when setup initially." << std::endl;
                    break;
                }
                
                pkt.data += frame->pkt_size;
                pkt.size -= frame->pkt_size;
                
                if(pkt.size > 0)
                    std::cout << "remaining Pkt data size" << pkt.size << std::endl;
            }
        } while (pkt.size > 0);
        av_packet_unref(&pkt);
    }

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