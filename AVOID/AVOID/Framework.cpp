#include "stdafx.h"
// 
// Ÿ��Ʋ �� ���� include�� ����
#include "Scene_Main.h"
#include "Scene_MusicSelect.h"
#include "Scene_Music1.h"
#include "Scene_Music2.h"
#include "Scene_Result.h"
// 
#include "Framework.h"

using namespace std;// �����ϸ� ���� ����. ������ �ð��� ��û ��������. ���ɻ��� ������ ����.
					// ������ �� �� std ���ӽ����̽��� ���Ե� ��� ��������� ���� ������ ����� �����̴�.

int mouse_mx;
int mouse_my;
int finalhp;

template<typename T>
T GetUserDataPtr(HWND hWnd)
{
	return reinterpret_cast<T>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}
void SetUserDataPtr(HWND hWnd, LPVOID ptr)
{
	LONG_PTR result = ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ptr));
}

CFramework::CFramework()
{
}

CFramework::~CFramework()
{
	OnDestroy();
}

HDC* CFramework::GetPlayerDC()
{
	return &m_PlayerMemDC;
}

bool CFramework::OnCreate(HINSTANCE hInstance, HWND hWnd, const RECT & rc)
{
	srand((unsigned int)time(NULL));
	m_hInstance = hInstance;
	m_hWnd = hWnd;
	m_rcClient = rc;
	// Ŭ���̾�Ʈ ��ǥ �ʱ�ȭ
	m_rcClient.right -= m_rcClient.left;
	m_rcClient.bottom -= m_rcClient.top;
	m_rcClient.left = 0;
	m_rcClient.top = 0;

	// ���� ����
	CreatebackBuffer();

	// Ŭ������ ������ ���ν��� ����
	::SetUserDataPtr(m_hWnd, this);

	// ...

	// ĸ�� ����
	lstrcpy(m_CaptionTitle, TITLESTRING);

#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT("("));
#endif
	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);

	// Ÿ�̸� �ʱ�ȭ
//	m_LastUpdate_time = chrono::system_clock::now(); // �������� �ʾҴ�.
	m_current_time = chrono::system_clock::now();
	m_fps = 0;

	// �� ����
	BuildScene();
	// ������ ���� �����ΰ�?
	ChangeScene(CScene::SceneTag::Main);

	return (m_hWnd != NULL);
}

void CFramework::CreatebackBuffer()
{
	if (m_hDC)
	{
		::SelectObject(m_hDC, NULL);
		::DeleteDC(m_hDC);
	}
	if (m_hBitmapBackBuffer) ::DeleteObject(m_hBitmapBackBuffer);

	HDC hdc = ::GetDC(m_hWnd);
	m_hDC = ::CreateCompatibleDC(hdc);	// ȣȯ�� �����ִ� �Լ�
	// ������ ��ũ ���ο� ����۸� �����. hdc�� ȣȯ�� ��Ʈ���� ����Ѵ�.
	m_hBitmapBackBuffer = ::CreateCompatibleBitmap(hdc, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDC, m_hBitmapBackBuffer);

	SetBKColor(RGB(255, 255, 255));

	ClearBackgroundColor();

	::ReleaseDC(m_hWnd, hdc);
}

void CFramework::BuildScene()
{
	// arrScene[SceneTag::Title] = new TitleScene();	// �̷� ������� ���� ������.
	arrScene[CScene::SceneTag::Main] = new CMainScene(CScene::SceneTag::Main, this);
	arrScene[CScene::SceneTag::MusicSelect] = new Scene_MusicSelect(CScene::SceneTag::MusicSelect, this);
	arrScene[CScene::SceneTag::Music1] = new Scene_Music1(CScene::SceneTag::Music1, this);
	arrScene[CScene::SceneTag::Music2] = new Scene_Music2(CScene::SceneTag::Music2, this);
	arrScene[CScene::SceneTag::Result] = new Scene_Result(CScene::SceneTag::Result, this);
	arrScene[CScene::SceneTag::Main]->OnCreate();
}

void CFramework::BuildPlayer()
{
}

void CFramework::ReleaseScene()
{
}

bool CFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		return true;
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);	// ESC Ű�� ������ �� �����츦 �����Ѵ�.
			break;
		}
		return true;
	}
	return false;
}

bool CFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONUP:
		break;
	case WM_LBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		mouse_mx = LOWORD(lParam);
		mouse_my = HIWORD(lParam);
		break;

	}
	return false;
}

HRESULT CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:	// ������ ũ�⸦ �����ϴ� ���� ���� ���´�.
		break;
	default:
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);
	}
	//	return E_NOTIMPL;
	return 0;
}

void CFramework::Update(float fTimeElapsed)
{
	m_pCurrScene->Update(fTimeElapsed);
}

void CFramework::SetBKColor(COLORREF color)
{
	m_clrBackBuffer = color;
	if (m_hbrBackground) ::DeleteObject(m_hbrBackground);
	m_hbrBackground = ::CreateSolidBrush(m_clrBackBuffer);	// �� ����� �귯���� ������.
}

void CFramework::ClearBackgroundColor()
{
	::FillRect(m_hDC, &m_rcClient, m_hbrBackground);	// ���� ������� ��׶��忡 ���� Ŭ���̾�Ʈ��ŭ�� ������ ��� ���� �귯���� ������ ��ĥ�ض�.
}

