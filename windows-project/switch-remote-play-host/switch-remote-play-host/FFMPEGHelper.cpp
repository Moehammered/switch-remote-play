#include "FFMPEGHelper.h"
#include <Shlwapi.h>
#include <iostream>
#include <sstream>

auto ffmpegProcessFlag{ 0 }; // 0 - in parent process window
auto audioProcessFlag{ CREATE_NO_WINDOW };

// Generate the command line argument string to execute ffmpeg
std::string CreateVideoCommandLineArg(FFMPEG_Config const config, std::string const ip, uint16_t port)
{
    using namespace std;

    char filePath[MAX_PATH];
    GetModuleFileNameA(NULL, filePath, MAX_PATH);
    PathRemoveFileSpecA(filePath);
    PathCombineA(filePath, filePath, "ffmpeg.exe");

    auto vsyncMode = "1";
    switch (config.vsyncMode)
    {
    case 0:
        vsyncMode = "passthrough"; //each frame is passed to the muxer
        break;
    case 1:
        vsyncMode = "cfr"; //constant fps
        break;
    case 2:
        vsyncMode = "vfr"; //variable fps (prevent duplicate frames)
        break;
    case 3:
        vsyncMode = "drop"; //same as passthrough, but removes timestamps
        break;
    case -1:
        vsyncMode = "-1"; //automatically choose between 1 or 2
        break;
    default:
        vsyncMode = "drop";
        break;
    }

    auto const connectionIP = "tcp://" + ip + ":" + std::to_string(port);
    stringstream args;
    args << filePath << " -probesize 32 -hwaccel auto -y -f gdigrab ";
    args << "-framerate " << config.desiredFrameRate << " ";
    args << "-vsync " << vsyncMode << " ";
    args << "-video_size " << config.videoX << "x" << config.videoY << " ";
    args << "-i desktop -f h264 -vf ";
    args << "\"scale=" << config.scaleX << "x" << config.scaleY << "\" ";
    args << "-preset ultrafast -tune zerolatency -pix_fmt yuv420p -profile:v baseline ";
    args << "-x264-params \"nal-hrd=cbr\" ";
    args << "-b:v " << config.bitrateKB << "k -minrate " << config.bitrateKB << "k -maxrate " << config.bitrateKB << "k ";
    args << "-bufsize " << config.bitrateKB << "k " << connectionIP;

    return args.str();
}

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, std::string const ip, uint16_t port)
{
    using namespace std;

    char filePath[MAX_PATH];
    GetModuleFileNameA(NULL, filePath, MAX_PATH);
    PathRemoveFileSpecA(filePath);
    PathCombineA(filePath, filePath, "ffmpeg.exe");

    auto const connectionIP = "udp://" + ip + ":" + std::to_string(port);
    auto const inputArgs = " -y  -f dshow -i audio=\"virtual-audio-capturer\" ";
    auto const qualityArgs = "-f s16le ";
    auto const sampleRateArg = "-ar ";
    auto const channelArgs = " -ac 2 -c:a pcm_s16le ";
    auto const packetArg = "pkt_size=";
    stringstream args;
    args << filePath << inputArgs << qualityArgs << sampleRateArg << sampleRate << channelArgs;
    args << connectionIP << "?" << packetArg << packetSize;

    return args.str();
}

// Create a windows process to start the ffmpeg.exe application via CMD in a new window
PROCESS_INFORMATION StartStream(FFMPEG_Config const config, std::string const ip, uint16_t port, bool& started)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    auto args = CreateVideoCommandLineArg(config, ip, port);

    if (!CreateProcessA(NULL, (LPSTR)args.c_str(), NULL, NULL, FALSE, ffmpegProcessFlag, NULL, NULL, &si, &pi))
    {
        //error occured
        std::cout << "Failed to start process. CreateProcess error code: " << GetLastError() << std::endl;
        std::cout << "Argument line passed to cmd: " << std::endl << std::endl << args << std::endl;
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

PROCESS_INFORMATION StartAudio(std::string const ip, uint16_t port, bool& started)
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

    if (!CreateProcessA(NULL, (LPSTR)args.c_str(), NULL, NULL, FALSE, audioProcessFlag, NULL, NULL, &si, &pi))
    {
        //error occured
        std::cout << "Failed to start process. CreateProcess error code: " << GetLastError() << std::endl;
        std::cout << "Argument line passed to cmd: " << std::endl << std::endl << args << std::endl;
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

std::string ConfigToString(FFMPEG_Config const config)
{
    using namespace std;

    auto vsyncText = "";
    switch (config.vsyncMode)
    {
    case 0:
        vsyncText = "passthrough"; //each frame is passed to the muxer
        break;
    case 1:
        vsyncText = "constant frame rate"; //constant fps
        break;
    case 2:
        vsyncText = "variable frame rate"; //variable fps (prevent duplicate frames)
        break;
    case 3:
        vsyncText = "drop duplicate frames"; //same as passthrough, but removes timestamps
        break;
    case -1:
        vsyncText = "auto"; //automatically choose between 1 or 2
        break;
    default:
        vsyncText = "UNKNOWN";
        break;
    }

    stringstream args;
    args << "Vsync Mode: " << config.vsyncMode << " (" << vsyncText << ")" << endl;
    args << "Target Framerate: " << config.desiredFrameRate << " fps" << endl;
    args << "Video Capture Size(x,y): " << config.videoX << ", " << config.videoY << endl;
    args << "Stream Scale Size(x,y): " << config.scaleX << ", " << config.scaleY << endl;
    args << "Target Stream Bitrate: " << config.bitrateKB << " kb/s" << endl;

    return args.str();
}