#pragma once
#include "Object.h"

extern int mouse_mx;
extern int mouse_my;

class OBJECT_Bullet : public CObject
{
private:
	//float x = 0;									// x�� ��ǥ�� y�� ��ǥ
	//float y = 0;
	float xydata = 0;

	int direction = 0;						// ���� ���� ���ư��� ������ ������.
											// �� ���� OnCreate �������� x�� y�� �Բ� �־���� �ϴµ�, x�� y�� ���� enemyObject�� x, y ���� �����Ƿ�
											// enemyObject���� OnCreate�� �ؾ� �� �� ����.
											// �̸��׸� ��Ƣ���� �÷��̾ ��ų�� ����ϸ� �÷��̾�� ��ų ������Ʈ�� �����ϵ���
											// �� ������Ʈ�� �Ѿ� ������Ʈ�� �����ϴ� ������ �����ϸ� �ɵ�.

	bool draw = true;						// true�� ��� ȭ�鿡 �׸�.

	float pi = 3.14159265358979;
	float TimeDelay = 0;
	float CrashTime = 0;

public:
	int windowX = ::GetSystemMetrics(SM_CXSCREEN);		// ����� x���� �޾ƿ�
	int windowY = ::GetSystemMetrics(SM_CYSCREEN);		// ����� y���� �޾ƿ�

	OBJECT_Bullet();
	OBJECT_Bullet(int x, int y, int direction);			// ���� ��ǥ. x�� y���� MainEnemy�� ���� ��������, direction�� ä������ �����´�.

	~OBJECT_Bullet();

	CImage bullet;

	OBJECT_Bullet *nextbullet;						// ���Ḯ��Ʈ �����ϱ� ���� ���� ������
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