void CFramework::PreprocessingForDraw()
{
	ClearBackgroundColor();	// �׸��� ������ ���۵Ǹ� ������ �׶��带 �����ϰ� �����.
	::SetBkColor(m_hDC, TRANSPARENT);	// ���ĺ��� ��� �����ϰ� ����
	::SetStretchBltMode(m_hDC, COLORONCOLOR);	// ���� ������ �޶� �þ�ų� �پ�� ������ �ִ� ��� �����.

	//printf("%d", m_pCurrScene->m_Tag);
	m_pCurrScene->Render(m_hDC);
}

void CFramework::OnDraw(HDC hDC)
{
	// ���ڷ� �޴� hDC�� �ۿ��� ������� hDC�̴�. �� ��Ʈ�ʿ� m_hDC�� ����ۿ��� ������� �׸��� �Ѹ���.
	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right, m_rcClient.bottom, m_hDC, 0, 0, SRCCOPY);
}

void CFramework::FrameAdvance()
{
	// Get tick
	m_timeElapsed = chrono::system_clock::now() - m_current_time;
	if (m_timeElapsed.count() > MAX_FPS)
	{
		m_current_time = chrono::system_clock::now();

		Update(m_timeElapsed.count());
		PreprocessingForDraw(); // ����� �����̹Ƿ� OnDraw�� �ƴϴ�. OnDraw ������ ����ۿ� �׷��ִ� ������ �Ѵ�.

		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(m_hWnd, &ps);
		OnDraw(hdc);
		::EndPaint(m_hWnd, &ps);

		InvalidateRect(m_hWnd, &m_rcClient, FALSE);	// False�� �ʱ�ȭ�� ���� �ʴ´ٴ� ���̴�. ������ ������ �޽����� ȣ���Ѵ�.
		if (m_timeElapsed.count() > 0.0)
			m_fps = 1.0 / m_timeElapsed.count();
	}
	// else return;


	InvalidateRect(m_hWnd, &m_rcClient, FALSE);	// False�� �ʱ�ȭ�� ���� �ʴ´ٴ� ���̴�. ������ ������ �޽����� ȣ���Ѵ�.

	// ĸ�ǿ� ���ڸ� �� ������ �����ϴ� ĸ�� ��Ʈ�� ����
	_itow_s(
		m_fps + 0.1f
		, m_CaptionTitle + m_TitleLength
		, TITLE_MX_LENGTH - m_TitleLength
		, 10);
	wcscat_s(
		m_CaptionTitle + m_TitleLength
		, TITLE_MX_LENGTH - m_TitleLength
		, TEXT("FPS )"));
	SetWindowText(m_hWnd, m_CaptionTitle);
}

LRESULT CFramework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CFramework* self = ::GetUserDataPtr<CFramework*>(hWnd);	// static �Լ��� �����Ϸ��� ������ ������. Ŭ������ �����Լ��� ����ϱ� ���� �ļ�
	if (!self)
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);	// �޽��� ó���� OS���� �ѱ��.

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:

	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:

	case WM_MOUSEMOVE:
		self->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;


	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		self->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hWnd, &ps);	// �� ����̽� ���ؽ�Ʈ�� ����ϰڴ�.
		// hdc���� �� �Լ��� ���� bitmap�� ���δ�.

		// CPU �ܰ迡�� ȣ���ϴ� �Ŷ� �޽��� �˻� �κ� ���� ������ ������.
		// ���� ������ �޽����� �� ���� ������ ���ؼ��� ���� �ʰ� �ȴ�. ������ �˾Ƴ���� �ؾ��Ѵ�.
		// �ȸ޽����� �˻��ϰ� Ʈ�����޽���, ����ġ�޽������� ���������ν����� �޽����� �Ѱ��ش�.
		self->OnDraw(hdc);

		::EndPaint(hWnd, &ps); // �׸��� �����.
	}
	break;
	case WM_DESTROY:
		::SetUserDataPtr(hWnd, NULL);
		::PostQuitMessage(0);
		break;

	default:
		return self->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
	}
	return 0;
	//	return LRESULT();
}

void CFramework::curSceneCreate()
{
	m_pCurrScene->OnCreate();
}

void CFramework::ChangeScene(CScene::SceneTag tag)
{
	m_pCurrScene = arrScene[tag];
}

/*
20171206 ���͵� ����
winmain()
{
	������ ���
	������ ����

	�޽��� ����
	{
		�޽��� ó��
		���� ���� ó��
		{
			FrameWork
				FrameAdvanced();
				{
					ProcessInput()	// Ű �Է�
					{
						WndProc
							WM_KEY:
							WM_MOUSE:
					}
					m_pscene->	Update();	// ���� ó��
					m_pscene->	Render()	// �׸���	// InvalidateRect()�� ���� �Ʒ� �� ������ �̵�
					{
						WndProc
							WM_PAINT:
							hdc = BeginDraw();
							Render(hdc);
							EndDraw(hdc);
					}
				}
		}
	}
}


private:
	CScene * m_pscene;
	Oncreate() �Լ� �ȿ��� new�� ���� �� ������ ����
	�� �̸��� enum tag�� ���� �����ָ� �ȴ�.

	** arrscene;
	2D���α׷��ֶ��� �޸�, �����ӿ�ũ�� ��ü������ �� �迭�� ������ �ִ�.
	[0]������ �κ�, [1]������ ���ӿ��� ��...
	�׷��� 2D�� Ǫ�� �˰��� �޸� ChangeScene���� �ذ��Ѵ�.

	�̷��� ������ �� �̵��� �����Ӵ�.


	update�� render�� ���� Ŭ�������� =0; �� �����

	���� scene���� ��ӹ޾Ƽ� �̰� ������ �־�� �Ѵ�.
*/