// ���� ���α׷��� �������� �����մϴ�.
//
#include "stdafx.h"
#include "AVOID.h"
#include "Framework.h"

#define MAX_LOADSTRING	100
#define CLIENT_WIDTH	1920
#define CLIENT_HEIGHT	1080

// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
CFramework myFramework;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				InitServer();
DWORD CALLBACK		ProcessClient(LPVOID arg);
void				Recv();
char				TranslatePacket(const packet& packetBuf);
void*				GetDataFromPacket(char* dataBuf, char packetType);
void				ApplyPacketData(char* dataBuf, char packetType);
void				Send(void* packetBuf);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_AVOID, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);	// Ŭ������ ����Ѵٴ� ���̴�. ������ Ŭ������ ���¸� �����ش�. 


	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AVOID));

	MSG msg;

	// �⺻ �޽��� �����Դϴ�.
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))	// GetMessage(&msg, nullptr, 0, 0)
														// PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)
														// ���� ������ ����������� ����������, ���� Ŭ���Ҷ����� �����´�.
														// ���� ��� ������ ������ �� �ٲ���� �Ѵ�.
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			if (msg.message == WM_QUIT) break; // esc ������ ������ ����.

			TranslateMessage(&msg);	// �޽��� �̵�
			DispatchMessage(&msg);	// �޽��� �İ�
		}
		// ���⼭ �������� ���� ���α׷��� ������ �Ѵ�.
		// FrameAdvance�� ���� ���α׷��� ������.
		myFramework.FrameAdvance();


	}

	return (int)msg.wParam;
}

//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;	// WND CLASS EX W == '������ Ŭ���� Ȯ�� �����ڵ�' ��� ���� ����ü�̴�.
						// { style , WndProc, hInstance, ClassName, return RegisterClassW }
						// style�� msdn ���� ���� �˾Ƽ� ������ �Ѵ�.
						// WndProc�� �ݹ� �Լ��̴�. Ȱ��ȭ�� Input���� Getmsg�� �޾Ƽ� �ҷ��´�.
						// hInstance�� main �ܼ��Լ������� �ִ�. GetModuleHandle(NULL)�Լ��ε� ������ �� �ִ�.
						// hInstance�� ���μ����� ���̵��̴�. ���̵� �����ͼ� ���α׷��� ���� �ĺ���ȣ�� ����Ѵ�.
						// ClassName�� �� �״�� Ŭ������ �̸��� �����Ѵ�.

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style =
		CS_HREDRAW		// Ŭ���̾�Ʈ�� �ʺ� �����ϸ�, ��ü �����츦 �ٽ� �׸��� �Ѵ�.	WM{_SIZE�� ���� ����.
		| CS_VREDRAW	// Ŭ���̾�Ʈ�� ���̸� �����ϸ�, ��ü �����츦 �ٽ� �׸��� �Ѵ�.
	//  | CS_DBLCLKS	// �ش� �����쿡�� ����Ŭ���� ����ؾ� �Ѵٸ� �߰��ؾ� �Ѵ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// ���� ������ �ڵ�
	HWND hWnd;

	// ������ ��Ÿ��
	DWORD dwStyle =
		WS_OVERLAPPED			// ����Ʈ ������. Ÿ��Ʋ �ٿ� ũ�� ������ �ȵǴ� ��輱�� ������. �ƹ��� ��Ÿ�ϵ� ���� ������ �� ��Ÿ���� ����ȴ�.
		| WS_CAPTION			// Ÿ��Ʋ �ٸ� ���� �����츦 ����� WS_BORDER ��Ÿ���� �����Ѵ�.
		| WS_SYSMENU			// �ý��� �޴��� ���� �����츦 �����.
		| WS_MINIMIZEBOX		// �ּ�ȭ ��ư�� �����.
		| WS_BORDER				// �ܼ����� �� ��輱�� ����� ũ�� ������ �� �� ����.
  //	  | WS_THICKFRAME		// ũ�� ������ ������ �β��� ��輱�� ������. WS_BORDER�� ���� ����� �� ����.
		;						// �߰��� �ʿ��� ������ ��Ÿ���� http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm ����

	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	// Ŭ���̾�Ʈ ������
	RECT rc;
	rc.left = rc.top = 0;
	//rc.right = CLIENT_WIDTH;
	//rc.bottom = CLIENT_HEIGHT;
	rc.right = GetSystemMetrics(SM_CXSCREEN);											// ����� ���� ��� ȭ�� ����
	rc.bottom = GetSystemMetrics(SM_CYSCREEN);
	// ������ ����� ������ �߰��Ǵ� (ĸ��, �ܰ��� ��) ũ�⸦ ����.
	AdjustWindowRect(&rc, dwStyle, FALSE);

	// Ŭ���̾�Ʈ ���� ��ǥ(left, top)
	// ����ũ���� �߾ӿ� Ŭ���̾�Ʈ�� ��ġ�ϵ��� ����
	POINT ptClientWorld;
	//ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	//ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;
	ptClientWorld.x = (getWinSize.right - GetSystemMetrics(SM_CXSCREEN)) / 2;			// ����� ���� ��� ȭ�� ����
	ptClientWorld.y = (getWinSize.bottom - GetSystemMetrics(SM_CYSCREEN)) / 2;

	/*
	 HWND hWnd = CreateWindowW(
		szWindowClass			// ������ Ŭ���� ��
		, szTitle				// ĸ�� ǥ�� ���ڿ�
		, WS_OVERLAPPEDWINDOW	// ������ ��Ÿ��
		, CW_USEDEFAULT			// �θ� ������ ��� ������ ������ǥ : x
		, 0						// �θ� ������ ��� ������ ������ǥ : y
		, CW_USEDEFAULT			// ������ ������ : width
		, 0						// ������ ������ : height
		, nullptr				// �θ� ������.
		, nullptr				// �޴� �ڵ�
		, hInstance				// �ν��Ͻ� �ڵ�
		, nullptr);				// �߰� �Ķ��Ÿ : NULL
	*/

	hWnd = CreateWindowW(
		szWindowClass			// ������ Ŭ���� ��
		, szTitle				// ĸ�� ǥ�� ���ڿ�
		, dwStyle				// ������ ��Ÿ��
		, ptClientWorld.x		// �θ� ������ ��� ������ ������ǥ : x
		, ptClientWorld.y		// �θ� ������ ��� ������ ������ǥ : y
		, rc.right - rc.left		// ������ ������ : width
		, rc.bottom - rc.top		// ������ ������ : height
		, nullptr				// �θ� ������.
		, nullptr				// �޴� �ڵ�
		, hInstance				// �ν��Ͻ� �ڵ�
		, nullptr);				// �߰� �Ķ��Ÿ : NULL

	// CreateWindow �Լ���? 
	// ���� ������ �����찡 �ִµ�, �̴�� ����ڴ� ��� ���̴�. 
	// �������� Ŭ������ ������ �İ�, �� �Լ��� ù ���ڷ� ������ ã�´�.
	// 2��° ���ڴ� ĸ������, Ÿ��Ʋ �̸��̴�.
	// 3��° �÷��״� msdn ����
	// ȭ���� �»�� x
	// ȭ���� �»�� y
	// ���� ����� â�� ���� ũ��
	// ���� ����� â�� ���� ũ��
	// �� �ڷδ� null null hInstance(���μ��� �ּ�) null

	// ���� ���н� ���α׷� ����   
	if (!hWnd) return FALSE;
	if (!myFramework.OnCreate(hInstance, hWnd, rc)) return FALSE;
	// ������ ǥ��
	ShowWindow(hWnd, nCmdShow); // ������ â�� �����ش�. �̰� ������ ������ â�� �� ���̱� ������ Ȱ��ȭ�� �ȵǹǷ� �ƹ��͵� ���Ѵ�. ���� ������!
	// Ȯ�� : WnbdProc�� default msg handler�� DefWindowProc
	// �Լ��� ��ȯ�ϴ°�?
	UpdateWindow(hWnd);

	// ���� ��ȯ
	return TRUE;
}

