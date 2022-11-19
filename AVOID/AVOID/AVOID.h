#pragma once

#include "resource.h"

extern void				InitServer();
extern DWORD CALLBACK	ProcessClient(LPVOID arg);
extern void				Recv();
extern void				TranslatePacket(const packet& packetBuf);
extern void				Send(void* packetBuf);