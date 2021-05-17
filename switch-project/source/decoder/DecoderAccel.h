#ifndef __DECODERACCEL_H__
#define __DECODERACCEL_H__

#include <unordered_map>
#include <string>
#include <vector>

class DecoderAccel
{
public:
    DecoderAccel();

    std::unordered_map<int32_t, std::string> const& Options() const;
    std::unordered_map<int32_t, std::string> const& Descriptions() const;

    std::unordered_map<int32_t, std::string> const ToStrings(int32_t flags) const;
    int32_t Flags(std::vector<std::string> const & flagStrings) const;
    std::unordered_map<int32_t, std::string> const ToDescriptions(int32_t flags) const;

private:
    std::unordered_map<int32_t, std::string> const accelOptions;
    std::unordered_map<int32_t, std::string> const accelDesc;
};

#endif
