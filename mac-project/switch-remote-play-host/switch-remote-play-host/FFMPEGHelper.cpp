#include "FFMPEGHelper.h"
#include <iostream>
#include <sstream>

auto ffmpegProcessFlag{ 0 }; // 0 - in parent process window
auto audioProcessFlag{ 0 };
std::string parentDirectory{};
std::string ffmpegPath{};
std::string screenIndex{};

void SetParentDirectory(std::string path)
{
    parentDirectory = path;
    ffmpegPath = path + "\\ffmpeg.exe";
}

void SetScreenIndex(std::string index)
{
    screenIndex = index;
}

// Generate the command line argument string to execute ffmpeg
std::string CreateVideoCommandLineArg(EncoderConfig const config, std::string const ip, uint16_t port)
{
    using namespace std;

    auto const & filePath = ffmpegPath;

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
    args << filePath << " -probesize 32 ";
    args << "-hwaccel " << HWAccelModeToString(config.hwaccelMode) << " ";
    args << "-y -f gdigrab ";
    args << "-framerate " << config.desiredFrameRate << " ";
    args << "-vsync " << vsyncMode << " ";
    args << "-video_size " << config.videoX << "x" << config.videoY << " ";
    args << "-i desktop -f h264 ";
    args << "-vcodec " << VideoCodecModeToString(config.videoCodecMode) << " ";
    args << "-vf \"scale=" << config.scaleX << "x" << config.scaleY << "\" ";
    args << "-preset " << EncoderPresetToString(config.preset) << " ";
    args << "-crf " << config.constantRateFactor << " ";
    args << "-tune zerolatency -pix_fmt yuv420p ";//-profile:v baseline ";
    args << "-x264-params \"nal-hrd=vbr:opencl=true\" ";
    args << "-b:v " << config.bitrateKB << "k -minrate " << config.bitrateKB << "k -maxrate " << config.bitrateKB << "k ";
    args << "-bufsize " << config.bitrateKB << "k " << connectionIP;

    std::cout << "\n" << args.str() << "\n";

    return args.str();
}

std::string CreateAudioCommandLineArg(int sampleRate, int packetSize, std::string const ip, uint16_t port)
{
    using namespace std;

    auto const & filePath = ffmpegPath;

    auto const connectionIP = "udp://" + ip + ":" + std::to_string(port);
    auto const inputArgs = " -y  -f dshow -i audio=\"virtual-audio-capturer\" ";
    auto const qualityArgs = "-f s16le ";
    auto const sampleRateArg = "-ar ";
    auto const channelArgs = " -ac 2 -c:a pcm_s16le ";
    auto const packetArg = "pkt_size=";
    stringstream args;
    args << filePath << inputArgs << qualityArgs << sampleRateArg << sampleRate << channelArgs;
    args << connectionIP;// << "?" << packetArg << packetSize;

    return args.str();
}

std::vector<std::string> CreateEncoderArgs(EncoderConfig const config, std::string const ip, uint16_t port)
{
    std::vector<std::string> arg_arr{};
    
    using namespace std;
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
        vsyncMode = "vfr";
        break;
    }

    auto const connectionIP = "tcp://" + ip + ":" + std::to_string(port);
    stringstream args;
//    args << filePath << " -probesize 32 ";
    arg_arr.push_back("ffmpeg-arm");
    arg_arr.push_back("-probesize");
    arg_arr.push_back("32");
    
    //args << "-hwaccel " << HWAccelModeToString(config.hwaccelMode) << " ";
    arg_arr.push_back("-hwaccel");
    arg_arr.push_back(HWAccelModeToString(config.hwaccelMode));
    arg_arr.push_back("-y");
    
//    args << "-y -f gdigrab ";
    arg_arr.push_back("-f");
    arg_arr.push_back("avfoundation");
    arg_arr.push_back("-capture_cursor");
    arg_arr.push_back("1");
    arg_arr.push_back("-i");
    arg_arr.push_back(screenIndex + ":none");
    
    arg_arr.push_back("-capture_raw_data");
    arg_arr.push_back("0");
    
