#include "stdafx.h"
#include "Scene_Ingame.h"

#include "OBJECT_MainEnemy.h"
#include "OBJECT_Player.h"
#include "OBJECT_Bullet.h"

#include "Framework.h"
#include "Sound.h"

#ifdef USE_NETWORK
extern HANDLE g_event;
#endif

Scene_Ingame::Scene_Ingame()
{
}

Scene_Ingame::Scene_Ingame(SceneTag tag, CFramework* pFramework) : CScene(tag, pFramework)
{
}

Scene_Ingame::~Scene_Ingame()
{
}

void Scene_Ingame::OnDestroy()
{
	Ingame.Destroy();
	Circle.Destroy();
#ifdef USE_NETWORK
	for (int i = 0; i < m_playerNum; ++i) {
		delete m_players[i];
}
#endif // USE_NETWORK

#ifndef USE_NETWORK
	delete m_players[0];
#endif

	for (int i = 0; i < 12; i++) {
		delete CMainEnemy[i];
	}
	if (m_selectedMusic == 0) this->IngameSound->stop(Sound::SoundTag::bbkkbkk);
	else if (m_selectedMusic == 1) this->IngameSound->stop(Sound::SoundTag::TrueBlue);
	this->IngameSound->Release();
	delete IngameSound;
}

