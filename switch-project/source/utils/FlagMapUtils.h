#ifndef __FLAGMAPUTILS_H__
#define __FLAGMAPUTILS_H__

#include <unordered_map>
#include <string>
#include <vector>

std::unordered_map<int32_t, std::string> const ToStrings(int32_t flags, std::unordered_map<int32_t, std::string> const & map);
int32_t const ParseFlagStrings(std::vector<std::string> const & values, std::unordered_map<int32_t, std::string> const & map);

#endif