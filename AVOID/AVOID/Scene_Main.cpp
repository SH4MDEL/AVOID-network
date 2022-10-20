#include "stdafx.h"
#include "Scene_Main.h"

#include "Framework.h"		// �����ӿ�ũ ��� �ҷ���
#include "Sound.h"

//double theta = 0;

CMainScene::CMainScene()
{
}

CMainScene::CMainScene(SceneTag tag, CFramework * pFramework) : CScene(tag, pFramework)
{

}

CMainScene::~CMainScene()
{
}

void CMainScene::OnDestroy()
{
	for (int i = 0; i < 7; i++) {
		MainTitle[i].Destroy();
	}
	manual[0].Destroy();
	manual[1].Destroy();
	STs.Destroy();

	this->MainSound->stop(Sound::SoundTag::GameStart);
	this->MainSound->Release();
	delete MainSound;				// Sound �����Ҵ� ����
}

void CMainScene::OnCreate()
{
	WCHAR LoadText[100];

	CheckKey = false;					// ���� Enter�� �����ִ°�
	SceneNum = 0;						// �� ��° ȭ���� ������ �ִ°�
	finish = false;						// ���� Scene���� �Ѿ �غ� �Ǿ��°�

	for (int i = 1; i <= 13; i++) {
		wsprintf(LoadText, L"Graphic\\UI\\MAINTITLE\\MainTitle%d.png", i);
		this->MainTitle[i - 1].Load(LoadText);
	}

	manual[0].Load(L"Graphic\\UI\\Help1.png");
	manual[1].Load(L"Graphic\\UI\\Help2.png");
	STs.Load(L"Graphic\\UI\\SpecialThanks.png");

	this->MainSound = new Sound;
	MainSound->init();
	MainSound->loading();
	MainSound->play(Sound::SoundTag::MainSound);
}

void CMainScene::BuildObjects()
{
}

void CMainScene::Render(HDC hdc)
{
	MainTitle[ImageRender].Draw(hdc, 0, 0, windowX, windowY);

	if (SceneNum != 0 && SceneNum != 3 && SceneNum != 4) {				// 0�� 3, 4�϶��� �� ȭ���̹Ƿ�
		manual[SceneNum - 1].Draw(hdc, windowX / 2 - 500, windowY / 2 - 300, 1000, 600);
	}
	if (SceneNum == 3) {
		STs.Draw(hdc, windowX / 2 - 500, windowY / 2 - 300, 1000, 600);
	}

}

void CMainScene::Update(float fTimeElapsed)
{
	static float TimeBack = 0;
	TimeBack += fTimeElapsed;
	if (TimeBack >= 0.1f) {
		TimeBack = 0;
		if (ImageRender == 6) {
			ImageRender = 0;
		}
		else {
			ImageRender++;
		}
	}

	KeyState();

	this->MainSound->update(Sound::SoundTag::MainSound);		// ���� ������ �ý����� �����Ӵ� ������Ʈ

	static float TimeDelay = 0;
	TimeDelay += fTimeElapsed;	
	if (TimeDelay >= 5.5f) {				// ����Ű ���� ��� �� ����
		if (finish)
		{
			m_pFramework->ChangeScene(CScene::SceneTag::MusicSelect);
			m_pFramework->curSceneCreate();
			CMainScene::OnDestroy();
		}
	}

}

void CMainScene::KeyState() {
	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		if (!CheckKey) {
			switch (SceneNum) {
			case 0:
				SceneNum = 1;
				break;
			case 1:
				SceneNum = 2;
				break;
			case 2:
				SceneNum = 3;
				break;
			case 3:
				SceneNum = 4;
				break;
			case 4:
				finish = true;
				this->MainSound->stop(Sound::SoundTag::MainSound);
				this->MainSound->play(Sound::SoundTag::GameStart);
				break;
			}
		}
		CheckKey = true;
	}
	else {
		CheckKey = false;
	}
}