void Scene_Ingame::OnCreate()
{
	Ingame.Load(L"Graphic\\UI\\Ingame.png");
	Circle.Load(L"Graphic\\UI\\Circle.png");
	
	IngameSound = new Sound;
	IngameSound->init();
	IngameSound->loading();

#ifndef USE_NETWORK
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char Inbuff[3000];
	DWORD read_size = 3000;
	DWORD c = 3000;

	if (m_selectedMusic == 0) {
		hFileSpeed = CreateFile(L"Data\\bbkkbkkSpeed.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	}
	else if (m_selectedMusic == 1) {
		hFileSpeed = CreateFile(L"Data\\TrueBlueSpeed.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	}
	ReadFile(hFileSpeed, Inbuff, c, &read_size, NULL); // hFile에서 size 만큼 읽어 InBuff에 저장
	CloseHandle(hFileSpeed);

	int num = 0;
	for (int i = 0; i < 3000; i++) {
		if (Inbuff[i] >= 48 && Inbuff[i] <= 57) {
			lotateSpeed[num] = Inbuff[i] - 48;
			++num;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char noteInbuff[36000];
	DWORD read_note = 36000;
	DWORD n = 36000;
	if (m_selectedMusic == 0) {
		hFileNote = CreateFile(L"Data\\bbkkbkkNote.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	}
	else if (m_selectedMusic == 1) {
		hFileNote = CreateFile(L"Data\\TrueBlueNote.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	}
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
#endif

	Circleradius = 200;

	TimeDelay = -3.f;
	leastTime = 0;
	musicStart = 0;
	time = 0;

	BuildObjects();
}

void Scene_Ingame::BuildObjects()
{
	for (int i = 0; i < 12; i++) {
		this->CMainEnemy[i] = new OBJECT_MainEnemy(i + 1, m_selectedMusic + 1);		// location은 1부터 12까지 들어간다.
	}
#ifdef USE_NETWORK
	m_isGameEnd = false;
	m_rank = -1;
	for (int i = 0; i < m_playerNum; ++i) {
		m_players[i] = new OBJECT_Player(windowX, windowY, i);
	}
	for (int i = 0; i < 50; ++i) {
		m_bullets[i] = new OBJECT_Bullet;
	}
#endif
#ifndef USE_NETWORK
		m_players[0] = new OBJECT_Player(windowX, windowY);
#endif
}

void Scene_Ingame::Render(HDC hdc)
{
	Ingame.Draw(hdc, 0, 0, windowX, windowY);
	Circle.Draw(hdc, windowX / 2 - this->Circleradius, windowY / 2 - this->Circleradius, this->Circleradius * 2, this->Circleradius * 2);

	for (int i = 0; i < 12; i++) {
		CMainEnemy[i]->Render(&hdc);
	}
#ifdef USE_NETWORK
	for (int i = 0; i < m_playerNum; i++) {
		m_players[i]->Render(&hdc);
	}
	for (int i = 0; i < m_bulletNum; ++i) {
		m_bullets[i]->Render(&hdc);
	}
#endif // USE_NETWORK
#ifndef USE_NETWORK
	m_players[0]->Render(&hdc);
#endif // !USE_NETWORK

	hBrush = CreateSolidBrush(RGB(0, 0, 105));			// 옅은 파란색
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Rectangle(hdc, 0, windowY - 72, windowX * (m_players[m_playerID]->GetMp() / (float)(2.5) / 100), windowY - 50);		// mp에 따라 mp바의 길이가 달라짐

	SelectObject(hdc, oldBrush);
	DeleteObject(hBrush);

	if (m_players[m_playerID]->GetHp() < 70.f) {

		hBrush = CreateSolidBrush(RGB(105, 0, 0));			// 옅은 붉은색
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Rectangle(hdc, 0, windowY - 50, windowX * (m_players[m_playerID]->GetHp() / 100), windowY);		// hp에 따라 hp바의 길이가 달라짐

		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);
	}
	else {
		hBrush = CreateSolidBrush(RGB(0, 105, 0));			// 옅은 붉은색
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Rectangle(hdc, 0, windowY - 50, windowX * (m_players[m_playerID]->GetHp() / 100), windowY);		// hp에 따라 hp바의 길이가 달라짐

		SelectObject(hdc, oldBrush);
		DeleteObject(hBrush);
	}
}

void Scene_Ingame::Update(float fTimeElapsed)
{
	TimeDelay += fTimeElapsed;
	if (musicStart == false && TimeDelay >= -1.f) {
		if (m_selectedMusic == 0) {
			IngameSound->play(Sound::SoundTag::bbkkbkk);
		}
		else if (m_selectedMusic == 1) {
			IngameSound->play(Sound::SoundTag::TrueBlue);
		}
		musicStart = true;
	}
#ifdef USE_NETWORK
	for (int i = 0; i < m_playerNum; i++) {
		m_players[i]->Update(fTimeElapsed);
	}
#endif // USE_NETWORK
#ifndef USE_NETWORK
	m_players[0]->Update(fTimeElapsed);
#endif // !USE_NETWORK
#ifdef USE_NETWORK
	// 플레이어의 위치 정해주기
	if (!m_isGameEnd && TimeDelay >= -1.f) {
		cs_packet_player_status packet;
		packet.type = CS_PACKET_PLAYER_STATUS;
		packet.size = sizeof(cs_packet_player_status);
		packet.coord.x = (short)m_players[m_playerID]->GetServerX();
		packet.coord.y = (short)m_players[m_playerID]->GetServerY();
		packet.isSkill = m_players[m_playerID]->GetSpaceKeyDown();
		packet.playerID = m_players[m_playerID]->GetID();
		Send(&packet);
#ifdef NETWORK_DEBUG
		cout << "CS_PACKET_PLAYER_STATUS 송신" << endl;
#endif // NETWORK_DEBUG
		WaitForSingleObject(g_event, INFINITE);
		// 플레이어의 위치 정해주기
		for (int i = 0; i < m_playerNum; ++i) {
			PlayerStatus ps = m_playersStatus[i];
			m_players[i]->SetServerPos(ps.coord.x, ps.coord.y);
			m_players[i]->SetAbilityState(ps.isSkill);
			if (ps.isCollide) {
				if (i == m_playerID) {
					m_players[i]->SetHp(m_players[i]->hitHp);
					if (m_players[0]->GetHp() <= 0) {
						m_players[0]->SetHp_zero();
					}
					IngameSound->play(Sound::SoundTag::Hitsound);
				}
				m_players[i]->SetState(true);			// 피격시 무적으로 만들음
			}
		}
		// 적의 위치 정해주기
		for (int i = 0; i < m_enemyNum; ++i) {
			Coord coord = m_enemysCoord[i];
			CMainEnemy[i]->SetServerPos(coord.x, coord.y);
		}
		// 총알의 위치 정해주기
		for (int i = 0; i < m_bulletNum; ++i) {
			Coord coord = m_bulletsCoord[i];
			m_bullets[i]->SetServerPos(coord.x, coord.y);
		}
	}
	if (m_isGameEnd && m_rank == -1) {
		cs_packet_player_hp packet;
		packet.type = CS_PACKET_PLAYER_HP;
		packet.size = sizeof(cs_packet_player_hp);
		packet.playerID = m_playerID;
		packet.hp = m_players[m_playerID]->GetHp();
		Send(&packet);
	}
	if (m_rank != -1) {
#ifdef NETWORK_DEBUG
		cout << "SCENE 전환(Ingame -> Result)" << endl;
#endif // NETWORK_DEBUG
#ifdef USE_NETWORK
#endif // USE_NETWORK
		m_pFramework->ChangeScene(CScene::SceneTag::Result);
		m_pFramework->SetRank(m_rank, CScene::SceneTag::Result);
		m_pFramework->curSceneCreate();
		OnDestroy();
	}
#endif
#ifndef USE_NETWORK
	for (int i = 0; i < 12; i++) {
		PlayerCrash(CMainEnemy[i]);
		AbilityCrash(CMainEnemy[i]);
	}

	if (TimeDelay >= 0.f) {
		leastTime += fTimeElapsed;
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (m_selectedMusic == 0) {
			if (TimeDelay >= (60.f / 680.f) * (float)time) {		// 60 / bpm / 4 초마다 도는 속도를 읽어들인다.
				for (int i = 0; i < 12; i++) {
					CMainEnemy[i]->Update(leastTime, lotateSpeed[time], note[time][i]);
				}
				++time;
				leastTime = 0;
			}
		}

		else if (m_selectedMusic == 1) {
			if (TimeDelay >= (60.f / 656.f) * (float)time) {		// 60 / bpm / 4 초마다 도는 속도를 읽어들인다.
				for (int i = 0; i < 12; i++) {
					CMainEnemy[i]->Update(leastTime, lotateSpeed[time], note[time][i]);
				}
				++time;
				leastTime = 0;
			}
		}
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	if ((m_selectedMusic == 0 && time >= 1450) || (m_selectedMusic == 1 && time >= 1310)) {
		finalhp = m_players[0]->GetHp();
		m_pFramework->ChangeScene(CScene::SceneTag::Result);
		Scene_Ingame::OnDestroy();
		m_pFramework->curSceneCreate();
	}
#endif // !USE_NETWORK
}

void Scene_Ingame::PlayerCrash(OBJECT_MainEnemy* Enemy)
{
#ifndef USE_NETWORK
	double d = 0;
	double r1 = 0;
	double r2 = 0;

	for (auto bullet = Enemy->m_bullets.begin(); bullet != Enemy->m_bullets.end(); ++bullet) {
		d = sqrt((double)(pow((*bullet)->bulletx - (double)m_players[0]->GetX(), 2) + pow((*bullet)->bullety - (double)m_players[0]->GetY(), 2)));
		r1 = (*bullet)->radius + m_players[0]->GetRadius();
		r2 = (*bullet)->radius - m_players[0]->GetRadius();

		if (((r2 < d && d <= r1) || r2 >= d) && !m_players[0]->GetState()) {			// 충돌, 무적 x 이면 hp 감소
			m_players[0]->SetHp(m_players[0]->hitHp);
			if (m_players[0]->GetHp() <= 0) {
				m_players[0]->SetHp_zero();
			}

			IngameSound->play(Sound::SoundTag::Hitsound);
			m_players[0]->SetState(true);			// 피격시 무적으로 만들음
		}
	}
#endif // !USE_NETWORK
}

void Scene_Ingame::AbilityCrash(OBJECT_MainEnemy* Enemy)
{
#ifndef USE_NETWORK
	double d = 0;
	double r1 = 0;
	double r2 = 0;

	for (auto bullet = Enemy->m_bullets.begin(); bullet != Enemy->m_bullets.end(); ++bullet) {
		d = sqrt((double)(pow((*bullet)->bulletx - (double)m_players[0]->GetX(), 2) + pow((*bullet)->bullety - (double)m_players[0]->GetY(), 2)));
		r1 = (*bullet)->radius + m_players[0]->Ability_radius;
		r2 = (*bullet)->radius - m_players[0]->Ability_radius;

		if (((r2 < d && d <= r1) || r2 >= d) && m_players[0]->GetAbilityState()) {			// 충돌, 능력o 이면 속도 감소
			(*bullet)->BulletSpeed = 3;
		}
		else {
			(*bullet)->BulletSpeed = 10;
		}
	}
#endif
}

void Scene_Ingame::SetPlayerNum(int playerNum)
{
#ifdef USE_NETWORK
	m_playerNum = playerNum;
#endif
}

void Scene_Ingame::SetPlayerID(int playerID)
{
#ifdef USE_NETWORK
	m_playerID = playerID;
#endif
}

void Scene_Ingame::SetMusic(int selectedMusic)
{
	m_selectedMusic = selectedMusic;
}