//    args << "-framerate " << config.desiredFrameRate << " ";
    arg_arr.push_back("-framerate");
    arg_arr.push_back(std::to_string(config.desiredFrameRate));
    
//    args << "-vsync " << vsyncMode << " ";
    arg_arr.push_back("-vsync");
    arg_arr.push_back(vsyncMode);
    
//    args << "-video_size " << config.videoX << "x" << config.videoY << " ";
    arg_arr.push_back("-video_size");
    args.str("");
    args.clear();
    args << config.videoX << "x" << config.videoY;
    arg_arr.push_back(args.str());
    
//    args << "-i desktop -f h264 ";
//    args << "-vcodec " << VideoCodecModeToString(config.videoCodecMode) << " ";
    arg_arr.push_back("-vcodec");
    arg_arr.push_back(VideoCodecModeToString(config.videoCodecMode));
    
//    args << "-vf \"scale=" << config.scaleX << "x" << config.scaleY << "\" ";
    arg_arr.push_back("-vf");
    args.str("");
    args.clear();
    args << "scale=" << config.scaleX << "x" << config.scaleY;
    arg_arr.push_back(args.str());
    
//    args << "-preset " << EncoderPresetToString(config.preset) << " ";
    arg_arr.push_back("-preset");
    arg_arr.push_back(EncoderPresetToString(config.preset));
    
//    args << "-crf " << config.constantRateFactor << " ";
    arg_arr.push_back("-crf");
    arg_arr.push_back(std::to_string(config.constantRateFactor));
    
//    args << "-tune zerolatency -pix_fmt yuv420p ";//-profile:v baseline ";
    arg_arr.push_back("-tune");
    arg_arr.push_back("zerolatency");
    // on retina displays this incurs an extra transcoding step
    // as the retina display doesn't natively support YUV420 progressive
    arg_arr.push_back("-pix_fmt");
    arg_arr.push_back("yuv420p");
    
//    args << "-x264-params \"nal-hrd=vbr:opencl=true\" ";
    arg_arr.push_back("-x264-params");
    arg_arr.push_back("nal-hrd=vbr");
    
    //all the below parameters need to be specified correctly for it to work
    //without causing the stream to stop / underflow locking up the switch
//    arg_arr.push_back("-profile:v");
//    arg_arr.push_back("main");
    
//    args << "-b:v " << config.bitrateKB << "k -minrate " << config.bitrateKB << "k -maxrate " << config.bitrateKB << "k ";
//    auto bitrate = std::to_string(config.bitrateKB) + "k";
//    arg_arr.push_back("-b:v");
//    arg_arr.push_back(bitrate);
//
//    arg_arr.push_back("-minrate");
//    arg_arr.push_back(bitrate);
//
//    arg_arr.push_back("-maxrate");
//    arg_arr.push_back(bitrate);
//
////    args << "-bufsize " << config.bitrateKB << "k " << connectionIP;
//    arg_arr.push_back("-bufsize");
//    arg_arr.push_back(bitrate);
    
    arg_arr.push_back("-f");
    arg_arr.push_back("mpegts");
    arg_arr.push_back(connectionIP);
    
    for(auto const & arg : arg_arr)
        std::cout << arg << " ";
    
    std::cout << "\n\n";
    
    return arg_arr;
}

Process StartStream(EncoderConfig const config, std::string const ip, uint16_t port, bool& started)
{
    auto args = CreateEncoderArgs(config, ip, port);
    auto process = Process{"ffmpeg", args, true};
    if (!process.Running())
    {
        //error occured
        std::cout << "Failed to start process\n";
        started = false;
    }
    else
        started = true;

    return process;
}

std::string ConfigToString(EncoderConfig const config)
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
