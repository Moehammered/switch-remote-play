#include "SystemSetup.h"
#include <iostream>
#include <switch.h>
#include <atomic>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

// this crashes other programs - assuming because the original stdout isn't restored
//#define LOG_STDOUT

int fileOutDescriptor {-1};
FILE* outFile {nullptr};

void redirectStdOut()
{
    #ifdef LOG_STDOUT
    auto path = "sdmc:/switch/switch-remote-play/log.txt";
    fflush(stdout);
    outFile = freopen(path, "w", stdout);
    #else
    auto libnxRes = nxlinkStdio();
    if(libnxRes <= 0)
        std::cout << "Failed to call nxlinkStdio with result: " << libnxRes << std::endl;
    #endif
}

void initialiseSwitch()
{
    SocketInitConfig socketConfiguration = {
        .bsdsockets_version = 1,

        .tcp_tx_buf_size = 0x80000,
        .tcp_rx_buf_size = 0x100000,
        .tcp_tx_buf_max_size = 0x300000,
        .tcp_rx_buf_max_size = 0x500000,

        .udp_tx_buf_size = 0x1400,
        .udp_rx_buf_size = 0x2500,

        .sb_efficiency = 3,
    };

    auto libnxRes = socketInitialize(&socketConfiguration);
    if(!R_SUCCEEDED(libnxRes))
        std::cout << "Failed to call socketInitialize with result: " << libnxRes << std::endl;
    
    redirectStdOut();

    libnxRes = plInitialize(PlServiceType_User); //required to access system resources (font data for example)
    if(!R_SUCCEEDED(libnxRes))
        std::cout << "Failed to call plInitialize(PlServiceType_User) with result: " << libnxRes << std::endl;
    // pcvInitialize();

    libnxRes = romfsInit();
    if(!R_SUCCEEDED(libnxRes))
        std::cout << "Failed to call romfsInit with result: " << libnxRes << std::endl;

    libnxRes = audoutInitialize();
    if(!R_SUCCEEDED(libnxRes))
        std::cout << "Failed to call audoutInitialize with result: " << libnxRes << std::endl;
    libnxRes = audoutStartAudioOut();
    if(!R_SUCCEEDED(libnxRes))
        std::cout << "Failed to call audoutStartAudioOut with result: " << libnxRes << std::endl;
}

FC_Font* LoadSystemFont(SDL_Renderer* renderer, Uint32 fontSize, SDL_Color defaultCol)
{
    auto systemFont = FC_CreateFont();
    if(!TTF_WasInit() && TTF_Init() < 0)
        std::cout << "Failed to call TTF_Init: " << TTF_GetError() << std::endl;
    //auto fontPath = "romfs:/fonts/PTSerif-Regular.ttf";
    //auto fontPath = "romfs:/fonts/RobotoMono-Regular.ttf";
    // auto fontPath = "romfs:/fonts/RobotoMono-Light.ttf";
    auto fontPath = "romfs:/fonts/RobotoCondensed-Regular.ttf";
    auto ttf = TTF_OpenFont(fontPath, fontSize);
    if(ttf == nullptr)
        std::cout << "Failed to open ttf font\n";
    auto result = FC_LoadFontFromTTF(systemFont, renderer, ttf, ttf, defaultCol);
    if(result == 0)
        std::cout << "Failed to load font from TTF\n";
    TTF_CloseFont(ttf);
    if(systemFont == nullptr)
        std::cout << "Failed to load system font\n";

    return systemFont;
}

void FreeFont(FC_Font* font)
{
    FC_ClearFont(font);
    FC_FreeFont(font);
}

void CleanupSystem()
{
    #ifdef LOG_STDOUT
    fflush(outFile);
    fclose(outFile);
    #endif
    audoutExit();
    plExit();
    // pcvExit();
    socketExit();
}

template <typename T>
bool IsAtomicTypeLockFree()
{
    return std::atomic<T>{}.is_lock_free();
}

void PrintOutAtomicLockInfo()
{
    auto boolToWord = [](bool value) 
    { 
        return value ? "yes" : "no"; 
    };
    std::cout << "Atomic Info Breakdown" << std::endl;
    std::cout << "bool lock free? " << boolToWord(IsAtomicTypeLockFree<bool>()) << std::endl << std::endl;

    std::cout << "int_8 lock free? " << boolToWord(IsAtomicTypeLockFree<int8_t>()) << std::endl;
    std::cout << "uint_8 lock free? " << boolToWord(IsAtomicTypeLockFree<uint8_t>()) << std::endl << std::endl;

    std::cout << "int_16 lock free? " << boolToWord(IsAtomicTypeLockFree<int16_t>()) << std::endl;
    std::cout << "uint_16 lock free? " << boolToWord(IsAtomicTypeLockFree<uint16_t>()) << std::endl << std::endl;

    std::cout << "int_32 lock free? " << boolToWord(IsAtomicTypeLockFree<int32_t>()) << std::endl;
    std::cout << "uint_32 lock free? " << boolToWord(IsAtomicTypeLockFree<uint32_t>()) << std::endl << std::endl;

    std::cout << "int_64 lock free? " << boolToWord(IsAtomicTypeLockFree<int64_t>()) << std::endl;
    std::cout << "uint_64 lock free? " << boolToWord(IsAtomicTypeLockFree<uint64_t>()) << std::endl << std::endl;

    std::cout << "float lock free? " << boolToWord(IsAtomicTypeLockFree<float>()) << std::endl;
    std::cout << "double lock free? " << boolToWord(IsAtomicTypeLockFree<double>()) << std::endl << std::endl;

    std::cout << "uintptr_t lock free? " << boolToWord(IsAtomicTypeLockFree<uintptr_t>()) << std::endl;
    std::cout << "int* lock free? " << boolToWord(IsAtomicTypeLockFree<int*>()) << std::endl << std::endl;

    std::cout << "SDL_Color lock free? " << boolToWord(IsAtomicTypeLockFree<SDL_Color>()) << std::endl << std::endl;
}

