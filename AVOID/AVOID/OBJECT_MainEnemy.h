#pragma once
#include "Object.h"

class OBJECT_Bullet;
class CFramework;

class OBJECT_MainEnemy : public CObject
{
private:
	float x = 0;									// x축 좌표와 y축 좌표. 생성시 적어준 location의 값에 따라 달라진다.
	float y = 0;									// 그 외 MainEnemy의 좌표가 달라질 일은?
	float xydata = 0;

	float beatCount = 0;						// 이미지를 움직이는데 사용되는 변수.
	int imageCount = 0;							// 현재 선택된 이미지를 나타냄

	bool draw = true;							// true일 경우 MainEnemy를 그림

	float pi = 3.14159265358979;
	int direction = 0;							// Ingame으로부터 노트의 방향을 보냄. 0일경우 아무것도 하지 않음.
	int bulletNum = 0;

	int selectedMusic = 0;
	int enemyc = 0;

public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	OBJECT_Bullet* head;						// head 가리키는 포인터

	OBJECT_MainEnemy();							// 그냥 생성할 경우
	OBJECT_MainEnemy(int location, int selectedMusic);				// 좌표를 넣어 생성할 경우
												// 시작점을 나타낸다. 1부터 12까지 넣을 수 있으며, 
												// 해당 숫자는 시계 방향으로 적 오브젝트가 등장할 위치를 의미
	~OBJECT_MainEnemy();						// 소멸자

	CImage Enemy[2];
	//CImage Siren;

	void OnCreate(int location);				// 생성시 좌표를 설정해 준다.
	void Update(float fTimeElapsed, int lotateSpeed, int note);
	void Render(HDC* hdc);
	//void firstRender(HDC* hdc);

	void BulletCrash();							// 원과 총알 충돌체크
	void DeleteBullet(OBJECT_Bullet* bullet);

	void SetDrawFalse();				
	void SetDrawTrue();


};
