#include "Log.h"
#include "DateTimeUtility.h"
#include <filesystem>

Log::Log(encodedOutputStream& stream, LogImportance logFilter, bool logToFile)
    : outputStream{ stream }, dummyStream{ nullptr }, fileStream{},
    minimumLevel{ logFilter }, streamLevel{ LogImportance::Verbose }
{
    if (logToFile)
    {
        auto const directory = std::string{ "logs" };
        if (!std::filesystem::exists(directory))
            std::filesystem::create_directory(directory);

        auto const timestamp = DateToFileName(LocalDateTime());
        auto filename = "log_" + timestamp + ".txt";
        auto const path = directory + "/" + filename;
        fileStream = encodedOutputFile{ path, std::ios::app | std::ios::out };
    }
}

void Log::Write(std::string const str, LogImportance level)
{
    Write(str, level, false);
}

void Log::Write(std::string const str, LogImportance level, bool timestamp)
{
    if (level >= minimumLevel)
    {
        if (timestamp)
        {
            auto const dateTime = LocalDateTime();
            auto timestamp = DateToFileName(dateTime);
            auto transformedTimestamp = transformString(timestamp);

            outputStream << transformedTimestamp << "\n";
            if (fileStream.is_open())
                fileStream << transformedTimestamp << "\n";
        }

        auto transformed = transformString(str);
        outputStream << transformed;
        if (fileStream.is_open())
            fileStream << transformed;
    }
}

void Log::Write(std::wstring const str, LogImportance level)
{
    Write(str, level, false);
}

void Log::Write(std::wstring const str, LogImportance level, bool timestamp)
{
    if (level >= minimumLevel)
    {
        if (timestamp)
        {
            auto const dateTime = LocalDateTime();
            auto timestamp = DateToFileName(dateTime);
            auto transformedTimestamp = transformString(timestamp);

            outputStream << transformedTimestamp << "\n";
            if (fileStream.is_open())
                fileStream << transformedTimestamp << "\n";
        }

        auto transformed = transformString(str);
        outputStream << transformed;
        if (fileStream.is_open())
            fileStream << transformed;
    }
}

Log& Log::operator<<(uint64_t const num)
{
    return (*this) << std::to_string(num);
}

Log& Log::operator<<(std::string const& str)
{
    if (streamLevel >= minimumLevel)
    {
        auto transformed = transformString(str);

        outputStream << transformed;
        if (fileStream.is_open())
            fileStream << transformed;
    }

    return *this;
}

Log& Log::operator<<(std::wstring const& str)
{
    if (streamLevel >= minimumLevel)
    {
        auto transformed = transformString(str);

        outputStream << transformed;
        if (fileStream.is_open())
            fileStream << transformed;
    }

    return *this;
}

Log& Log::operator<<(LogImportance const level)
{
    streamLevel = level;
    return *this;
}