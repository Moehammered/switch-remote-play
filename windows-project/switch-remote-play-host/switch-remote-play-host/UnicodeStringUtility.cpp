#include "UnicodeStringUtility.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::string const utf16ToUtf8(std::wstring const wstr)
{
    auto sizeNeeded = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.c_str(), wstr.size(), 0, 0, NULL, NULL);
    if (sizeNeeded > 0)
    {
        auto buffer = new char[sizeNeeded+1];
        WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, wstr.c_str(), wstr.size(), buffer, sizeNeeded, NULL, NULL);
        buffer[sizeNeeded] = '\0';
        auto utf8 = std::string{ buffer, buffer + sizeNeeded };
        delete[] buffer;
        return utf8;
    }
    else
        return std::string{};
}

std::wstring const utf8ToUtf16(std::string const str)
{
    auto sizeNeeded = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), str.size(), 0, 0);
    if (sizeNeeded > 0)
    {
        auto buffer = new wchar_t[sizeNeeded+1];
        MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, str.c_str(), str.size(), buffer, sizeNeeded);
        buffer[sizeNeeded] = L'\0';
        auto utf16 = std::wstring{ buffer, buffer + sizeNeeded };
        delete[] buffer;
        return utf16;
    }
    else
        return std::wstring{};
}

#ifdef UNICODE
tstring transformString(std::string const str)
{
    return utf8ToUtf16(str);
}

tstring transformString(std::wstring const wstr)
{
    return wstr;
}

tstring transformString(const char* cstr)
{
    return transformString(std::string{ cstr });
}

tstring transformString(const wchar_t* wcstr)
{
    return std::wstring{ wcstr };
}

#else
tstring transformString(std::string const str)
{
    return str;
}

tstring transformString(std::wstring const wstr)
{
    return utf16ToUtf8(wstr);
}

tstring transformString(const char* cstr)
{
    return std::string{ cstr };
}

tstring transformString(const wchar_t* wcstr)
{
    return transformString(std::wstring{ wcstr });
}

#endif