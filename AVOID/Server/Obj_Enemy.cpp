#include "Obj_Enemy.h"

Enemy::Enemy() {
	m_fBeatCount = 0;
	m_nBullet = 0;
	m_position.x = 0;
	m_position.y = 0;
	theta = 0;
}

// ȭ���� �߾��� 0, 0���� ����. �׷��� �ϸ� Ŭ���̾�Ʈ ũ�⿡ ��� ���� �׸� �� ����.

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
	theta = location * pi / 6;
	m_position.x = ENEMY_DIST * cos(theta);
	m_position.y = ENEMY_DIST * sin(theta);
}

void Enemy::Update(float fElapsedTime, int lotateSpeed, int note) {
	float SpeedData = (float)lotateSpeed;

	theta += SpeedData * pi * 1 / 170 * (fElapsedTime * (680 / 60));
	m_position.x = ENEMY_DIST * cos(theta);
	m_position.y = ENEMY_DIST * sin(theta);

	if (note) {
		m_bullets.push_back(Bullet());
	}
	for (auto& bullet : m_bullets) {
		
	}
}