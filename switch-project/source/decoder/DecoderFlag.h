#ifndef __DECODERFLAG_H__
#define __DECODERFLAG_H__

#include <unordered_map>
#include <string>
#include <vector>

class DecoderFlag
{
public:
    DecoderFlag();

    std::unordered_map<int32_t, std::string> const& Flag1Options() const;
    std::unordered_map<int32_t, std::string> const& Flag1Descriptions() const;
    std::unordered_map<int32_t, std::string> const& Flag2Options() const;
    std::unordered_map<int32_t, std::string> const& Flag2Descriptions() const;

    std::unordered_map<int32_t, std::string> const Flag1Strings(int32_t flags) const;
    std::unordered_map<int32_t, std::string> const Flag2Strings(int32_t flags) const;

    std::unordered_map<int32_t, std::string> const Flag1Descriptions(int32_t flags) const;
    std::unordered_map<int32_t, std::string> const Flag2Descriptions(int32_t flags) const;

    int32_t Flags1(std::vector<std::string> const& flagStrings) const;
    int32_t Flags2(std::vector<std::string> const& flagStrings) const;

private:
    std::unordered_map<int32_t, std::string> const flag1Options;
    std::unordered_map<int32_t, std::string> const flag1Desc;
    std::unordered_map<int32_t, std::string> const flag2Options;
    std::unordered_map<int32_t, std::string> const flag2Desc;

    std::unordered_map<int32_t, std::string> const ToStrings(int32_t flags, std::unordered_map<int32_t, std::string> const& map) const;
    int32_t ToFlags(std::vector<std::string> const& flags, std::unordered_map<int32_t, std::string> const& map) const;
};

#endif
