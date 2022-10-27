#include "stdafx.h"
#include "Scene_Music1.h"

#include "OBJECT_MainEnemy.h"
#include "OBJECT_Player.h"
#include "OBJECT_Bullet.h"

#include "Framework.h"
#include "Sound.h"

Scene_Music1::Scene_Music1()
{
}

Scene_Music1::Scene_Music1(SceneTag tag, CFramework * pFramework) : CScene(tag, pFramework)
{
}

Scene_Music1::~Scene_Music1()
{
}

void Scene_Music1::OnDestroy()
{
	Ingame.Destroy();
	Circle.Destroy();
	delete Player;

	for (int i = 0; i < 12; i++) {
		delete CMainEnemy[i];
	}

	this->IngameSound->stop(Sound::SoundTag::bbkkbkk);
	this->IngameSound->Release();
	delete IngameSound;
}

void Scene_Music1::OnCreate()
{
	Ingame.Load(L"Graphic\\UI\\Ingame.png");
	Circle.Load(L"Graphic\\UI\\Circle.png");

	IngameSound = new Sound;
	IngameSound->init();
	IngameSound->loading();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char Inbuff[3000];
	DWORD read_size = 3000;
	DWORD c = 3000;

	hFileSpeed = CreateFile(L"Data\\bbkkbkkSpeed.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	ReadFile(hFileSpeed, Inbuff, c, &read_size, NULL); // hFile에서 size 만큼 읽어 InBuff에 저장
	CloseHandle(hFileSpeed);

	for (int i = 0; i < 3000; i++) {
		static int num = 0;
		if (Inbuff[i] >= 48 && Inbuff[i] <= 57) {
			lotateSpeed[num] = Inbuff[i] - 48;
			num++;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char noteInbuff[36000];
	DWORD read_note = 36000;
	DWORD n = 36000;

	hFileNote = CreateFile(L"Data\\bbkkbkkNote.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	ReadFile(hFileNote, noteInbuff, n, &read_note, NULL); // hFile에서 size 만큼 읽어 InBuff에 저장
	CloseHandle(hFileNote);

	int numI = 0, numJ = 0;
	for (int i = 0; i < 36000; i++) {
		if (noteInbuff[i] >= 48 && noteInbuff[i] <= 57) {
			note[numI][numJ] = noteInbuff[i] - 48;
			if (numJ == 11) {
				numJ = 0, numI++;
			}
			else {
				numJ++;
			}
		}
	}

	Circleradius = 200;

	TimeDelay = -3.f;
	leastTime = 0;
	musicStart = 0;
	time = 0;

	BuildObjects();
}

void Scene_Music1::BuildObjects()
{
	for (int i = 0; i < 12; i++) {
		this->CMainEnemy[i] = new OBJECT_MainEnemy(i + 1, 1);		// location은 1부터 12까지 들어간다.
	}
	this->Player = new OBJECT_Player(windowX, windowY);
}

void Scene_Music1::Render(HDC hdc)
{
	Ingame.Draw(hdc, 0, 0, windowX, windowY);
	Circle.Draw(hdc, windowX / 2 - this->Circleradius, windowY / 2 - this->Circleradius, this->Circleradius * 2, this->Circleradius * 2);

	this->Player->Render(&hdc);

	for (int i = 0; i < 12; i++) {
		this->CMainEnemy[i]->Render(&hdc);
	}
}

void Scene_Music1::Update(float fTimeElapsed)
{
	TimeDelay += fTimeElapsed;

	this->Player->Update(fTimeElapsed);

	for (int i = 0; i < 12; i++) {
		PlayerCrash(CMainEnemy[i]);
		AbilityCrash(CMainEnemy[i]);
	}

	if (musicStart == false && TimeDelay >= -1.f) {
		IngameSound->play(Sound::SoundTag::bbkkbkk);
		musicStart = true;
	}

	if (TimeDelay >= 0.f) {
		leastTime += fTimeElapsed;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (TimeDelay >= (60.f / 680.f) * (float)time) {		// 60 / bpm / 4 초마다 도는 속도를 읽어들인다.
			for (int i = 0; i < 12; i++) {
				CMainEnemy[i]->Update(leastTime, lotateSpeed[time], note[time][i]);
			}
			++time;
			leastTime = 0;
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	if (time >= 1450) {
		finalhp = Player->GetHp();
		m_pFramework->ChangeScene(CScene::SceneTag::Result);
		Scene_Music1::OnDestroy();
		m_pFramework->curSceneCreate();
	}

}

void Scene_Music1::PlayerCrash(OBJECT_MainEnemy* Enemy)
{
	double d = 0;
	double r1 = 0;
	double r2 = 0;

	for (auto bullet = Enemy->m_bullets.begin(); bullet != Enemy->m_bullets.end(); ++bullet) {
		d = sqrt((double)(pow((*bullet)->bulletx - (double)Player->GetX(), 2) + pow((*bullet)->bullety - (double)Player->GetY(), 2)));
		r1 = (*bullet)->radius + Player->GetRadius();
		r2 = (*bullet)->radius - Player->GetRadius();

		if (((r2 < d && d <= r1) || r2 >= d) && !Player->GetState()) {			// 충돌, 무적 x 이면 hp 감소
			Player->SetHp(Player->hitHp);
			if (Player->GetHp() <= 0) {
				Player->SetHp_zero();
			}

			IngameSound->play(Sound::SoundTag::Hitsound);
			Player->SetState(true);			// 피격시 무적으로 만들음
		}
	}
}

void Scene_Music1::AbilityCrash(OBJECT_MainEnemy* Enemy)
{
	double d = 0;
	double r1 = 0;
	double r2 = 0;

	for (auto bullet = Enemy->m_bullets.begin(); bullet != Enemy->m_bullets.end(); ++bullet) {
		d = sqrt((double)(pow((*bullet)->bulletx - (double)Player->GetX(), 2) + pow((*bullet)->bullety - (double)Player->GetY(), 2)));
		r1 = (*bullet)->radius + Player->Ability_radius;
		r2 = (*bullet)->radius - Player->Ability_radius;

		if (((r2 < d && d <= r1) || r2 >= d) && Player->GetAState()) {			// 충돌, 능력o 이면 속도 감소
			(*bullet)->BulletSpeed = 3;
		}
		else {
			(*bullet)->BulletSpeed = 10;
		}
	}
}