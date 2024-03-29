#include "FFMPEGHelper.h"
#include "FfmpegArgParser.h"
#include "srp/utils/Configuration.h"
#include <iostream>
#include <sstream>

std::string parentDirectory{};
std::string ffmpegPath{};

void SetParentDirectory(std::string path)
{
    parentDirectory = path;
    ffmpegPath = path + "\\ffmpeg.exe";
}

// Generate the command line argument string to execute ffmpeg
std::string CreateVideoCommandLineArg(DisplayDeviceInfo const display, EncoderConfig const config, std::string const ip, uint16_t port)
{
    auto parser = FfmpegArgParser(config, display);
    auto const connectionIP = "tcp://" + ip + ":" + std::to_string(port);
    auto const parsedArgs = parser.CompleteArgs() + " " + connectionIP;
    auto const completeCmd = ffmpegPath + " " + parsedArgs;

    return completeCmd;
}

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, std::string const ip, uint16_t port)
{
    auto const & filePath = ffmpegPath;

    auto const connectionIP = "udp://" + ip + ":" + std::to_string(port);
    auto const inputArgs = " -y -f dshow -audio_buffer_size 30 -i audio=\"virtual-audio-capturer\" ";
    auto const qualityArgs = "-f s16le "; //s16le
    auto const sampleRateArg = "-ar ";
    auto const channelArgs = " -ac 2 "; //s8 -- -c:a pcm_s16le 
    auto const packetArg = "pkt_size=";
    std::stringstream args{};
    args << filePath << inputArgs << qualityArgs << sampleRateArg << sampleRate << channelArgs;
    args << connectionIP;// << "?" << packetArg << packetSize;

    return args.str();
}

// Create a windows process to start the ffmpeg.exe application via CMD in a new window
PROCESS_INFORMATION StartStream(DisplayDeviceInfo const display, EncoderConfig const config, std::string const ip, uint16_t port, bool showEncoderOutput, Log& logger, bool& started)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    auto args = CreateVideoCommandLineArg(display, config, ip, port);
    
    auto configFile = Configuration{ parentDirectory };
    auto ffmpegProcessFlag = 0;
    if (!showEncoderOutput)
        ffmpegProcessFlag = CREATE_NO_WINDOW;

    if (!CreateProcessA(NULL, (LPSTR)args.c_str(), NULL, NULL, FALSE, ffmpegProcessFlag, NULL, NULL, &si, &pi))
    {
        //error occured
        auto const errorValue = std::to_string(GetLastError());
        auto const errorMessage = "Failed to start process. CreateProcess error code: " + errorValue + "\n";
        logger.Write(errorMessage, LogImportance::High, true);
        logger.Write("Argument line passed to cmd:\n\n" + args + "\n", LogImportance::High);
        started = false;

        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
        started = true;

    CloseHandle(si.hStdInput);
    CloseHandle(si.hStdOutput);
    CloseHandle(si.hStdError);

    return pi;
}

PROCESS_INFORMATION StartAudio(std::string const ip, uint16_t port, bool showAudioEncoderWindow, Log& logger, bool& started)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    auto constexpr samplerate = 48000;
    auto constexpr framerate = 100;
    auto constexpr inputSampleCount = samplerate / framerate;
    auto constexpr channels = 2; // stereo audio
    auto constexpr bitrate = 16;

    auto constexpr data_size = inputSampleCount * channels * bitrate/8;
    
    auto const args = CreateAudioCommandLineArg(samplerate, data_size, ip, port);

    auto audioProcessFlag = CREATE_NO_WINDOW;
    if (showAudioEncoderWindow)
        audioProcessFlag = CREATE_NEW_CONSOLE;

    if (!CreateProcessA(NULL, (LPSTR)args.c_str(), NULL, NULL, FALSE, audioProcessFlag, NULL, NULL, &si, &pi))
    {
        //error occured
        auto const errorValue = std::to_string(GetLastError());
        auto const errorMessage = "Failed to start process. CreateProcess error code: " + errorValue + "\n";
        logger.Write(errorMessage, LogImportance::High, true);
        logger.Write("Argument line passed to cmd:\n\n" + args + "\n", LogImportance::High);
        started = false;

        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else
        started = true;

    CloseHandle(si.hStdInput);
    CloseHandle(si.hStdOutput);
    CloseHandle(si.hStdError);

    return pi;
}

std::string ConfigToString(EncoderConfig const config)
{
    auto const& commonSettings = config.commonSettings;
    auto const& desktopRes = commonSettings.desktopResolution;
    auto const& switchRes = commonSettings.switchResolution;
    auto const vsyncText = ffmpeg::vsyncModeToStr(commonSettings.vsyncMode);
    auto const codecMode = ffmpeg::videoCodecToStr(commonSettings.videoCodec);

    std::stringstream args{};
    args << "Monitor Number: " << commonSettings.monitorNumber << "\n";
    args << "Video Capture Size(x,y): " << desktopRes.width << ", " << desktopRes.height << "\n";
    args << "Stream Scale Size(x,y): " << switchRes.width << ", " << switchRes.height << "\n\n";
    args << "Chosen Codec: " << codecMode << "\n";
    args << "Vsync Mode: " << (int)commonSettings.vsyncMode << " (" << vsyncText << ")" << "\n";
    args << "Target Framerate: " << commonSettings.desiredFrameRate << " fps" << "\n";
    args << "Target Stream Bitrate: " << commonSettings.bitrateKB << " kb/s" << "\n\n";

    return args.str();
}