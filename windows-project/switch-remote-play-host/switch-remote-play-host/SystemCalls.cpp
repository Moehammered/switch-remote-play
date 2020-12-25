#include <winsock2.h> // to get the windows definitions made so 'ChangeDisplaySettings' can be defined
#include "SystemCalls.h"

#include <iostream>

MONITORINFOEX DefaultMonitorInfo()
{
    auto monitorPoint = POINT{ 0 };
    monitorPoint.x = 0; monitorPoint.y = 0;
    auto handle = MonitorFromPoint(monitorPoint, MONITOR_DEFAULTTOPRIMARY);

    auto monitorInfo = MONITORINFOEX{};
    monitorInfo.cbSize = sizeof(monitorInfo);

    GetMonitorInfo(handle, &monitorInfo);

    return monitorInfo;
}

bool ChangeResolution(int width, int height)
{
    auto deviceMode = DEVMODE{ 0 };

    deviceMode.dmSize = sizeof(deviceMode);
    deviceMode.dmBitsPerPel = 32;
    deviceMode.dmPelsWidth = width;
    deviceMode.dmPelsHeight = height;
    deviceMode.dmDisplayFrequency = 60;
    deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;

    auto result = ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN);
    return result == DISP_CHANGE_SUCCESSFUL;
}

void StopStreamProcesses()
{
    if (streamProcessInfo.hProcess != nullptr)
    {
        TerminateProcess(streamProcessInfo.hProcess, 1);
        CloseHandle(streamProcessInfo.hProcess);
        CloseHandle(streamProcessInfo.hThread);
        ZeroMemory(&streamProcessInfo, sizeof(streamProcessInfo));
    }
    if (audioProcessInfo.hProcess != nullptr)
    {
        TerminateProcess(audioProcessInfo.hProcess, 1);
        CloseHandle(audioProcessInfo.hProcess);
        CloseHandle(audioProcessInfo.hThread);
        ZeroMemory(&audioProcessInfo, sizeof(audioProcessInfo));
    }
}

BOOL WINAPI ConsoleWindowEventHandler(DWORD eventType)
{
    switch (eventType)
    {
        // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
        std::cout << "Cleaning up in event handler...\n";
        std::cout << "Terminating video and audio process\n";
        StopStreamProcesses();
        if (switchHandshakeConnection != nullptr)
        {
            std::cout << "terminating handshake connection\n";
            switchHandshakeConnection->Shutdown();
            switchHandshakeConnection->Close();
        }
        if (switchBroadcastListener != nullptr)
        {
            std::cout << "terminating broadcast listener ";
            switchBroadcastListener->Close();
            switchBroadcastListener->Shutdown();
        }
        if (switchCommandListener != nullptr)
        {
            std::cout << "terminating command receiver connection\n";
            switchCommandListener->Close();
            switchCommandListener->Shutdown();
        }
        Sleep(2000);
        return TRUE;

    default:
        return FALSE;
    }
}

bool WinsockReady()
{
    WSADATA wsaStateData;
    auto socketStartup = WSAStartup(MAKEWORD(2, 2), &wsaStateData);

    switch (socketStartup)
    {
    case WSASYSNOTREADY:
        std::cout << "Winsock Error - WSASYSNOTREADY: The underlying network subsystem is not ready for network communication\n";
        return false;

    case WSAVERNOTSUPPORTED:
        std::cout << "Winsock Error - WSAVERNOTSUPPORTED: The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation\n";
        return false;

    case WSAEINPROGRESS:
        std::cout << "Winsock Error - WSAEINPROGRESS: A blocking Windows Sockets 1.1 operation is in progress\n";
        return false;

    case WSAEPROCLIM:
        std::cout << "Winsock Error - WSAEPROCLIM: A limit on the number of tasks supported by the Windows Sockets implementation has been reached\n";
        return false;

    case WSAEFAULT:
        std::cout << "Winsock Error - WSAEFAULT: The lpWSAData parameter is not a valid pointer\n";
        return false;
    }

    return true;
}
