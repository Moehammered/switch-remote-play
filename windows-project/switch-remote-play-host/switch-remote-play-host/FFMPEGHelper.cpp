#include "FFMPEGHelper.h"
#include <iostream>
#include <sstream>
#include "Configuration.h"

std::string parentDirectory{};
std::string ffmpegPath{};

void SetParentDirectory(std::string path)
{
    parentDirectory = path;
    ffmpegPath = path + "\\ffmpeg.exe";
}

// Generate the command line argument string to execute ffmpeg
std::string CreateVideoCommandLineArg(EncoderConfig const config, std::string const ip, uint16_t port)
{
    using namespace std;

    auto const & filePath = ffmpegPath;

    auto vsyncMode = "1";
    switch (config.commonSettings.vsyncMode)
    {
    case VsyncMode::VSYNC_PASSTHROUGH:
        vsyncMode = "passthrough"; //each frame is passed to the muxer
        break;
    case VsyncMode::CONSTANT_FPS:
        vsyncMode = "cfr"; //constant fps
        break;
    case VsyncMode::VARIABLE_FPS:
        vsyncMode = "vfr"; //variable fps (prevent duplicate frames)
        break;
    case VsyncMode::VSYNC_DROP_TIME:
        vsyncMode = "drop"; //same as passthrough, but removes timestamps
        break;
    case VsyncMode::VSYNC_AUTO:
        vsyncMode = "-1"; //automatically choose between 1 or 2
        break;
    default:
        vsyncMode = "vfr";
        break;
    }

    auto const connectionIP = "tcp://" + ip + ":" + std::to_string(port);
    auto const& cpuSettings = config.cpuSettings;
    auto const& desktopRes = config.commonSettings.desktopResolution;
    auto const& switchRes = config.commonSettings.switchResolution;
    stringstream args;
    args << filePath << " -probesize 32 ";
    args << "-hwaccel " << HWAccelModeToStr(config.commonSettings.hwaccelMode) << " ";
    args << "-y -f gdigrab ";
    args << "-framerate " << config.commonSettings.desiredFrameRate << " ";
    args << "-vsync " << vsyncMode << " ";
    args << "-video_size " << desktopRes.width << "x" << desktopRes.height << " ";
    args << "-i desktop -f h264 ";
    args << "-vcodec " << VideoCodecToStr(config.commonSettings.videoCodec) << " ";
    args << "-vf \"scale=" << switchRes.width << "x" << switchRes.height << "\" ";
    args << "-preset " << h264::EncoderPresetToStr(cpuSettings.Preset) << " ";
    args << "-crf " << cpuSettings.ConstantRateFactor << " ";
    args << "-tune zerolatency -pix_fmt yuv420p ";//-profile:v baseline ";
    args << "-x264-params \"nal-hrd=vbr:opencl=true\" ";
    /*args << "-b:v " << config.bitrateKB << "k -minrate " << config.bitrateKB << "k -maxrate " << config.bitrateKB << "k ";
    args << "-bufsize " << config.bitrateKB << "k " << connectionIP;*/
    args << connectionIP;

    std::cout << "\n" << args.str() << "\n";

    return args.str();
}

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, std::string const ip, uint16_t port)
{
    using namespace std;

    auto const & filePath = ffmpegPath;

    auto const connectionIP = "udp://" + ip + ":" + std::to_string(port);
    auto const inputArgs = " -y -f dshow -audio_buffer_size 30 -i audio=\"virtual-audio-capturer\" ";
    auto const qualityArgs = "-f s16le "; //s16le
    auto const sampleRateArg = "-ar ";
    auto const channelArgs = " -ac 2 "; //s8 -- -c:a pcm_s16le 
    auto const packetArg = "pkt_size=";
    stringstream args;
    args << filePath << inputArgs << qualityArgs /*<< sampleRateArg << sampleRate << channelArgs*/;
    args << connectionIP;// << "?" << packetArg << packetSize;

    return args.str();
}

// Create a windows process to start the ffmpeg.exe application via CMD in a new window
PROCESS_INFORMATION StartStream(EncoderConfig const config, std::string const ip, uint16_t port, bool showEncoderOutput, bool& started)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    si.cb = sizeof(si);

    auto args = CreateVideoCommandLineArg(config, ip, port);
    
    auto configFile = Configuration{ parentDirectory };
    auto ffmpegProcessFlag = 0;
    if (!showEncoderOutput)
        ffmpegProcessFlag = CREATE_NO_WINDOW;

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

PROCESS_INFORMATION StartAudio(std::string const ip, uint16_t port, bool showAudioEncoderWindow, bool& started)
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

    std::cout << "audio--\n\n" << args << "\n\n";

    auto audioProcessFlag = 0;
    if (showAudioEncoderWindow)
        audioProcessFlag = CREATE_NEW_CONSOLE;

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

std::string ConfigToString(EncoderConfig const config)
{
    using namespace std;

    auto vsyncText = "";
    switch (config.commonSettings.vsyncMode)
    {
    case VsyncMode::VSYNC_PASSTHROUGH:
        vsyncText = "passthrough"; //each frame is passed to the muxer
        break;
    case VsyncMode::CONSTANT_FPS:
        vsyncText = "constant frame rate"; //constant fps
        break;
    case VsyncMode::VARIABLE_FPS:
        vsyncText = "variable frame rate"; //variable fps (prevent duplicate frames)
        break;
    case VsyncMode::VSYNC_DROP_TIME:
        vsyncText = "drop duplicate frames"; //same as passthrough, but removes timestamps
        break;
    case VsyncMode::VSYNC_AUTO:
        vsyncText = "auto"; //automatically choose between 1 or 2
        break;
    default:
        vsyncText = "UNKNOWN";
        break;
    }

    auto const& desktopRes = config.commonSettings.desktopResolution;
    auto const& switchRes = config.commonSettings.switchResolution;
    stringstream args;
    args << "Vsync Mode: " << (int)config.commonSettings.vsyncMode << " (" << vsyncText << ")" << endl;
    args << "Target Framerate: " << config.commonSettings.desiredFrameRate << " fps" << endl;
    args << "Video Capture Size(x,y): " << desktopRes.width << ", " << desktopRes.height << endl;
    args << "Stream Scale Size(x,y): " << switchRes.width << ", " << switchRes.height << endl;
    args << "Preset: " << h264::EncoderPresetToStr(config.cpuSettings.Preset) << endl;
    args << "CRF: " << config.cpuSettings.ConstantRateFactor << endl;
    //args << "Target Stream Bitrate: " << config.bitrateKB << " kb/s" << endl;

    return args.str();
}