#include "stdafx.h"
#include "Protocol.h"
#include "PlayerData.h"

char TranslatePacket(char* packetBuf);
void GetDataFromPacket(SOCKET socket, char* dataBuf, char packetType);
void ApplyPacketData(SOCKET socket, char* dataBuf, char packetType);
void MakePacket(SOCKET sock);
void initServer();