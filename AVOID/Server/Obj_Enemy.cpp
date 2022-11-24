#include "Obj_Enemy.h"

Enemy::Enemy() {

}

// 화면의 중앙은 0, 0으로 고정. 그렇게 하면 클라이언트 크기에 상관 없이 그릴 수 있음.

Enemy::Enemy(int location) {
	OnCreate(location);
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