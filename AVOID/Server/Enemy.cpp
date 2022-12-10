#include "Enemy.h"

Enemy::Enemy(int location) : m_theta((float)location * PI / 6.f)
{
	m_position.x = (float)ENEMY_DIST * cos(m_theta);
	m_position.y = (float)ENEMY_DIST * sin(m_theta);
}

void Enemy::Update(float timeElapsed)
{
	if (g_selectedMusic == 0) {
		m_theta += m_speed * PI * 1.f / 170.f * (timeElapsed * (680.f / 60.f));
	}
	else if (g_selectedMusic == 1) {
		m_theta += m_speed * PI * 1.f / 170.f * (timeElapsed * (656.f / 60.f));
	}
	m_position.x = (float)ENEMY_DIST * cos(m_theta);
	m_position.y = (float)ENEMY_DIST * sin(m_theta);

	for (auto& bullet : m_bullets) {
		bullet.Update(timeElapsed);
	}
}

void Enemy::SetNextNode(int speed, int note)
{
	m_speed = speed;
	if (note) {
		m_bullets.emplace_back(Bullet(m_position, note));
	}
}
