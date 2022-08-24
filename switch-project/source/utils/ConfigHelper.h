#ifndef __CONFIGHELPER_H__
#define __CONFIGHELPER_H__

#include "../network/NetworkOptions.h"
#include "srp/decoder/DecoderOptions.h"
#include "srp/network/NetworkData.h"

struct ConfigContainer
{
    EncoderConfig encoderConfig;
    DecoderData decoderConfig;
    controller::ControllerConfig controllerConfig;
    mouse::MouseConfig mouseConfig;
    keyboard::KeyboardConfig keyboardConfig;
    touch::TouchConfig touchConfig;
    network::NetworkConfig networkConfig;
    audio::AudioConfig audioConfig;
};

void SaveConfigData(ConfigContainer const configurations);

#endif
