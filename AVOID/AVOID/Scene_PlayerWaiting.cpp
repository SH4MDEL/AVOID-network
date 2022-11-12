#include "stdafx.h"
#include "Scene_PlayerWaiting.h"

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

}

void Scene_PlayerWaiting::OnCreate()
{
#ifndef USE_NETWORK
	m_pFramework->InitServer();
#endif // !USE_NETWORK
	
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
