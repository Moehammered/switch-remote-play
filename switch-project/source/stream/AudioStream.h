#ifndef __AUDIOSTREAM_H__
#define __AUDIOSTREAM_H__

#include <stdint.h>
#include "srp/audio/AudioOptions.h"
#include "audio/FfmpegAudioStream.h"

class AudioStream
{
public:
    AudioStream();

    bool Running() const;

    bool Start(audio::AudioConfig const audioSettings, uint16_t const port);

    void Shutdown();

private:
    ffmpeg::AudioStream ffmpegStream;
    // int32_t audioSocket;
    // int32_t createUdpSocket(uint16_t const port);
};

#endif