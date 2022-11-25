#include "stdafx.h"
#include "Protocol.h"
#include "PlayerData.h"

DWORD WINAPI Collision_Thread(LPVOID arg);
char TranslatePacket(char* packetBuf);
char* GetDataFromPacket(SOCKET socket, char* dataBuf, char packetType);
void ApplyPacketData(SOCKET socket, char* dataBuf, char packetType);
void MakePacket(SOCKET sock);
void initServer();


DWORD WINAPI Client_Thread(LPVOID arg);
void CollisionCheckPlayerAndBullet();
void CollisionCheckPlayerAndWall();
void CollisionCheckAbility();
