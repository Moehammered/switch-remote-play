#include "LiveStream.h"

LiveStream::LiveStream(uint16_t const audioPort)
    : videoStream{}, audioStream{audioPort}, streamDecoder{nullptr}, streamPacket{0}
{
}

bool LiveStream::Startup(DecoderData const decoderConfig, uint16_t const videoPort)
{
    if(videoStream.WaitForStream(decoderConfig, videoPort))
    {
        auto streamInfo = videoStream.StreamInfo();
        if(streamDecoder != nullptr)
            delete streamDecoder;

        streamDecoder = new StreamDecoder(streamInfo->codecpar);
        if(audioStream.Ready() && !audioStream.Running())
            audioStream.Start();

        streamPacket = AVPacket{0};
        return true;
    }
    else
        return false;
}

bool LiveStream::Run(frameRenderDelegate const frameDataCallback)
{
    if(videoStream.Read(streamPacket))
    {
        if(streamDecoder->DecodeFramePacket(streamPacket))
        {
            auto decodedFrame = streamDecoder->DecodedFrame();
            auto frameRenderData = YUVFrame
            {
                .yPlane = decodedFrame.data[0],
                .yPlaneStride = decodedFrame.width,
                .uPlane = decodedFrame.data[1],
                .uPlaneStride = decodedFrame.width/2,
                .vPlane = decodedFrame.data[2],
                .vPlaneStride = decodedFrame.width/2
            };

            frameDataCallback(frameRenderData);
        }

        av_packet_unref(&streamPacket);

        return true;
    }
    else
    {
        videoStream.CloseStream();
        streamDecoder->Flush();
        streamDecoder->Cleanup();
        videoStream.Cleanup();
        delete streamDecoder;
        streamDecoder = nullptr;

        return false;
    }
}

void LiveStream::Cleanup()
{
    videoStream.Cleanup();

    if(streamDecoder != nullptr)
    {
        streamDecoder->Cleanup();
        delete streamDecoder;
        streamDecoder = nullptr;
    }

    if(audioStream.Running())
        audioStream.Shutdown();
}