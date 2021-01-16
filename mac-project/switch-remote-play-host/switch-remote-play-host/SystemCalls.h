#pragma once
#include "Broadcast.h"
#include "Connection.h"


extern Broadcast* switchBroadcastListener;
extern Connection* switchHandshakeConnection;
extern Connection* switchCommandListener;

#ifdef __APPLE__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "DisplayData.h"

pid_t StartProcess(std::string path);

#endif


bool ChangeResolution(int width, int height);

void StopStreamProcesses();

#ifdef _WIN32
#include <WinUser.h>
extern PROCESS_INFORMATION streamProcessInfo;
extern PROCESS_INFORMATION audioProcessInfo;
MONITORINFOEX DefaultMonitorInfo();

BOOL WINAPI ConsoleWindowEventHandler(DWORD eventType);

bool WinsockReady();
#endif