void InitServer()
{
	int retval;
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		cout << "Init Fail." << endl;
		return;
	}

	//���� ����
	g_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (g_socket == INVALID_SOCKET) {
		std::cout << "Create Socket Fail." << std::endl;
		return;
	}

	//connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDR, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVER_PORT);
	retval = connect(g_socket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		std::cout << "Socket Error in connect" << std::endl;
		return;
	}

	HANDLE hThread = CreateThread(nullptr, 0, ProcessClient, (LPVOID)g_socket, 0, nullptr);
	if (!hThread) closesocket(g_socket);
}

DWORD CALLBACK ProcessClient(LPVOID arg)
{
	while (true) {
		Recv();
	}
	return 0;
}

void Recv()
{
	packet pk;
	int retval = recv(g_socket, reinterpret_cast<char*>(&pk), sizeof(packet), MSG_WAITALL);
	if (retval == SOCKET_ERROR) {
		std::cout << "Socket Error in send" << std::endl;
		return;
	}
	TranslatePacket(pk);
}

char TranslatePacket(const packet& packetBuf)
{
	switch (packetBuf.type)
	{
	case SC_PACKET_LOGIN_CONFIRM:
		myFramework.GetCurrScene();
		return SC_PACKET_LOGIN_CONFIRM;
		break;
	case SC_PACKET_START_GAME:
		myFramework.GetCurrScene();
		return SC_PACKET_START_GAME;
		break;
	case SC_PACKET_OBJECTS_INFO:
		myFramework.GetCurrScene();
		return SC_PACKET_OBJECTS_INFO;
		break;
	case SC_PACKET_MUSIC_END:
		myFramework.GetCurrScene();
		return SC_PACKET_MUSIC_END;
		break;
	case SC_PACKET_RANK:
		myFramework.GetCurrScene();
		return SC_PACKET_MUSIC_END;
		break;
	case CS_PACKET_LOGOUT:
		myFramework.GetCurrScene();
		return CS_PACKET_LOGOUT;
		break;
	default:
		return -1;
		break;
	}
}

void* GetDataFromPacket(char* dataBuf, char packetType)
{
	switch (packetType)
	{
	case SC_PACKET_LOGIN_CONFIRM:

		break;
	case SC_PACKET_START_GAME:

		break;
	case SC_PACKET_OBJECTS_INFO:

		break;
	case SC_PACKET_MUSIC_END:

		break;
	case SC_PACKET_RANK:

		break;
	case CS_PACKET_LOGOUT:

		break;
	default:
		return nullptr;
	}
}

void ApplyPacketData(char* dataBuf, char packetType)
{
	switch (packetType)
	{
	case SC_PACKET_LOGIN_CONFIRM:

		break;
	case SC_PACKET_START_GAME:

		break;
	case SC_PACKET_OBJECTS_INFO:

		break;
	case SC_PACKET_MUSIC_END:

		break;
	case SC_PACKET_RANK:

		break;
	case CS_PACKET_LOGOUT:

		break;
	default:
	}
	return;
}

void Send(void* packetBuf)
{
	int retval = send(g_socket, reinterpret_cast<char*>(packetBuf), reinterpret_cast<packet*>(packetBuf)->size, 0);
}