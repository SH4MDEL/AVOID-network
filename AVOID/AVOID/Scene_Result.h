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
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	CImage Result[2];

	bool CheckKey;
	bool finish;
};

