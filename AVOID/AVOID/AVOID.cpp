// 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"
#include "AVOID.h"
#include "Framework.h"

#include "Scene_Main.h"
#include "Scene_MusicSelect.h"
#include "Scene_PlayerWaiting.h"
#include "Scene_Ingame.h"
#include "Scene_Result.h"

#define MAX_LOADSTRING	100
#define CLIENT_WIDTH	1920
#define CLIENT_HEIGHT	1080

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
CFramework myFramework;
HANDLE g_event;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

#ifdef USE_NEWORK
void				InitServer();
DWORD CALLBACK		ProcessClient(LPVOID arg);
void				Recv();
void				TranslatePacket(const packet& packetBuf);
void				Send(void* packetBuf);
#endif

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_AVOID, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);	// 클래스를 등록한다는 뜻이다. 윈도우 클래스의 형태를 정해준다. 


	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AVOID));

	MSG msg;

	// 기본 메시지 루프입니다.
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))	// GetMessage(&msg, nullptr, 0, 0)
														// PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)
														// 픽은 루프가 비어있을때도 가져오지만, 겟은 클릭할때마다 가져온다.
														// 따라서 모든 게임은 픽으로 다 바꿔줘야 한다.
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			if (msg.message == WM_QUIT) break; // esc 누르면 윈도우 종료.

			TranslateMessage(&msg);	// 메시지 이동
			DispatchMessage(&msg);	// 메시지 파견
		}
		// 여기서 프레임을 통해 프로그램을 돌려야 한다.
		// FrameAdvance를 통해 프로그램을 돌린다.
		myFramework.FrameAdvance();
	}

	return (int)msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;	// WND CLASS EX W == '윈도우 클래스 확장 유니코드' 라는 뜻의 구조체이다.
						// { style , WndProc, hInstance, ClassName, return RegisterClassW }
						// style은 msdn 가서 직접 알아서 보도록 한다.
						// WndProc는 콜백 함수이다. 활성화된 Input으로 Getmsg를 받아서 불러온다.
						// hInstance는 main 콘솔함수에서도 있다. GetModuleHandle(NULL)함수로도 가져올 수 있다.
						// hInstance는 프로세스의 아이디이다. 아이디를 가져와서 프로그램의 고유 식별번호로 사용한다.
						// ClassName은 말 그대로 클래스의 이름을 정의한다.

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style =
		CS_HREDRAW		// 클라이언트의 너비를 변경하면, 전체 윈도우를 다시 그리게 한다.	WM{_SIZE를 통해 조작.
		| CS_VREDRAW	// 클라이언트의 높이를 변경하면, 전체 윈도우를 다시 그리게 한다.
	//  | CS_DBLCLKS	// 해당 윈도우에서 더블클릭을 사용해야 한다면 추가해야 한다.
		;
	wcex.lpfnWndProc = CFramework::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AVOID));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL; // MAKEINTRESOURCEW(IDC_CSTUDYWINDOW);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// 메인 윈도우 핸들
	HWND hWnd;

	// 윈도우 스타일
	DWORD dwStyle =
		WS_OVERLAPPED			// 디폴트 윈도우. 타이틀 바와 크기 조절이 안되는 경계선을 가진다. 아무런 스타일도 주지 않으면 이 스타일이 적용된다.
		| WS_CAPTION			// 타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함한다.
		| WS_SYSMENU			// 시스템 메뉴를 가진 윈도우를 만든다.
		| WS_MINIMIZEBOX		// 최소화 버튼을 만든다.
		| WS_BORDER				// 단선으로 된 경계선을 만들며 크기 조정은 할 수 없다.
  //	  | WS_THICKFRAME		// 크기 조정이 가능한 두꺼운 경계선을 가진다. WS_BORDER와 같이 사용할 수 없다.
		;						// 추가로 필요한 윈도우 스타일은 http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm 참고

	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	// 클라이언트 사이즈
	RECT rc;
	rc.left = rc.top = 0;
	//rc.right = CLIENT_WIDTH;
	//rc.bottom = CLIENT_HEIGHT;
	rc.right = GetSystemMetrics(SM_CXSCREEN);											// 모니터 길이 기반 화면 설정
	rc.bottom = GetSystemMetrics(SM_CYSCREEN);
	// 윈도우 사이즈에 실제로 추가되는 (캡션, 외곽선 등) 크기를 보정.
	AdjustWindowRect(&rc, dwStyle, FALSE);

	// 클라이언트 절대 좌표(left, top)
	// 데스크톱의 중앙에 클라이언트가 위치하도록 설정
	POINT ptClientWorld;
	//ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	//ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;
	ptClientWorld.x = (getWinSize.right - GetSystemMetrics(SM_CXSCREEN)) / 2;			// 모니터 길이 기반 화면 설정
	ptClientWorld.y = (getWinSize.bottom - GetSystemMetrics(SM_CYSCREEN)) / 2;

	/*
	 HWND hWnd = CreateWindowW(
		szWindowClass			// 윈도우 클래스 명
		, szTitle				// 캡션 표시 문자열
		, WS_OVERLAPPEDWINDOW	// 윈도우 스타일
		, CW_USEDEFAULT			// 부모 윈도우 기반 윈도우 시작좌표 : x
		, 0						// 부모 윈도우 기반 윈도우 시작좌표 : y
		, CW_USEDEFAULT			// 윈도우 사이즈 : width
		, 0						// 윈도우 사이즈 : height
		, nullptr				// 부모 윈도우.
		, nullptr				// 메뉴 핸들
		, hInstance				// 인스턴스 핸들
		, nullptr);				// 추가 파라메타 : NULL
	*/

	hWnd = CreateWindowW(
		szWindowClass			// 윈도우 클래스 명
		, szTitle				// 캡션 표시 문자열
		, dwStyle				// 윈도우 스타일
		, ptClientWorld.x		// 부모 윈도우 기반 윈도우 시작좌표 : x
		, ptClientWorld.y		// 부모 윈도우 기반 윈도우 시작좌표 : y
		, rc.right - rc.left		// 윈도우 사이즈 : width
		, rc.bottom - rc.top		// 윈도우 사이즈 : height
		, nullptr				// 부모 윈도우.
		, nullptr				// 메뉴 핸들
		, hInstance				// 인스턴스 핸들
		, nullptr);				// 추가 파라메타 : NULL

	// CreateWindow 함수란? 
	// 내가 정의한 윈도우가 있는데, 이대로 만들겠다 라는 뜻이다. 
	// 레지스터 클래스로 도장을 파고, 이 함수의 첫 인자로 도장을 찾는다.
	// 2번째 인자는 캡션으로, 타이틀 이름이다.
	// 3번째 플래그는 msdn 참조
	// 화면의 좌상단 x
	// 화면의 좌상단 y
	// 실제 만드는 창의 가로 크기
	// 실제 만드는 창의 세로 크기
	// 그 뒤로는 null null hInstance(프로세스 주소) null

	// 생성 실패시 프로그램 종료   
	if (!hWnd) return FALSE;
	if (!myFramework.OnCreate(hInstance, hWnd, rc)) return FALSE;
	// 윈도우 표시
	ShowWindow(hWnd, nCmdShow); // 윈도우 창을 보여준다. 이게 없으면 윈도우 창이 안 보이기 떄문에 활성화가 안되므로 아무것도 못한다. 종료 조차도!
	// 확인 : WnbdProc의 default msg handler가 DefWindowProc
	// 함수를 반환하는가?
	UpdateWindow(hWnd);

	// 성공 반환
	return TRUE;
}

