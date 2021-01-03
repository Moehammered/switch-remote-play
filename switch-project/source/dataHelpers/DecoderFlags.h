#ifndef __DECODERFLAGS_H__
#define __DECODERFLAGS_H__

extern "C"
{
    #include <libavcodec/avcodec.h>
}

#include <string>
#include <unordered_map>
#include <vector>

struct DecoderConfiguration
{
    int32_t flag1;
    int32_t flag2;
    int32_t hwAccelFlags;
    AVDiscard skipLoopFilter;
    int32_t threadType;
    int32_t threadCount;
};

int32_t constexpr DefaultHWAccelFlags = AV_HWACCEL_FLAG_IGNORE_LEVEL;
int32_t constexpr DefaultDecoderFlags1 = AV_CODEC_FLAG_LOW_DELAY;
int32_t constexpr DefaultDecoderFlags2 = AV_CODEC_FLAG2_FAST;
AVDiscard constexpr DefaultSkipLoopFilter = AVDiscard::AVDISCARD_DEFAULT;
int32_t constexpr DefaultThreadType = FF_THREAD_SLICE;

/*
    AVCodecContext - https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html
    Flag1 propeties and examples
    https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html#abb01e291550fa3fb96188af4d494587e
*/
std::unordered_map<int32_t, std::string> AllDecoderFlag1Descriptions();
std::unordered_map<int32_t, std::string> DecoderFlags1Description(int32_t flags);
std::unordered_map<int32_t, std::string> DecoderFlags1ToString(int32_t flags);
int32_t ParseDecoderFlags1Strings(std::vector<std::string> const & flagStrings);

/*
    AVCodecContext - https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html
    Flag2 propeties and examples
    https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html#a1944f9a4f8f2e123c087e1fe7613d571
*/
std::unordered_map<int32_t, std::string> AllDecoderFlag2Descriptions();
std::unordered_map<int32_t, std::string> DecoderFlags2Description(int32_t flags);
std::unordered_map<int32_t, std::string> DecoderFlags2ToStrings(int32_t flags);
int32_t ParseDecoderFlags2Strings(std::vector<std::string> const & flagStrings);

/*
    AVCodecContext - https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html
    HWAccelFlag properties and examples
    https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html#a0a90b6a8e324a220e2be52d6e33038b8
*/
std::unordered_map<int32_t, std::string> AllHWAccelFlagsDescriptions();
std::unordered_map<int32_t, std::string> HWAccelFlagsDescription(int32_t flags);
std::unordered_map<int32_t, std::string> HWAccelFlagsToStrings(int32_t flags);
int32_t ParseHWAccelFlagStrings(std::vector<std::string> const & flagStrings);

/*
    AVCodecContext - https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html
    Skip Loop Filter property and examples
    https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html#a6be328131743a97103b89e028e62e771
*/
std::unordered_map<AVDiscard, std::string> AllAVDiscardDescriptions();
std::string AVDiscardDescription(AVDiscard discard);
std::string AVDiscardToString(AVDiscard discard);
AVDiscard ParseAVDiscardString(std::string s);

/*
    AVCodecContext - https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html
    Thread Type properties and examples
    https://ffmpeg.org/doxygen/4.0/structAVCodecContext.html#a7651614f4309122981d70e06a4b42fcb
*/
std::string ThreadTypeDescription(int32_t type);
std::string ThreadTypeToString(int32_t type);
int32_t ParseThreadTypeString(std::string s);

#endif
