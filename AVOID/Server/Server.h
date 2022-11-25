#pragma once

#include "stdafx.h"
#include "Protocol.h"
#include "PlayerData.h"

DWORD WINAPI Collision_Thread(LPVOID arg);
char TranslatePacket(char* packetBuf);
char* GetDataFromPacket(SOCKET socket, char* dataBuf, char packetType);
void ApplyPacketData(SOCKET socket, char* dataBuf, char packetType);
void MakePacket(SOCKET sock);
void initServer();

HANDLE hClientEvent;
HANDLE hCollideEvent;

DWORD WINAPI Client_Thread(LPVOID arg);
void CollisionCheckPlayerAndBullet();
void CollisionCheckBulletAndWall();
void CollisionCheckAbility();
