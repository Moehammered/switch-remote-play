#ifndef __NETWORKDATA_H__
#define __NETWORKDATA_H__

#include <stdint.h>

enum STREAM_MODE : int32_t
{
    LOW_LATENCY_30_FPS = 0,
    OK_LATENCY_60_FPS,
    LOW_LATENCY_V_FPS,
    STREAM_MODE_COUNT
};

struct alignas(16) FFMPEG_Config 
{
    int16_t     desiredFrameRate;
    int16_t     videoX;
    int16_t     videoY;
    int16_t     scaleX;
    int16_t     scaleY;
    uint16_t    bitrateKB;
    int16_t     vsyncMode;
    int8_t      padding[2];
};

constexpr int FFMPEG_CONFIG_SIZE = sizeof(FFMPEG_Config);

enum Command : int16_t
{
    SHUTDOWN = 0xEEE,
    UPDATE_FFMPEG_CONFIG = 0xADD,
    START_STREAM = 0xCAD,
    START_STREAM_LOW_LATENCY_30FPS = 0x030,
    START_STREAM_LOW_LATENCY_VFPS = 0xA60,
    START_STREAM_OK_LATENCY_60FPS = 0x060,
    STOP_STREAM = 0xBAB,
    CLOSE_SERVER = 0xAAA,
    SHUTDOWN_PC = 0x0FF,
    IGNORE_COMMAND = 0xFFF
};

constexpr int COMMAND_CODE_SIZE = sizeof(Command);

struct alignas(32) CommandPayload
{
    //for now only add ffmpeg-config as an extra data member
    FFMPEG_Config   configData;
    Command         commandCode;
    //fill the struct to pad it out to 32 bytes
    int8_t          padding[32 - FFMPEG_CONFIG_SIZE - COMMAND_CODE_SIZE];

    // int16_t dataBufferSize;
    // char dataBuffer[255];
};

constexpr int COMMAND_PAYLOAD_SIZE = sizeof(CommandPayload);

#endif
