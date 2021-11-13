#ifndef __STREAMHELPER_H__
#define __STREAMHELPER_H__

#include "../ScreenRenderer.h"
#include "../MainScreen.h"
#include "../stream/StreamState.h"
#include "../stream/LiveStream.h"
#include "../network/NetworkDiscovery.h"
#include <thread>
extern "C"
{
    #include <switch/runtime/pad.h>
}

StreamState const renderMenus(ScreenRenderer& screen,
    PadState& mainPad,
    MenuSelection& menuScreens,
    NetworkDiscovery& network,
    FC_Font* systemFont);

StreamState const processStreamRequest(ScreenRenderer& screen,
    MenuSelection& menuScreens,
    LiveStream& liveStream,
    NetworkDiscovery& network,
    network::NetworkConfig const startupNetworkSettings,
    FC_Font* systemFont,
    std::thread& gamepadThread);

void renderStreamFrame(ScreenRenderer& screen, YUVFrame frameData);

#endif
