#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include <switch.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "VideoStream.h"
#include "CommandSender.h"

using namespace std;

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
            cout << "Sent command payload" << endl;
        else
            cout << "Error sending payload" << endl;
    }
    close(commandSocket);
}

thread CommandConnectionThreadStart(std::string ip, uint16_t port, STREAM_MODE setting)
{
    return thread(RunCommandThread, ip, port, setting);
}

void RunInputThread(StreamConfigData& config, mutex& configMutex)
{
    while(appletMainLoop())
    {
        sleep(1);
        hidScanInput();

        //hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
        u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) 
        {
            lock_guard<mutex> guard(configMutex);
            if(!config.streamOn)
                config.quitApp = true; // break in order to return to hbmenu
            config.streamOn = false;

            if(config.quitApp)
                break;
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
            config.streamOn = true;
        }
    }
}

thread StartInputThread(StreamConfigData& config, mutex& configMutex)
{
    return thread(RunInputThread, ref(config), ref(configMutex));
}

#endif
