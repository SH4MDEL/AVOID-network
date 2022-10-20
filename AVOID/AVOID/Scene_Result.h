#pragma once
#include "Scene.h"

extern int finalhp;

class Scene_Result : public CScene
{
public:
	Scene_Result();
	Scene_Result(SceneTag tag, CFramework * pFramwork);
	~Scene_Result();

	void OnDestroy();

	void BuildObjects() override;
	void OnCreate() override;
	void Render(HDC hDC) override;
	void Update(float fTimeElapsed) override;

	int finalhp = 0;
	void KeyState();

private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	CImage Result[2];

	bool CheckKey;
	bool finish;
};

