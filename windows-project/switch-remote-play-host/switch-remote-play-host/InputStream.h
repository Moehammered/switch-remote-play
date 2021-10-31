#ifndef __INPUTSTREAM_H__
#define __INPUTSTREAM_H__

#include "MouseStream.h"
#include "KeyboardStream.h"
#include "TouchStream.h"
#include "ControllerStream.h"
#include <stdint.h>
#include <atomic>

class InputStream
{
public:
    InputStream(TouchStream& touchStream,
            MouseStream& mouseStream,
            KeyboardStream& keyboardStream,
            ControllerStream& controllerStream,
            mouse::MouseConfig const mouseConfig,
            int64_t mouseFrameNanoTime,
            int64_t keyboardFrameNanoTime);

    void Run(uint64_t const socket, std::atomic_bool & gamepadActive, int32_t const maxRetries);

private:
    TouchStream& touchStream;
    MouseStream& mouseStream;
    KeyboardStream& keyboardStream;
    ControllerStream& controllerStream;
    bool mouseMode;
    int64_t const mouseFrameNanoTime;
    int64_t const keyboardFrameNanoTime;
    int64_t const mouseToggleNanoTime;
    uint32_t const mouseToggleBtnCombo;

    int32_t ReadStream(uint64_t const socket, char* const buffer, size_t const size);
};

#endif
