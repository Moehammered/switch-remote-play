#ifndef __DISCARDTYPE_H__
#define __DISCARDTYPE_H__

#include <unordered_map>
#include <string>

extern "C"
{
    #include <libavcodec/avcodec.h>
}

class DiscardType
{
public:
    DiscardType();

    std::unordered_map<AVDiscard, std::string> const& Options() const;
    std::unordered_map<AVDiscard, std::string> const& Descriptions() const;

    std::string const ToString(AVDiscard type) const;
    AVDiscard Value(std::string const type) const;
    std::string const ToDescription(AVDiscard type) const;

private:
    std::unordered_map<AVDiscard, std::string> const discardOptions;
    std::unordered_map<AVDiscard, std::string> const discardDesc;
};

#endif
