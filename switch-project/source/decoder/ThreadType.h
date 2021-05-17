#ifndef __THREADTYPE_H__
#define __THREADTYPE_H__

#include <unordered_map>
#include <string>

class ThreadType
{
public:
    ThreadType();

    std::unordered_map<int32_t, std::string> const& Options() const;
    std::unordered_map<int32_t, std::string> const& Descriptions() const;

    std::string const ToString(int32_t type) const;
    int32_t Flag(std::string const type) const;
    std::string const ToDescription(int32_t type) const;

private:
    std::unordered_map<int32_t, std::string> const threadOptions;
    std::unordered_map<int32_t, std::string> const threadDesc;
};

#endif
