#pragma once
#include "Scene.h"

class Sound;

class CMainScene : public CScene
{
public:
	CMainScene();
	CMainScene(SceneTag tag, CFramework * pFramework);
	~CMainScene();

	void OnDestroy();			// 만들때 선언한것 해제용 함수
	void OnCreate() override;	// 장면 선언 함수

	void BuildObjects() override;
	void Render(HDC hdc) override;				// FrameAdvanced
	void Update(float fTimeElapsed) override;	// m_pCurrScene->Update();
												// m_pCurrScene->Render(m_hdc);
	void KeyState();
private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	Sound* MainSound = NULL;

	CImage MainTitle[7];			// 메인 화면
	CImage manual[2];			// 도움말 창
	CImage STs;					// 감사합니다.

	int ImageRender = 0;

	bool CheckKey;
	int SceneNum;				// 장면 전환용 변수

	bool finish;

	CObject** ppObjects;
	int nObjects;
};