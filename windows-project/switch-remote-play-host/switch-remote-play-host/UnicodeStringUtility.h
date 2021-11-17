#ifndef __UNICODESTRINGUTILITY_H__
#define __UNICODESTRINGUTILITY_H__

#include <string>
#include <ostream>

std::string const utf16ToUtf8(std::wstring const wstr);
std::wstring const utf8ToUtf16(std::string const str);

#ifdef UNICODE
typedef std::wostream encodedOutputStream;
typedef std::wstring tstring;
#else
typedef std::ostream encodedOutputStream;
typedef std::string tstring;
#endif

template<typename sourceType>
tstring transformString(sourceType const str)
{
    static_assert(false, "Invalid creation of transform function detected. Use only wstring or string.");
    return tstring{};
}

#ifdef UNICODE
template<>
tstring transformString(std::string const str);

template<>
tstring transformString(std::wstring const wstr);

template<>
tstring transformString(const char * cstr);

template<>
tstring transformString(const wchar_t* wcstr);

inline
tstring operator+(const char* cstr, std::wstring const wstr)
{
    return transformString(cstr) + wstr;
}

inline
tstring operator+(std::wstring const wstr, const char* cstr)
{
    return wstr + transformString(cstr);
}

#else
template<>
tstring transformString(std::string const str);

template<>
tstring transformString(std::wstring const wstr);

template<>
tstring transformString(const char* cstr);

template<>
tstring transformString(const wchar_t* wcstr);

inline
tstring operator+(const wchar_t* wcstr, std::string const str)
{
    return transformString(wcstr) + str;
}

inline
tstring operator+(std::string const str, const wchar_t* wcstr)
{
    return str + transformString(wcstr);
}
#endif

#endif