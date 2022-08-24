#include "ConfigHelper.h"
#include "srp/codec/general/GenericCodecConfiguration.h"
#include "srp/codec/h264/H264Configuration.h"
#include "srp/codec/h264_amf/H264AmfConfiguration.h"
#include "srp/decoder/DecoderConfiguration.h"
#include "srp/controller/ControllerConfiguration.h"
#include "srp/mouse/MouseConfiguration.h"
#include "srp/keyboard/KeyboardConfiguration.h"
#include "srp/touch/TouchConfiguration.h"
#include "srp/audio/AudioConfiguration.h"

void saveEncoderData(EncoderConfig const encoderData)
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

void saveDecoderData(DecoderData const decoderData)
{
    auto decoderConf = DecoderConfiguration{};
    decoderConf.Save(decoderData);
}

void saveControllerData(controller::ControllerConfig const controllerData)
{
    auto conf = ControllerConfiguration{};
    conf.Save(controllerData);
}

void saveMouseData(mouse::MouseConfig const mouseData)
{
    auto conf = MouseConfiguration{};
    conf.Save(mouseData);
}

void saveKeyboardData(keyboard::KeyboardConfig const keyboardData)
{
    auto conf = KeyboardConfiguration{};
    conf.Save(keyboardData);
}

void saveTouchData(touch::TouchConfig const touchData)
{
    auto conf = TouchConfiguration{};
    conf.Save(touchData);
}

void saveAudioData(audio::AudioConfig const audioData)
{
    auto conf = AudioConfiguration{};
    conf.Save(audioData);
}

void SaveConfigData(ConfigContainer const configurations)
{
    saveEncoderData(configurations.encoderConfig);
    saveDecoderData(configurations.decoderConfig);
    saveControllerData(configurations.controllerConfig);
    saveMouseData(configurations.mouseConfig);
    saveKeyboardData(configurations.keyboardConfig);
    saveTouchData(configurations.touchConfig);
    saveAudioData(configurations.audioConfig);
}