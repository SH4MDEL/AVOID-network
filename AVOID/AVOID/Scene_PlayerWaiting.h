#pragma once
#include "Scene.h"

#ifdef USE_NETWORK

class Sound;

class Scene_PlayerWaiting : public CScene
{
public:
	Scene_PlayerWaiting();
	Scene_PlayerWaiting(SceneTag tag, CFramework* pFramework);
	~Scene_PlayerWaiting();

	void OnDestroy();			// ���鶧 �����Ѱ� ������ �Լ�
	void OnCreate() override;	// ��� ���� �Լ�

	void BuildObjects() override;
	void Render(HDC hDC) override;				// FrameAdvanced
	void Update(float fTimeElapsed) override;	// m_pCurrScene->Update();
												// m_pCurrScene->Render(m_hdc);
	void KeyState();


	void SetMusic(int selectedMusic);

	// ���� ���� �߰� �Լ�
	void SetPlayerID(char playerID) { m_playerID = (int)playerID; }
	void ChangeGameStart(char playerNum);
private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	Sound* m_playerWaitingSound = NULL;

	CImage m_backGround;

	int m_selectedMusic;

	// ���� �߰� ���� ����

	int m_playerID;
	int m_playerNum;

	bool m_nextScene;
	bool m_returnOK;
};
#endif

