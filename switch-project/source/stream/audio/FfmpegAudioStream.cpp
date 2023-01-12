#include "FfmpegAudioStream.h"
#include <iostream>

namespace ffmpeg
{
    std::string const avErrorToString(int errCode)
    {
        char strbuffer[AV_ERROR_MAX_STRING_SIZE]{0};
        av_make_error_string(strbuffer, AV_ERROR_MAX_STRING_SIZE, errCode);
        return strbuffer;
    }

    AudioStream::AudioStream()
        : formatContext{nullptr}, streamCodec{nullptr}, codecContext{nullptr}, 
        dataPacket{nullptr}, dataFrame{nullptr}
    {
    }

    bool AudioStream::openStream(std::string const& uri)
    {
        formatContext = avformat_alloc_context();

        AVDictionary* options = nullptr;
        av_dict_set(&options, "listen", "1", 0);
        // av_dict_set(&options, "protocol_whitelist", "file,udp,rtp", 0);
        // av_dict_set(&options, "sdp_flags", "custom_io", 0);
        // auto const infmt = av_find_input_format("sdp");

        auto const openState = avformat_open_input(&formatContext, uri.c_str(), nullptr, &options);

        if(openState < 0)
        {
            std::cout << "Failed to open [" << uri << "] with error:\n";
            std::cout << "    " << avErrorToString(openState) << "\n";
            avformat_close_input(&formatContext);
            return false;
        }

        auto const streamInfoState = avformat_find_stream_info(formatContext, nullptr);
        if(streamInfoState < 0)
        {
            std::cout << "Failed to find stream info with error:\n";
            std::cout << "    " << avErrorToString(openState) << "\n";
            avformat_close_input(&formatContext);
            return false;
        }

        auto streamIdx = 0U;
        while(streamIdx < formatContext->nb_streams)
        {
            auto const currStream = formatContext->streams[streamIdx];
            if(currStream->codecpar->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO)
                break;
            ++streamIdx;
        }

        if(streamIdx >= formatContext->nb_streams)
        {
            std::cout << "Failed to find an audio stream. Closing stream...\n";
            avformat_close_input(&formatContext);
            return false;
        }

        auto const stream = formatContext->streams[streamIdx];
        auto const codecParams = stream->codecpar;
        streamCodec = avcodec_find_decoder(codecParams->codec_id);

        if(streamCodec == nullptr)
        {
            std::cout << "Failed to find decoder for codec ID " << codecParams->codec_id << "\n";
            avformat_close_input(&formatContext);
            return false;
        }

        codecContext = avcodec_alloc_context3(streamCodec);
        if(codecContext == nullptr)
        {
            std::cout << "Failed to allocate codec context\n";
            avformat_close_input(&formatContext);
            return false;
        }
        codecContext->request_sample_fmt = AVSampleFormat::AV_SAMPLE_FMT_S16;
        auto const parameterResult = avcodec_parameters_to_context(codecContext, codecParams);
        if(parameterResult < 0)
        {
            std::cout << "Failed to find stream info with error:\n";
            std::cout << "    " << avErrorToString(parameterResult) << "\n";
            avformat_close_input(&formatContext);
            return false;
        }
        auto const openCodecResult = avcodec_open2(codecContext, streamCodec, nullptr);
        if(openCodecResult < 0)
        {
            std::cout << "Failed to open stream codec context with error:\n";
            std::cout << "    " << avErrorToString(openCodecResult) << "\n";
            avformat_close_input(&formatContext);
            return false;
        }

        dataPacket = av_packet_alloc();
        if(dataPacket == nullptr)
        {
            std::cout << "Failed to allocate packet...\n";
            avformat_close_input(&formatContext);
            return false;
        }

        dataFrame = av_frame_alloc();
        if(dataFrame == nullptr)
        {
            std::cout << "Failed to allocate frame\n";
            avformat_close_input(&formatContext);
            return false;
        }

        return true;
    }

    bool AudioStream::isOpen() const 
    {
        return formatContext != nullptr && streamCodec != nullptr
            && codecContext != nullptr && dataPacket != nullptr
            && dataFrame != nullptr;
    }

    AVCodecContext const * const AudioStream::CodecContext() const
    {
        return codecContext;
    }

    void AudioStream::closeStream()
    {
        cleanup();
    }

    bool AudioStream::nextPacket()
    {
        if(dataPacket == nullptr)
            return false;

        if(dataPacket->buf != nullptr)
            av_packet_unref(dataPacket);
        
        auto const readResult = av_read_frame(formatContext, dataPacket);
        if(readResult < 0 && readResult != AVERROR_EOF)
        {
            std::cout << "Failed to read packet with error:\n";
            std::cout << "    " << avErrorToString(readResult) << "\n";
        }
        return readResult == 0;
    }

    AVFrame const * const AudioStream::nextFrame()
    {
        auto const decoderPacketState = avcodec_send_packet(codecContext, dataPacket);
        if(decoderPacketState < 0)
        {
            if(decoderPacketState != AVERROR(EAGAIN) && decoderPacketState != AVERROR_EOF)
            {
                std::cout << "Failed to send packet to decoder with error:\n";
                std::cout << "    " << avErrorToString(decoderPacketState) << "\n";
            }
            return nullptr;
        }

        auto const decoderFrameState = avcodec_receive_frame(codecContext, dataFrame);
        if(decoderFrameState < 0 && decoderFrameState != AVERROR(EAGAIN) && decoderFrameState != AVERROR_EOF)
        {
            std::cout << "Failed to receive frame from decoder with error:\n";
            std::cout << "    " << avErrorToString(decoderFrameState) << "\n";
            return nullptr;
        }

        return dataFrame;
    }

    void AudioStream::cleanup()
    {
        //flush packets and frames
        avcodec_send_packet(codecContext, nullptr);
        auto bufferState = 0;
        do
        {
            bufferState = avcodec_receive_frame(codecContext, dataFrame);
        } while(bufferState != AVERROR_EOF);

        avio_flush(formatContext->pb);
        avformat_flush(formatContext);

        avformat_close_input(&formatContext);

        if(dataFrame != nullptr)
            av_frame_free(&dataFrame);
        if(dataPacket != nullptr)
            av_packet_free(&dataPacket);
    }
}