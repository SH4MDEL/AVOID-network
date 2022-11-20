#pragma once
#include "Object.h"
#include "Obj_Bullet.h"

class Enemy : CObject {
private:
	Coord m_position;
	float m_fBeatCount;

	int m_iDirection;
	int m_nBullet;

public:

	std::list<Bullet> m_bullets;

	Enemy();
	~Enemy();

	void OnCreate(int location);
	void Update(float fTimeElapsed, int lotateSpeed, int note);

	Coord GetPosition() { return m_position; };
	float GetBeatCount() { return m_fBeatCount; };
	int GetDirection() { return m_iDirection; };
	int GetBulletCount() { return m_nBullet; };

	void SetPosition(Coord position) { m_position = position; };
	void SetBeatCount(float fBeatCount) { m_fBeatCount = fBeatCount; };
	void SetDirection(int iDirection) { m_iDirection = iDirection; };
	void SetBulletCount(int nBullet) { m_nBullet = nBullet; };
};