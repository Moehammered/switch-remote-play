#ifndef __THREADTYPE_H__
#define __THREADTYPE_H__

#include <unordered_map>
#include <string>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

namespace decoder
{
    std::string const ThreadTypeToString(int32_t flag);
    std::string const ThreadTypeToDescription(int32_t flag);
    int32_t ParseThreadType(std::string const type);

    static std::unordered_map<int32_t, std::string> threadOptions
    {
        { FF_THREAD_FRAME, "thread frame" },
        { FF_THREAD_SLICE, "thread slice" }
    };

    static std::unordered_map<int32_t, std::string> threadDesc
    {
        { FF_THREAD_FRAME, "Thread Frame (1 thread/frame - incurs a delay per thread)" },
        { FF_THREAD_SLICE, "Thread Slice (many threads per frame)" }
    };

    int32_t constexpr MinThreadCount = 1;
    int32_t constexpr DefaultThreadCount = 4;
    int32_t constexpr MaxThreadCount = 24;

    std::string const ThreadCountToString(int32_t count);
    int32_t ParseThreadCount(std::string const count);
}

#endif
