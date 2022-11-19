#include "stdafx.h"
#include "Scene_MusicSelect.h"

#include "Framework.h"		// 프레임워크 헤더 불러옴
#include "Sound.h"


Scene_MusicSelect::Scene_MusicSelect()
{
}

Scene_MusicSelect::Scene_MusicSelect(SceneTag tag, CFramework * pFramework) : CScene(tag, pFramework)
{
}

Scene_MusicSelect::~Scene_MusicSelect()
{
}

void Scene_MusicSelect::OnDestroy()
{
	for (int i = 0; i < 13; i++) {
		MusicSelect[i].Destroy();
	}
	Music[0].Destroy();
	Music[1].Destroy();

	this->SelectSound->stop(Sound::SoundTag::MusicStart);	// 음악시작 효과음 정지
	this->SelectSound->Release();							// 해제
	delete SelectSound;
}

void Scene_MusicSelect::OnCreate()
{
	WCHAR LoadText[100];

	this->selectedMusic = 0;
	this->CheckRight = false;					// 현재 Enter가 눌려있는가
	this->CheckLeft = false;					// 현재 Enter가 눌려있는가
	this->CheckReturn = false;					// 현재 Enter가 눌려있는가
	finish = false;						// 다음 Scene으로 넘어갈 준비가 되었는가
	this->music1 = true;
	this->music2 = false;
	this->FirstMusic = true;
	this->Musicstart = false;

	for (int i = 1; i <= 13; i++) {
		wsprintf(LoadText, L"Graphic\\UI\\MUSICSELECT\\MusicSelect%d.png", i);
		this->MusicSelect[i - 1].Load(LoadText);
	}

	Music[0].Load(L"Graphic\\UI\\MusicJacket1.png");
	Music[1].Load(L"Graphic\\UI\\MusicJacket2.png");

	this->SelectSound = new Sound;
	SelectSound->init();
	SelectSound->loading();
}

void Scene_MusicSelect::BuildObjects()
{
}

void Scene_MusicSelect::Render(HDC hDC)
{
	MusicSelect[ImageRender].Draw(hDC, 0, 0, windowX, windowY);

	if (this->selectedMusic == 0) {
		Music[0].Draw(hDC, windowX / 2 - 250, windowY / 2 - 230, 500, 520);
	}
	else if (this->selectedMusic == 1) {
		Music[1].Draw(hDC, windowX / 2 - 250, windowY / 2 - 230, 500, 520);
	}
}

void Scene_MusicSelect::Update(float fTimeElapsed)
{
	static float TimeBack = 0;
	TimeBack += fTimeElapsed;
	if (TimeBack >= 0.1f) {
		TimeBack = 0;
		if (ImageRender == 12) {
			ImageRender = 0;
		}
		else {
			ImageRender++;
		}
	}

	if (this->selectedMusic == 0) {									// 곡 선택에 따라 시스템의 업데이트가 다름
		if (music1) {												// music1 이 true 면
			if (!FirstMusic) {										// 맨처음 곡에선 선택효과음 x
				this->SelectSound->play(Sound::SoundTag::Select);	// 좌 or 우를 누른 후에야 효과음 O
			}
			this->SelectSound->play(Sound::SoundTag::bbkkbkk);		// 1번곡 재생
			this->SelectSound->stop(Sound::SoundTag::TrueBlue);		// 2번곡 정지
			music1 = false;											// if 한번만 돌게끔 false로 만듬
		}
		this->SelectSound->update(Sound::SoundTag::bbkkbkk);		// 1번곡 update
	}

	else if (this->selectedMusic == 1) {
		if (music2) {
			if (!FirstMusic) {
				this->SelectSound->play(Sound::SoundTag::Select);
			}
			this->SelectSound->play(Sound::SoundTag::TrueBlue);
			this->SelectSound->stop(Sound::SoundTag::bbkkbkk);
			music2 = false;
		}
		this->SelectSound->update(Sound::SoundTag::TrueBlue);
	}

	static float TimeDelay = 0;
	TimeDelay += fTimeElapsed;
	if (TimeDelay >= 1.f) {				// 1초는 지나야 넘어갈 수 있게 하자.
		KeyState();

		if (TimeDelay >= 5.f) {			// 키 검사 이후 일정 시간 뒤 다음 장면으로 넘어감				 원래는 5.f
			if (this->finish)
			{
				m_pFramework->ChangeScene(CScene::SceneTag::PlayerWaiting);
				m_pFramework->SetMusic(selectedMusic, CScene::SceneTag::PlayerWaiting);
				m_pFramework->curSceneCreate();
				Scene_MusicSelect::OnDestroy();
			}
		}
	}
}

void Scene_MusicSelect::KeyState() {
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		if (!CheckRight) {
			if (selectedMusic == MUSICNUM - 1) {
				selectedMusic = 0;
				music1 = true;					// 2번곡->1번곡   music1 = true
			}
			else {
				selectedMusic++;
				music2 = true;				// 1번곡->2번곡 music2 = true
			}
		}
		FirstMusic = false;
		CheckRight = true;
	}
	else {
		CheckRight = false;
	}

	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		if (!CheckLeft) {
			if (selectedMusic == 0) {
				selectedMusic = MUSICNUM - 1;
				music2 = true;
			}
			else {
				selectedMusic--;
				music1 = true;
			}
		}
		FirstMusic = false;
		CheckLeft = true;
	}
	else {
		CheckLeft = false;
	}

	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		if (!CheckReturn) {
			if (!this->Musicstart) {
				this->SelectSound->stop(selectedMusic + 1);				// 화살표 이동 시 선택한 곡만 틀어져 있으므로 해당 곡 정지
				this->SelectSound->play(Sound::SoundTag::MusicStart);
				this->Musicstart = true;
			}
			this->finish = true;
		}
		CheckReturn = true;
	}
	else {
		CheckReturn = false;
	}
}
