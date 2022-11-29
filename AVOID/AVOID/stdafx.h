// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define USE_NETWORK

#ifdef USE_NETWORK
#define NETWORK_DEBUG
#endif

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include <atlimage.h>					// png 이미지 파일을 사용하기 위함

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>						// 삼각함수를 사용하기 위함

// C++ 런타임 헤더 파일입니다.
#include <iostream>
#include <chrono>
#include <string>
#include <list>
#include <array>
#include <memory.h>
using namespace std;

#include "Protocol.h"
#ifdef USE_NETWORK
// 서버 관련 헤더 파일입니다.
#include <winsock2.h> // 윈속2 메인 헤더
#include <ws2tcpip.h> // 윈속2 확장 헤더
#pragma comment(lib, "ws2_32") // ws2_32.lib 링크
#endif

// 타이머를 쓰기 위함
#pragma comment(lib, "winmm.lib")
#include <Mmsystem.h>

// 음악 추가하기 위함
#include "inc/fmod.hpp"
#pragma comment (lib,"fmod64_vc.lib")

extern SOCKET		g_socket;

#define TITLE_MX_LENGTH 64

#define TITLESTRING TEXT("AVOID network ")	// 타이틀 이름이 들어갈 문자열이므로 내용을 원하는 대로 바꿔주자.

// 캡션 FPS 출력 여부
// 항상 캡션에 FPS를 출력	(0: 비활성 | 1: 활성)
#define USE_CAPTIONFPS_ALWAYS	1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS
#elif _DEBUG	// debug에서는 항상 실행
#define SHOW_CAPTIONFPS
#endif


#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// 최대 FPS
#if _DEBUG
#define MAX_FPS 1.0 / 30.0
#else
// #define MAX_FPS 1.0 / 60.0
#define MAX_FPS 0.0
#endif

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#define DIR_FORWARD		0x01
#define DIR_BACKWARD	0x02
#define DIR_LEFT		0x04
#define DIR_RIGHT		0x08
#define DIR_UP			0x10
#define DIR_DOWN		0x20


#define USE_CONSOLE_WINDOW
#ifdef USE_CONSOLE_WINDOW
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif
#endif