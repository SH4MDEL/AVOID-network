#pragma once
#include "Scene.h"

class Sound;

class CMainScene : public CScene
{
public:
	CMainScene();
	CMainScene(SceneTag tag, CFramework * pFramework);
	~CMainScene();

	void OnDestroy();			// ���鶧 �����Ѱ� ������ �Լ�
	void OnCreate() override;	// ��� ���� �Լ�

	void BuildObjects() override;
	void Render(HDC hdc) override;				// FrameAdvanced
	void Update(float fTimeElapsed) override;	// m_pCurrScene->Update();
												// m_pCurrScene->Render(m_hdc);
	void KeyState();
private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	Sound* MainSound = NULL;

	CImage MainTitle[7];			// ���� ȭ��
	CImage manual[2];			// ���� â
	CImage STs;					// �����մϴ�.

	int ImageRender = 0;

	bool CheckKey;
	int SceneNum;				// ��� ��ȯ�� ����

	bool finish;

	CObject** ppObjects;
	int nObjects;
};