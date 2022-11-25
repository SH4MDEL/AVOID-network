#pragma once
#include "Object.h"

extern int mouse_mx;
extern int mouse_my;

class OBJECT_Bullet : public CObject
{
private:
	//float x = 0;									// x축 좌표와 y축 좌표
	//float y = 0;
	float xydata = 0;

	int direction = 0;						// 값에 따라 날아가는 방향이 정해짐.
											// 이 값은 OnCreate 과정에서 x와 y와 함께 넣어줘야 하는데, x와 y의 값은 enemyObject의 x, y 값을 따르므로
											// enemyObject에서 OnCreate를 해야 할 것 같음.
											// 이를테면 들튀에서 플레이어가 스킬을 사용하면 플레이어에서 스킬 오브젝트를 생성하듯이
											// 적 오브젝트가 총알 오브젝트를 생성하는 것으로 생각하면 될듯.

	bool draw = true;						// true일 경우 화면에 그림.

	float pi = 3.14159265358979;
	float TimeDelay = 0;
	float CrashTime = 0;

public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// 모니터 x길이 받아옴
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// 모니터 y길이 받아옴

	OBJECT_Bullet();
	OBJECT_Bullet(int x, int y, int direction);			// 시작 좌표. x와 y값은 MainEnemy의 값을 가져오고, direction은 채보에서 가져온다.

	~OBJECT_Bullet();

	CImage bullet;

	OBJECT_Bullet *nextbullet;						// 연결리스트 구현하기 위한 연결 포인터
	OBJECT_Bullet *prebullet;

	int radius = 18;
	float bulletx = 0;
	float bullety = 0;
	float BulletSpeed = 15;

	void OnCreate();
	void OnCreate(int x, int y, int direction);
	void Update(float fTimeElapsed);
	void Render(HDC* hDC);

	void SetDrawFalse();
	void SetDrawTrue();

	int GetX();
	int GetY();
	float GetCrashTime();
	void SetPos(int x, int y);
	void Setx(int x);
	void Sety(int y);
};
