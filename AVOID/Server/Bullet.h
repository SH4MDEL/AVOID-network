#pragma once
#include "Object.h"

constexpr int BULLET_RADIUS = 18;
constexpr int BULLET_SPEED = 15;

class Bullet : public Object
{
public:
	Bullet(Coord position, int direction);
	~Bullet() = default;

	void Update(float timeElapsed) override;

	int GetSpeed() { return m_speed; }

	void SetSpeed(int speed) { m_speed = speed; }

private:
	int			m_speed;
	int			m_direction;
};

