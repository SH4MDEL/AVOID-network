#pragma once

// 서버 관련 헤더파일입니다.
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include "Protocol.h"

// C, C++ 관련 헤더파일입니다.
#include <iostream>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <mutex>
using namespace std;

constexpr float PI = 3.1415926535;
#define ENEMY_NUM 12
#define ENEMY_RADIUS 200


extern int			g_selectedMusic;
extern SOCKET		g_listenSock;

//#define SERVER_DEBUG