#ifdef USE_NETWORK
void InitServer()
{
	ifstream is;
	is.open("ip.txt");

	int retval;
	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "Init Fail." << endl;
		return;
	}

	g_event = CreateEvent(NULL, TRUE, FALSE, NULL);

	//소켓 생성
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket == INVALID_SOCKET) {
		std::cout << "Create Socket Fail." << std::endl;
		return;
	}

	//connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	if (is.fail()) inet_pton(AF_INET, SERVER_ADDR, &serveraddr.sin_addr);
	else {
		char osip[20];
		is >> osip;
		inet_pton(AF_INET, osip, &serveraddr.sin_addr);
	}
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = connect(g_socket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		std::cout << "Socket Error in connect" << std::endl;
		return;
	}

	g_threadRun = true;
	HANDLE hThread = CreateThread(nullptr, 0, ProcessClient, (LPVOID)g_socket, 0, nullptr);
	if (!hThread) {
		CloseHandle(g_event);
		closesocket(g_socket);
	}
}


DWORD CALLBACK ProcessClient(LPVOID arg)
{
	while (g_threadRun) {
		Recv();
	}
	return 0;
}

void Recv()
{
	packet pk;
	int retval = recv(g_socket, reinterpret_cast<char*>(&pk), sizeof(packet), MSG_WAITALL);
#ifdef NETWORK_DEBUG
	if (retval == SOCKET_ERROR) {
		std::cout << "Socket Error in send" << std::endl;
		return;
	}
#endif // NETWORK_DEBUG
	TranslatePacket(pk);
}

