#include "Configuration.h"
#include "FileOperations.h"
#include <sstream>
#include <iterator>
#include <iostream>

auto constexpr FFMPEG_ENCODER_OUTPUT = "show_ffmpeg_encoder_output";
auto constexpr FFMPEG_AUDIO_OUTPUT_WINDOW = "show_ffmpeg_audio_output_window";

bool YesToBool(std::string s)
{
    return s == "yes" || s == "y";
}

std::string BoolToYes(bool enabled)
{
    return enabled ? "yes" : "no";
}

Configuration::Configuration(std::string parentFolder) : parentFolder{}, absolutePath{}, data{}
{
    this->parentFolder = parentFolder;
    absolutePath = parentFolder + "\\config.ini";
    data = ReadConfigFile(absolutePath);
}

bool Configuration::SaveFfmpegEncoderOutputOption(bool enabled)
{
    auto newData = std::string{};
    if (ReplaceVariable(data, FFMPEG_ENCODER_OUTPUT, BoolToYes(enabled), newData))
        return SaveConfigFile(newData);
    else
    {
        newData = std::string{ FFMPEG_ENCODER_OUTPUT } + "=" + BoolToYes(enabled) + ";\n";
        newData += data;
        return SaveConfigFile(newData);
    }
}

bool Configuration::ShowFfmpegEncoderOutput()
{
    auto option = std::string{};
    if (ExtractVariable(data, FFMPEG_ENCODER_OUTPUT, option))
        return YesToBool(option);
    else
        return false;
}

bool Configuration::SaveFfmpegAudioEncoderOutputOption(bool enabled)
{
    auto newData = std::string{};
    if (ReplaceVariable(data, FFMPEG_AUDIO_OUTPUT_WINDOW, BoolToYes(enabled), newData))
        return SaveConfigFile(newData);
    else
    {
        newData = std::string{ FFMPEG_AUDIO_OUTPUT_WINDOW } + "=" + BoolToYes(enabled) + ";\n";
        newData += data;
        return SaveConfigFile(newData);
    }
}

bool Configuration::ShowFfmpegAudioEncoderOutputWindow()
{
    auto option = std::string{};
    if (ExtractVariable(data, FFMPEG_AUDIO_OUTPUT_WINDOW, option))
        return YesToBool(option);
    else
        return false;
}

std::string Configuration::ReadConfigFile(std::string const path)
{
    auto temp = std::string{};
    readFileAsText(path, temp);
    return temp;
}

bool Configuration::SaveConfigFile(std::string const & data)
{
    if(saveTextToFile(absolutePath, data))
    {
        this->data = data;
        return true;
    }

    return false;
}

bool Configuration::ExtractVariable(std::string const data, std::string const variable, std::string& result) const
{
    auto location = data.find(variable+"=", 0);
    result = std::string{};

    if(location != std::string::npos)
    {
        auto varStart = data.find('=', location);
        auto varEnd = data.find(';', location);

        if(varStart == std::string::npos || varEnd == std::string::npos)
            return false;
        else
        {
            auto varLength = varEnd-varStart-1;
            result = data.substr(varStart+1, varLength);
            return true;
        }
    }
    else
        return false;
}

bool Configuration::ReplaceVariable(std::string const data, std::string const variable, std::string const value, std::string& newData) const
{
    auto variableStart = data.find(variable+"=", 0);

    if(variableStart != std::string::npos)
    {
        auto varEnd = data.find(';', variableStart);

        if(varEnd == std::string::npos)
            return false;
        else
        {
            auto copy = std::string{data};
            copy.replace(variableStart, varEnd-variableStart, variable+"="+value);
            newData = copy;
            return true;
        }
    }
    else
    {
        std::cout << "couldn't find variable: " << variable+"= in " << std::endl << newData << std::endl;
        return false;
    }
}