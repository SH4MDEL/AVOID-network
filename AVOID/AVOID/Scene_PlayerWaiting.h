#pragma once
#include "Scene.h"

class Sound;

class Scene_PlayerWaiting : public CScene
{
public:
	Scene_PlayerWaiting();
	Scene_PlayerWaiting(SceneTag tag, CFramework* pFramework);
	~Scene_PlayerWaiting();

	void OnDestroy();			// 만들때 선언한것 해제용 함수
	void OnCreate() override;	// 장면 선언 함수

	void BuildObjects() override;
	void Render(HDC hDC) override;				// FrameAdvanced
	void Update(float fTimeElapsed) override;	// m_pCurrScene->Update();
												// m_pCurrScene->Render(m_hdc);
	void KeyState();

	// 서버 관련 추가 함수
	void SetPlayerID(char playerID) { m_playerID = (int)playerID; }
	void ChangeGameStart(char playerNum) { m_playerNum = (int)playerNum; }
private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	Sound* SelectSound = NULL;

	CImage m_backGround;

	// 서버 추가 관련 변수
	int m_playerID;
	int m_playerNum;
};

