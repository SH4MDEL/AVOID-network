#pragma once
#include "Scene.h"

class Sound;
class OBJECT_MainEnemy;
class OBJECT_Player;

extern int finalhp;

class Scene_Music2 : public CScene
{
public:
	Scene_Music2();
	Scene_Music2(SceneTag tag, CFramework * pFramework);
	~Scene_Music2();

	void OnDestroy();
	void OnCreate() override;		// 만들때 선언용 함수

	void BuildObjects() override;
	void Render(HDC hdc) override;
	void Update(float fTimeElapsed) override;

	void PlayerCrash(OBJECT_MainEnemy* Enemy);
	void AbilityCrash(OBJECT_MainEnemy* Enemy);

	void KeyState();


private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	OBJECT_MainEnemy* CMainEnemy[12] = { NULL };
	OBJECT_Player* Player = nullptr;

	Sound* IngameSound = NULL;

	CImage Ingame;			// 배경 화면
	CImage Circle;			// 플레이가 이뤄질 원	

	int Circleradius = 200;

	int SelectedMusic;
	float TimeDelay = -3;
	float leastTime = 0;
	bool musicStart = 0;

	HANDLE hFileSpeed, hFileNote;

	int lotateSpeed[3000] = { NULL };
	int note[3000][12] = { 0 };									// 파일 입출력을 통하여 노트의 정보를 받아와 저장한다.
	int time = 0;													// 노래가 시작하고 얼마나 지나갔는가? 1당 bpm / 240 이다.
};																// time 이 1310을 넘으면 노래 종료

