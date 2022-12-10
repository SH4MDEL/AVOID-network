#include "stdafx.h"
#include "Timer.h"
#include "SharedData.h"

extern Timer		g_timer;
extern HANDLE		g_event;

DWORD WINAPI MainThread(LPVOID arg);
DWORD WINAPI ClientThread(LPVOID arg);
int Recv(SOCKET socket);
void TranslatePacket(SOCKET socket, const packet& packetBuf);
void Send(SOCKET socket, void* packetBuf);

void CollisionCheckBulletAndWall();
void CollisionCheckPlayerAndBullet();
void CollisionCheckAbility();