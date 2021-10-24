/*
  switch-remote-play
    Remote PC connection focused on allowing PC games to be played on the switch
*/

#include <string>
#include <iostream>
#include <thread>
#include <atomic>
#include <switch.h>
#include "ScreenRenderer.h"
#include "MainScreen.h"
#include "InputThread.h"
#include "network/NetworkDiscovery.h"
#include "stream/VideoStream.h"
#include "stream/StreamDecoder.h"
#include "stream/PcmStream.h"
#include "system/SystemSetup.h"
#include "network/NetworkConfiguration.h"
#include "srp/codec/general/GenericCodecConfiguration.h"
#include "srp/codec/h264/H264Configuration.h"
#include "srp/codec/h264_amf/H264AmfConfiguration.h"
#include "srp/decoder/DecoderConfiguration.h"
#include "srp/controller/ControllerConfiguration.h"
#include "srp/mouse/MouseConfiguration.h"
#include "srp/keyboard/KeyboardConfiguration.h"
#include "srp/touch/TouchConfiguration.h"

namespace
{
    SDL_Color constexpr bgCol = {20, 20, 20, 255};
    SDL_Color constexpr pendingStreamBgCol = { 60, 60, 60, 255 };

    uint32_t constexpr fontSize = 24;

    //30fps refresh rate
    auto constexpr thirtyThreeMs = std::chrono::duration<int, std::milli>(33);
    auto constexpr oneSecond = std::chrono::duration<int, std::milli>(1000);

    std::atomic_int32_t streamState;

    void processStream(VideoStream& stream, AVPacket& streamPacket, StreamDecoder*& streamDecoder,
                        SDL_Texture* rendererScreenTexture, SDL_Rect& renderRegion,
                        ScreenRenderer& screen, std::thread& gamepadThread)
    {
        if(stream.Read(streamPacket))
        {
            if(streamDecoder->DecodeFramePacket(streamPacket))
            {
                // render frame data - expecting YUV420 format
                // (stride values represent space between horizontal lines across screen)
                auto decodedFrame = streamDecoder->DecodedFrame();

                auto yPlane = decodedFrame.data[0];
                auto yPlaneStride = decodedFrame.width;
                auto uPlane = decodedFrame.data[1];
                auto uPlaneStride = decodedFrame.width/2;
                auto vPlane = decodedFrame.data[2];
                auto vPlaneStride = decodedFrame.width/2;

                SDL_UpdateYUVTexture(rendererScreenTexture, &renderRegion, 
                                    yPlane, yPlaneStride,
                                    uPlane, uPlaneStride, 
                                    vPlane, vPlaneStride);

                screen.RenderScreenTexture();
            }

            av_packet_unref(&streamPacket);
        }
        else
        {
            stream.CloseStream();
            streamDecoder->Flush();
            streamDecoder->Cleanup();
            stream.Cleanup();
            streamState.store(StreamState::INACTIVE, std::memory_order_release);
            delete streamDecoder;
            streamDecoder = nullptr;

            if(gamepadThread.joinable())
                gamepadThread.join();
        }
    };

    void SaveConfigData(EncoderConfig const encoderData, 
    DecoderData const decoderData, 
    controller::ControllerConfig const controllerData,
    mouse::MouseConfig const mouseData,
    keyboard::KeyboardConfig const keyboardData,
    touch::TouchConfig const touchData)
    {
        {
            auto generalConf = GenericCodecConfiguration{};
            generalConf.Save(encoderData.commonSettings);
            switch(encoderData.commonSettings.videoCodec)
            {
                case ffmpeg::VideoCodec::H264:
                {
                    auto conf = H264Configuration{};
                    conf.Save(encoderData.cpuSettings);
                }
                break;

                case ffmpeg::VideoCodec::H264_AMF:
                {
                    auto conf = H264AmfConfiguration{};
                    conf.Save(encoderData.amdSettings);
                }
                break;

                case ffmpeg::VideoCodec::H264_NVENC:
                case ffmpeg::VideoCodec::H264_QSV:
                break;
            }
        }
        
        {
            auto decoderConf = DecoderConfiguration{};
            decoderConf.Save(decoderData);
        }

        {
            auto conf = ControllerConfiguration{};
            conf.Save(controllerData);
        }

        {
            auto conf = MouseConfiguration{};
            conf.Save(mouseData);
        }

        {
            auto conf = KeyboardConfiguration{};
            conf.Save(keyboardData);
        }

        {
            auto conf = TouchConfiguration{};
            conf.Save(touchData);
        }
    }
}

