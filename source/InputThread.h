#ifndef __INPUTTHREAD_H__
#define __INPUTTHREAD_H__

#include <switch.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "VideoStream.h"

using namespace std;

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
            if(--config.streamTechnique < 0)
                config.streamTechnique = 2;
        }
        else if(kDown & KEY_DDOWN)
        {
            lock_guard<mutex> guard(configMutex);
            if(++config.streamTechnique >= 3)
                config.streamTechnique = 0;
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
