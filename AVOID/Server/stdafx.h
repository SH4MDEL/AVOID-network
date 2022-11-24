#pragma once

#include <windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <cmath>

constexpr float pi = 3.14159265358979;
constexpr int NUMBER_OF_ENEMY = 12;


struct Coord
{
	short x, y;
};