void TranslatePacket(const packet& packetBuf)
{
	int retval, remain;
	switch (packetBuf.type)
	{
	case SC_PACKET_LOGIN_CONFIRM:
	{
		sc_packet_login_confirm pk;
		retval = recv(g_socket, reinterpret_cast<char*>(&pk) + 2, packetBuf.size - 2, MSG_WAITALL);
		Scene_PlayerWaiting* scene = (Scene_PlayerWaiting*)myFramework.GetCurrScene();
		scene->SetPlayerID(pk.playerID);
#ifdef NETWORK_DEBUG
		cout << "SC_PACKET_LOGIN_CONFIRM 해석" << endl;
#endif // NETWORK_DEBUG
		break;
	}
	case SC_PACKET_START_GAME:
	{
		sc_packet_start_game pk;
		retval = recv(g_socket, reinterpret_cast<char*>(&pk) + 2, packetBuf.size - 2, MSG_WAITALL);
		Scene_PlayerWaiting* scene = (Scene_PlayerWaiting*)myFramework.GetCurrScene();
		scene->ChangeGameStart(pk.playerNum);
#ifdef NETWORK_DEBUG
		cout << "SC_PACKET_START_GAME 해석" << endl;
#endif // NETWORK_DEBUG
		break;
	}
	case SC_PACKET_OBJECTS_INFO:
	{
		sc_packet_objects_info pk;
		remain = packetBuf.size - 2;
		while (remain > 0) {
			retval = recv(g_socket, reinterpret_cast<char*>(&pk) + packetBuf.size - remain, remain, MSG_WAITALL);
			remain -= retval;
		}
		Scene_Ingame* scene = (Scene_Ingame*)myFramework.GetCurrScene();
		scene->SetPlayerEnemyData(pk.playerNum, pk.enemyNum, pk.bulletNum);
		remain = pk.playerNum * sizeof(PlayerStatus);
		while (remain > 0) {
			retval = recv(g_socket, reinterpret_cast<char*>(&scene->GetPlayersCoord()) + pk.playerNum * sizeof(PlayerStatus) - remain,
				remain, MSG_WAITALL);
			remain -= retval;
		}
		remain = pk.enemyNum * sizeof(Coord);
		while (remain > 0) {
			retval = recv(g_socket, reinterpret_cast<char*>(&scene->GetEnemysCoord()) + pk.enemyNum * sizeof(Coord) - remain,
				remain, MSG_WAITALL);
			remain -= retval;
		}
		remain = pk.bulletNum * sizeof(Coord);
		while (remain > 0) {
			retval = recv(g_socket, reinterpret_cast<char*>(&scene->GetBulletsCoord()) + pk.bulletNum * sizeof(Coord) - remain,
				remain, MSG_WAITALL);
			remain -= retval;
		}
		SetEvent(g_event);
#ifdef NETWORK_DEBUG
		cout << "SC_PACKET_OBJECTS_INFO 해석" << endl;
#endif // NETWORK_DEBUG
		break;
	}
	case SC_PACKET_MUSIC_END:
	{
		Scene_Ingame* scene = (Scene_Ingame*)myFramework.GetCurrScene();
		scene->SetGameEnd();
		SetEvent(g_event);
#ifdef NETWORK_DEBUG
		cout << "SC_PACKET_MUSIC_END 해석" << endl;
#endif // NETWORK_DEBUG
		break;
	}
	case SC_PACKET_RANK:
	{
		sc_packet_rank pk;
		retval = recv(g_socket, reinterpret_cast<char*>(&pk) + 2, packetBuf.size - 2, MSG_WAITALL);
		Scene_Ingame* scene = (Scene_Ingame*)myFramework.GetCurrScene();
		scene->SetRank(pk.rank);
#ifdef NETWORK_DEBUG
		cout << "SC_PACKET_RANK 해석" << endl;
#endif // NETWORK_DEBUG
		break;
	}
	case CS_PACKET_LOGOUT:
	{
		myFramework.GetCurrScene();
		break;
	}
	default:
		break;
	}
}

void Send(void* packetBuf)
{
	int retval, remain;
	remain = reinterpret_cast<packet*>(packetBuf)->size;
	while (remain > 0) {
		retval = send(g_socket, reinterpret_cast<char*>(packetBuf) + reinterpret_cast<packet*>(packetBuf)->size - remain,
			remain, 0);
		remain -= retval;
	}
}
#endif // USE_NETWORK