int main(int argc, char **argv)
{
    std::cout << "Initialising Switch\n";
    initialiseSwitch(); 
    streamState = { StreamState::INACTIVE };
    
    ScreenRenderer screen;
    std::cout << "Initialising Screen\n";
    bool initOK = screen.Initialise(1280, 720, false);

    if(!initOK)
    {
        //send a copy to the nxlink server
        auto errorMessage = "Failed to initialise screen renderer...";
        std::cout << errorMessage << std::endl;
        
        //doesn't display for now on the switch screen from nxlink redirecting stdio
        consoleInit(NULL);
        std::cout << errorMessage << std::endl;
        
        consoleUpdate(NULL);
        int countdown = 3;
        while(countdown > 0)
        {
            --countdown;
            std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(1000));
            consoleUpdate(NULL);
        }

        consoleExit(NULL);
        
        cleanupSystem();
        return -1;
    }

    auto const startupNetworkSettings = NetworkConfiguration().Data();
    std::string broadcastAddress{startupNetworkSettings.broadcastIP};
    std::cout << "Initialising Network Discovery\n";
    std::cout << "Broadcasting discovery on address " << broadcastAddress << "\n";
    NetworkDiscovery network {startupNetworkSettings.handshakePort, broadcastAddress, startupNetworkSettings.broadcastPort};
    
    std::thread gamepadThread{};
    
    std::cout << "Initialising PcmStream\n";
    PcmStream audioStream {startupNetworkSettings.audioPort};
    std::cout << "Initialising Video Stream\n";
    VideoStream stream{};

    StreamDecoder* streamDecoder {nullptr};
    AVPacket streamPacket{};
    auto rendererScreenTexture = screen.GetScreenTexture();
    auto renderRegion = screen.Region();
    
    std::cout << "Loading System Fonts\n";
    auto systemFont = loadSystemFont(screen.Renderer(), fontSize, {255, 255, 255, 255});
    setMainSystemFont(systemFont);
    
    auto runApp {true};
    std::cout << "Starting main loop\n";

    //initialise hid
    PadState mainPad {0};
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    padInitializeDefault(&mainPad);

    std::cout << "Creating menu selection screens\n";
    auto menuScreens = MenuSelection{};
    
    while(appletMainLoop() && runApp)
    {
        switch(streamState.load(std::memory_order_acquire))
        {
            case StreamState::INACTIVE:
            {
                screen.ClearScreen(bgCol);

                if(audioStream.Running())
                    audioStream.Shutdown();
        
                padUpdate(&mainPad);

                auto kDown = padGetButtonsDown(&mainPad);

                if(kDown & HidNpadButton_Plus)
                    streamState.store(StreamState::QUIT, std::memory_order_release);
                else if(kDown & HidNpadButton_R)
                    streamState.store(StreamState::REQUESTED, std::memory_order_release);

                menuScreens.ProcessInput(mainPad);
                
                if(kDown & HidNpadButton_L)
                {
                    if(!network.Searching())
                        network.Search();
                }

                menuScreens.RenderTitle(screen.Renderer(), systemFont);
                menuScreens.RenderNetworkStatus(screen.Renderer(), systemFont, network);
                menuScreens.Render(screen.Renderer(), systemFont);
                screen.PresentScreen();
                
                // no point thrashing the screen to refresh text
                std::this_thread::sleep_for(thirtyThreeMs);
            }
            break;

            case StreamState::REQUESTED:
            {
                //display on the screen a connection is pending
                screen.ClearScreen(pendingStreamBgCol);
                menuScreens.RenderTitle(screen.Renderer(), systemFont);
                menuScreens.RenderPendingConnection(screen.Renderer(), systemFont);
                menuScreens.RenderNetworkStatus(screen.Renderer(), systemFont, network);
                
                screen.PresentScreen();
                SaveConfigData(menuScreens.GetFfmpegSettings(), 
                            menuScreens.GetDecoderSettings(), 
                            menuScreens.GetControllerSettings(),
                            menuScreens.MouseSettings(),
                            menuScreens.KeyboardSettings(),
                            menuScreens.TouchSettings());

                auto networkSettings = menuScreens.NetworkSettings();
                if(network.HostFound() || networkSettings.manualIPEnabled)
                {
                    auto ip = std::string{};
                    if(networkSettings.manualIPEnabled)
                        ip = networkSettings.manualIP;
                    else
                        ip = network.IPAddress();
                  
                    auto ffmpegConfig = menuScreens.GetFfmpegSettings();
                    auto controllerConfig = menuScreens.GetControllerSettings();
                    auto decoderConfig = menuScreens.GetDecoderSettings();
                    auto mouseConfig = menuScreens.MouseSettings();
                    auto keyboardConfig = menuScreens.KeyboardSettings();
                    auto touchConfig = menuScreens.TouchSettings();

                    runStartConfiguredStreamCommand(ip, startupNetworkSettings.commandPort, 
                        ffmpegConfig, 
                        controllerConfig,
                        mouseConfig,
                        keyboardConfig,
                        touchConfig);
                    auto streamOn = stream.WaitForStream(decoderConfig, startupNetworkSettings.videoPort);

                    if(streamOn)
                    {
                        auto streamInfo = stream.StreamInfo();
                        if(streamDecoder != nullptr)
                            delete streamDecoder;

                        streamDecoder = new StreamDecoder(streamInfo->codecpar);
                        gamepadThread = std::thread(runGamepadThread, ip, startupNetworkSettings.gamepadPort);
                        streamState.store(StreamState::ACTIVE, std::memory_order_release);

                        if(audioStream.Ready() && !audioStream.Running())
                            audioStream.Start();
                    }
                }
                else // no host to connect to
                {
                    std::this_thread::sleep_for(oneSecond);
                    streamState.store(StreamState::INACTIVE, std::memory_order_release);
                }
            }
            break;

            case StreamState::ACTIVE:
            {
                processStream(stream, streamPacket, streamDecoder, 
                                rendererScreenTexture, renderRegion, 
                                screen, gamepadThread);
                padUpdate(&mainPad); // stop the '+' button from quitting the app when stream stops
            }
            break;

            case StreamState::QUIT:
            {
                runApp = false;
            }
            break;
        }
    }

    SDL_Quit();

    //wait here if the stream is still on so we can clean it up properly
    if(streamState.load() == StreamState::ACTIVE)
    {
        while(streamState.load() == StreamState::ACTIVE)
            std::this_thread::sleep_for(thirtyThreeMs);
        stream.Cleanup();

        if(streamDecoder != nullptr)
        {
            streamDecoder->Cleanup();
            delete streamDecoder;
            streamDecoder = nullptr;
        }
    }
    
    network.Shutdown();

    if(gamepadThread.joinable())
        gamepadThread.join();

    if(audioStream.Running())
        audioStream.Shutdown();
    
    auto libnxRes = audoutStopAudioOut();
    if(!R_SUCCEEDED(libnxRes))
        std::cout << "Failed to call audoutStopAudioOut with result: " << libnxRes << std::endl;
    freeFont(systemFont);
    cleanupSystem();
    
    return 0;
}