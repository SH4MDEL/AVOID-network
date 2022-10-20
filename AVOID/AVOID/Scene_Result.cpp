#include "stdafx.h"
#include "Scene_Result.h"

#include "Framework.h"

Scene_Result::Scene_Result()
{
}

Scene_Result::Scene_Result(SceneTag tag, CFramework * pFramework) : CScene(tag, pFramework)
{

}

Scene_Result::~Scene_Result()
{

}

void Scene_Result::OnDestroy()
{
	this->Result[0].Destroy();
	this->Result[1].Destroy();
}

void Scene_Result::BuildObjects()
{

}

void Scene_Result::OnCreate()
{
	this->Result[0].Load(L"Graphic\\UI\\Clear.png");
	this->Result[1].Load(L"Graphic\\UI\\Fail.png");

	this->CheckKey = false;
	this->finish = false;
}

void Scene_Result::Render(HDC hDC)
{
	if (finalhp >= 70.f) {
		this->Result[0].Draw(hDC, 0, 0, windowX, windowY);
	}
	else {
		this->Result[1].Draw(hDC, 0, 0, windowX, windowY);
	}
}

void Scene_Result::Update(float fTimeElapsed)
{
	KeyState();

	if (this->finish) {
		m_pFramework->ChangeScene(CScene::SceneTag::MusicSelect);
		m_pFramework->curSceneCreate();
		Scene_Result::OnDestroy();
	}
}

void Scene_Result::KeyState() {
	if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		if (!CheckKey) {
			this->finish = true;
		}
		CheckKey = true;
	}

	else {
		CheckKey = false;
	}
}
