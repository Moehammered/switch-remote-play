#pragma once
#include <WinUser.h>
#include "Broadcast.h"
#include "Connection.h"

extern PROCESS_INFORMATION streamProcessInfo;
extern PROCESS_INFORMATION audioProcessInfo;
extern Broadcast* switchBroadcastListener;
extern Connection* switchHandshakeConnection;
extern Connection* switchCommandListener;

MONITORINFOEX DefaultMonitorInfo();

bool ChangeResolution(int width, int height);

void StopStreamProcesses();

BOOL WINAPI ConsoleWindowEventHandler(DWORD eventType);

bool WinsockReady();
