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

	// 서버 관련 추가 함수입니다.
	void SetRank(int rank) { m_rank = rank; }

private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	CImage Result[2];
	CImage m_imageRank[3];

	bool CheckKey;
	bool finish;

	// 서버 관련 추가 변수입니다.
	int		m_rank;
};

