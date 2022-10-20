#pragma once
#include "Scene.h"

#define MUSICNUM 2				// ������ ������ ����� �� ����

class Sound;

class Scene_MusicSelect : public CScene
{
public:
	Scene_MusicSelect();
	Scene_MusicSelect(SceneTag tag, CFramework * pFramework);
	~Scene_MusicSelect();

	void OnDestroy();			// ���鶧 �����Ѱ� ������ �Լ�
	void OnCreate() override;	// ��� ���� �Լ�

	void BuildObjects() override;
	void Render(HDC hDC) override;				// FrameAdvanced
	void Update(float fTimeElapsed) override;	// m_pCurrScene->Update();
												// m_pCurrScene->Render(m_hdc);
	void KeyState();
private:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	Sound* SelectSound = NULL;

	CImage MusicSelect[13];			// ���
	CImage Music[2];			// ������ �ٹ� ����

	int ImageRender = 0;

	bool music1;
	bool music2;
	bool FirstMusic;
	bool Musicstart;

	bool CheckRight;
	bool CheckLeft;
	bool CheckReturn;
	int selectedMusic;			// ���� ���õ� ���� 

	bool finish;
};