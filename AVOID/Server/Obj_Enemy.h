#pragma once

#include "Object.h"
#include "Obj_Bullet.h"

constexpr int ENEMY_DIST = 220;

class Enemy : CObject {
public:

	Enemy();
	Enemy(int location);
	~Enemy();

	void OnCreate(int location);
	void Update(float fTimeElapsed, int lotateSpeed, int note, int selectedMusic);

	Coord GetPosition() { return m_position; };
	float GetBeatCount() { return m_fBeatCount; };
	int GetDirection() { return m_iDirection; };
	int GetBulletCount() { return m_nBullet; };
	std::list<Bullet>& GetBullets() { return m_bullets; }

	void SetPosition(Coord position) { m_position = position; };
	void SetBeatCount(float fBeatCount) { m_fBeatCount = fBeatCount; };
	void SetDirection(int iDirection) { m_iDirection = iDirection; };
	void SetBulletCount(int nBullet) { m_nBullet = nBullet; };
	void SetBullet(const Bullet& bullet) { m_bullets.push_back(bullet); }


private:
	std::list<Bullet> m_bullets;

	Coord m_position;
	float m_fBeatCount;

	int m_iDirection;
	int m_nBullet;

	float theta;
};