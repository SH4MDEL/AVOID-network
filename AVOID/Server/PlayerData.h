#include "stdafx.h"
#include "Protocol.h"

class PlayerStatus {
private:
	Coord x, y;
	char playerID;
	bool isSkill;
public:
	PlayerStatus();
	PlayerStatus(short x, short y, char playerID, bool isSkill);
};