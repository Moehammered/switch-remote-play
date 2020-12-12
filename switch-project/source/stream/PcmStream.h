#ifndef __PCMSTREAM_H__
#define __PCMSTREAM_H__

#include <thread>

class PcmStream
{
    public:
        PcmStream(uint16_t port);

        bool Ready() const;

        bool Running() const;

        void Start();

        void Shutdown();

    private:
        int32_t audioSocket;
        uint16_t audioPort;
        std::thread audioThread;

        int CreateSocket(uint16_t port);
};

#endif
