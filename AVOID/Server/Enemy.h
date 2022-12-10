#pragma once
#include "Object.h"
#include "Bullet.h"

constexpr int ENEMY_DIST = 220;

class Enemy : public Object
{
public:
	Enemy(int location);
	~Enemy() = default;

	void Update(float timeElapsed) override;
	void SetNextNode(int speed, int note);

	list<Bullet>& GetBullets() { return m_bullets; };
	size_t GetBulletCount() { return m_bullets.size(); }

	void SetBullet(const Bullet& bullet) { m_bullets.push_back(bullet); }

private:
	list<Bullet>		m_bullets;
	float				m_theta;
	float				m_speed;
};

