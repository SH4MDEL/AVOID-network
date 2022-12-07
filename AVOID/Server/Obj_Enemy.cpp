#include "Obj_Enemy.h"

// 화면 중앙은 0,0 으로 하기로 했었음.


Enemy::Enemy() {
	m_fBeatCount = 0;
	m_nBullet = 0;
	m_position.x = 0;
	m_position.y = 0;
	theta = 0;
}

Enemy::Enemy(int location) {
	m_fBeatCount = 0;
	m_nBullet = 0;
	m_position.x = 0;
	m_position.y = 0;
	theta = 0;
	OnCreate(location);
}

Enemy::~Enemy() {
	m_fBeatCount = 0;
	m_nBullet = 0;
	m_position.x = 0;
	m_position.y = 0;
	theta = 0;
}

void Enemy::OnCreate(int location) {
	theta = (float)location * pi / 6.f;
	m_position.x = (float)ENEMY_DIST * cos(theta);
	m_position.y = (float)ENEMY_DIST * sin(theta);
}

void Enemy::SetNextNote(float fElapsedTime, int lotateSpeed, int note, int selectedMusic) {


	float SpeedData = (float)lotateSpeed;

	if (selectedMusic == 0) {
		theta += SpeedData * pi * 1.f / 170.f * (fElapsedTime * (680.f / 60.f));
		m_position.x = (float)ENEMY_DIST * cos(theta);
		m_position.y = (float)ENEMY_DIST * sin(theta);
	}
	else if (selectedMusic == 1) {
		theta += SpeedData * pi * 1.f / 170.f * (fElapsedTime * (656.f / 60.f));
		m_position.x = (float)ENEMY_DIST * cos(theta);
		m_position.y = (float)ENEMY_DIST * sin(theta);
	}

	if (note) {
		m_bullets.emplace_back(Bullet((int)m_position.x, (int)m_position.y, note));
	}
}

void Enemy::Update(float fElapsedTime)
{
	for (auto& bullet : m_bullets) {
		bullet.Update(fElapsedTime);
	}

	m_nBullet = m_bullets.size();
}