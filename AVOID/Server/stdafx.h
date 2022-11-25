#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <cmath>

constexpr float pi = 3.14159265358979;
constexpr int NUMBER_OF_ENEMY = 12;

HANDLE hClientEvent;
HANDLE hCollideEvent;

struct Coord
{
	short x, y;
};