#include "stdafx.h"
#include "Scene_PlayerWaiting.h"

#include "AVOID.h"
#include "Framework.h"		// 프레임워크 헤더 불러옴
#include "Sound.h"

Scene_PlayerWaiting::Scene_PlayerWaiting()
{

}

Scene_PlayerWaiting::Scene_PlayerWaiting(SceneTag tag, CFramework* pFramework) : CScene(tag, pFramework)
{
}

Scene_PlayerWaiting::~Scene_PlayerWaiting()
{
}

void Scene_PlayerWaiting::OnDestroy()
{
	m_backGround.Destroy();

	m_playerWaitingSound->stop(Sound::SoundTag::PlayerWaiting);	// 음악시작 효과음 정지
	m_playerWaitingSound->Release();							// 해제
	delete m_playerWaitingSound;
}

void Scene_PlayerWaiting::OnCreate()
{
	m_backGround.Load(L"Graphic\\UI\\Ingame.png");

	m_playerWaitingSound = new Sound;
	m_playerWaitingSound->init();
	m_playerWaitingSound->loading();

	m_playerWaitingSound->play(Sound::SoundTag::PlayerWaiting);

	m_nextScene = false;
	InitServer();
	cs_packet_login packet;
	packet.type = CS_PACKET_LOGIN;
	packet.size = sizeof(cs_packet_login);
	packet.selectMusic = m_selectedMusic;
	Send(&packet);
}

void Scene_PlayerWaiting::BuildObjects()
{
}

void Scene_PlayerWaiting::Render(HDC hDC)
{
	m_backGround.Draw(hDC, 0, 0, windowX, windowY);
}

void Scene_PlayerWaiting::Update(float fTimeElapsed)
{
	m_playerWaitingSound->update(Sound::SoundTag::PlayerWaiting);

	if (m_nextScene) {
		m_pFramework->ChangeScene(CScene::SceneTag::Ingame);
		m_pFramework->SetMusic(m_selectedMusic, CScene::SceneTag::Ingame);
		m_pFramework->curSceneCreate();
		OnDestroy();
	}
}

void Scene_PlayerWaiting::KeyState() {
	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		cs_packet_ready packet;
		packet.type = CS_PACKET_LOGIN;
		packet.size = sizeof(cs_packet_ready);
		Send(&packet);
	}
}

void Scene_PlayerWaiting::SetMusic(int selectedMusic)
{
	m_selectedMusic = selectedMusic;
}

void Scene_PlayerWaiting::ChangeGameStart(char playerNum)
{
	m_playerNum = (int)playerNum;
	m_nextScene = true;
}
