#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include <switch.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "VideoStream.h"
#include "CommandSender.h"
#include <sys/socket.h>

using namespace std;

void RunStopCommandThread(std::string ip, uint16_t port)
{
    int commandSocket = -1;
    if(ConnectTo(ip, 20001, commandSocket))
    {
        if(SendCode(commandSocket, Command::CLOSE_SERVER))
            cout << "Sent Close server command payload" << endl;
        else
            cout << "Error sending close server command payload" << endl;
    }
    close(commandSocket);
}

void CommandStopThreadStart(std::string ip, uint16_t port)
{
    auto t = thread(RunStopCommandThread, ip, port);
    t.join();
}

void RunCommandThread(std::string ip, uint16_t port, STREAM_MODE setting)
{
    int commandSocket = -1;
    if(ConnectTo(ip, 20001, commandSocket))
    {
        auto streamCommand = Command::START_STREAM_LOW_LATENCY_30FPS;
        switch(setting)
        {
            default:
            case STREAM_MODE::LOW_LATENCY_30_FPS:
                streamCommand = Command::START_STREAM_LOW_LATENCY_30FPS;
                break;

            case STREAM_MODE::OK_LATENCY_60_FPS:
                streamCommand = Command::START_STREAM_OK_LATENCY_60FPS;
                break;

            case STREAM_MODE::LOW_LATENCY_V_FPS:
                streamCommand = Command::START_STREAM_LOW_LATENCY_VFPS;
                break;
        }

        if(SendCode(commandSocket, streamCommand))
            cout << "Sent start command payload" << endl;
        else
            cout << "Error sending start payload" << endl;
    }
    close(commandSocket);
}

void CommandConnectionThreadStart(std::string ip, uint16_t port, STREAM_MODE setting)
{
    auto t = thread(RunCommandThread, ip, port, setting);
    t.join();
}

bool ProcessInactiveStreamInput(u32 kDown, StreamConfigData& config, mutex& configMutex)
{
    if (kDown & KEY_PLUS) 
    {
        lock_guard<mutex> guard(configMutex);
        if(!config.streamOn)
            config.quitApp = true; // break in order to return to hbmenu
        
        config.streamOn = false;

        if(config.quitApp)
            return false;
    }

    if (kDown & KEY_A) {
        cout << "A pressed - making bg colour orange?" << endl;
        lock_guard<mutex> guard(configMutex);
        config.bgCol.r = 255;
        config.bgCol.g = 200;
        config.bgCol.b = 100;
    }
    else if (kDown & KEY_B) {
        cout << "B pressed - making bg colour blue" << endl;
        lock_guard<mutex> guard(configMutex);
        config.bgCol.r = 100;
        config.bgCol.g = 100;
        config.bgCol.b = 255;
    }
    if(kDown & KEY_DUP)
    {
        lock_guard<mutex> guard(configMutex);
        auto nextSetting = (int)config.streamSetting - 1;
        auto topSetting = (int)STREAM_MODE::STREAM_MODE_COUNT - 1;
        if(nextSetting < 0)
            config.streamSetting = (STREAM_MODE)topSetting;
    }
    else if(kDown & KEY_DDOWN)
    {
        lock_guard<mutex> guard(configMutex);
        auto nextSetting = (int)config.streamSetting + 1;
        if(nextSetting >= STREAM_MODE::STREAM_MODE_COUNT)
            config.streamSetting = (STREAM_MODE)0;
    }
    if(kDown & KEY_L)
    {
        lock_guard<mutex> guard(configMutex);
        config.useFrameSkip = !config.useFrameSkip;
    }
    else if(kDown & KEY_R)
    {
        lock_guard<mutex> guard(configMutex);
        config.streamRequested = true;
    }

    return true;
}

void RunInputThread(StreamConfigData& config, mutex& configMutex)
{
    while(appletMainLoop())
    {
        bool streamActive = false;
        {
            lock_guard<mutex> guard(configMutex);
            streamActive = config.streamOn;
        }

        if(!streamActive)
        {
            hidScanInput();

            //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
            u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
            if(!ProcessInactiveStreamInput(kDown, config, configMutex))
                break;
        }
    }
}

thread StartInputThread(StreamConfigData& config, mutex& configMutex)
{
    return thread(RunInputThread, ref(config), ref(configMutex));
}

void RunGamepadThread(std::string ip, uint16_t port)
{
    int padSocket;
    if(ConnectTo(ip, port, padSocket))
    {
        while(appletMainLoop())
        {
            sleep(1);
            hidScanInput();

            //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
            u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
            if(kDown & KEY_PLUS)
            {
                u32 exit = 0xFFFF;
                send(padSocket, (char*)&exit, sizeof(exit), 0);
                break;
            }
            auto result = send(padSocket, (char*)&kDown, sizeof(kDown), 0);
            if(result < 0)
            {
                cout << "Error sending pad data" << endl;
            }
        }
    }

    close(padSocket);
}

thread StartGamepadThread(std::string ip, uint16_t port)
{
    return thread(RunGamepadThread, ip, port);
}

#endif
