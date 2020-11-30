#include "SystemSetup.h"
#include <iostream>
#include <switch.h>
#include <atomic>

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

    socketInitialize(&socketConfiguration);
    nxlinkStdio();
    plInitialize(PlServiceType_User); //required to access system resources (font data for example)
    // pcvInitialize();

    audoutInitialize();
    audoutStartAudioOut();
}

FC_Font* LoadSystemFont(SDL_Renderer* renderer, Uint32 fontSize, SDL_Color defaultCol)
{
    PlFontData switchFont, nintendoFont;
    plGetSharedFontByType(&switchFont, PlSharedFontType_Standard);
    plGetSharedFontByType(&nintendoFont, PlSharedFontType_NintendoExt);
    
    auto systemFont = FC_CreateFont();
    auto switchFontStream = SDL_RWFromMem((void *)switchFont.address, switchFont.size);
    auto nintFontStream = SDL_RWFromMem((void *)nintendoFont.address, nintendoFont.size);
    FC_LoadFont_RW(systemFont, renderer, switchFontStream, nintFontStream, 
                    1, fontSize, defaultCol, TTF_STYLE_NORMAL);

    return systemFont;
}

void FreeFont(FC_Font* font)
{
    FC_ClearFont(font);
    FC_FreeFont(font);
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

