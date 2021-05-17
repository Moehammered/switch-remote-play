#include "ThreadType.h"
#include "../utils/EnumMapper.h"

extern "C"
{
    #include <libavcodec/avcodec.h>
}

ThreadType::ThreadType()
    : threadOptions{
        { FF_THREAD_FRAME, "thread frame" },
        { FF_THREAD_SLICE, "thread slice" }
    }, 
    threadDesc{
        { FF_THREAD_FRAME, "Thread Frame (1 thread/frame - incurs a delay per thread)" },
        { FF_THREAD_SLICE, "Thread Slice (many threads per frame)" }
    }
{
}

std::unordered_map<int32_t, std::string> const& ThreadType::Options() const
{
    return threadOptions;
}

std::unordered_map<int32_t, std::string> const& ThreadType::Descriptions() const
{
    return threadDesc;
}

std::string const ThreadType::ToString(int32_t type) const
{
    return enumToStr(threadOptions, type);
}

int32_t ThreadType::Flag(std::string const type) const
{
    return strToEnum(threadOptions, type);
}

std::string const ThreadType::ToDescription(int32_t type) const
{
    return enumToStr(threadDesc, type);
}
