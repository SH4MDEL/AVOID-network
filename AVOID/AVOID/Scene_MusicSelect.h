#pragma once
#include "Scene.h"

#define MUSICNUM 2				// 음악의 개수가 변경될 때 수정

class Sound;

class Scene_MusicSelect : public CScene
{
public:
	Scene_MusicSelect();
	Scene_MusicSelect(SceneTag tag, CFramework * pFramework);
	~Scene_MusicSelect();

	void OnDestroy();			// 만들때 선언한것 해제용 함수
	void OnCreate() override;	// 장면 선언 함수

	void BuildObjects() override;
	void Render(HDC hDC) override;				// FrameAdvanced
	void Update(float fTimeElapsed) override;	// m_pCurrScene->Update();
												// m_pCurrScene->Render(m_hdc);
	void KeyState();
private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	Sound* SelectSound = NULL;

	CImage MusicSelect[13];			// 배경
	CImage Music[2];			// 음악의 앨범 자켓

	int ImageRender = 0;

	bool music1;
	bool music2;
	bool FirstMusic;
	bool Musicstart;

	bool CheckRight;
	bool CheckLeft;
	bool CheckReturn;
	int selectedMusic;			// 현재 선택된 음악 

	bool finish;
};