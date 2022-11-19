#include "stdafx.h"
#include "Scene_PlayerWaiting.h"

#include "AVOID.h"
#include "Framework.h"		// �����ӿ�ũ ��� �ҷ���
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

}

void Scene_PlayerWaiting::OnCreate()
{
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

}

void Scene_PlayerWaiting::Update(float fTimeElapsed)
{

}

void Scene_PlayerWaiting::KeyState() {
	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {

	}
}

void Scene_PlayerWaiting::SetMusic(int selectedMusic)
{
	m_selectedMusic = selectedMusic;
}
