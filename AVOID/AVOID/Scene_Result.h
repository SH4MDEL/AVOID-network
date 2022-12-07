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

	// ���� ���� �߰� �Լ��Դϴ�.
	void SetRank(int rank) { m_rank = rank; }

private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	CImage Result[2];
	CImage m_imageRank[3];

	bool CheckKey;
	bool finish;

	// ���� ���� �߰� �����Դϴ�.
	int		m_rank;
};

