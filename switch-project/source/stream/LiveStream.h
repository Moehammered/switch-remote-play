#ifndef __LIVESTREAM_H__
#define __LIVESTREAM_H__

#include "StreamState.h"
#include "VideoStream.h"
#include "PcmStream.h"
#include "StreamDecoder.h"
#include "srp/decoder/DecoderOptions.h"
#include <functional>

struct YUVFrame
{
    uint8_t*    yPlane;
    int32_t     yPlaneStride;

    uint8_t*    uPlane;
    int32_t     uPlaneStride;

    uint8_t*    vPlane;
    int32_t     vPlaneStride;
};

typedef std::function<void(YUVFrame)> frameRenderDelegate;

class LiveStream
{
public:
    LiveStream(uint16_t const audioPort);

    bool Startup(DecoderData const decoderConfig, uint16_t const videoPort);
    bool Run(frameRenderDelegate const frameDataCallback);
    void Cleanup();

private:
    VideoStream videoStream;
    PcmStream audioStream;
    StreamDecoder* streamDecoder;
    AVPacket streamPacket;
};

#endif
