#include "PlayerData.h"

CPlayerData::CPlayerData() {

	// 플레이어 데이터를 안 받았을 때 초기화

	playerSocket = NULL;
	playerState = PLAYER_STATE::NONE;
	selectedMusic = SELECTED_MUSIC::NONE;
	playerId = 0;
	position.x = 0;
	position.y = 0;
	isSkill = FALSE;
	isStatusChanged = FALSE;
	isInvincible = FALSE;
	isCollide = FALSE;
	AttackedTime = 0.0f;
	hp = -1;

}

CPlayerData::CPlayerData(SOCKET sock, PLAYER_STATE state, SELECTED_MUSIC music, int id) {

	// 플레이어 데이터를 받았을 때 초기화

	playerSocket = sock;
	playerState = state;
	selectedMusic = music;
	playerId = id;
	position.x = 0;
	position.y = 0;
	isSkill = FALSE;
	isStatusChanged = FALSE;
	isInvincible = FALSE;
	isCollide = FALSE;
	AttackedTime = 0.0f;
	hp = -1;

}

ServerSharedData::ServerSharedData() {

	// 최초로 서버가 생성되었을 때


	m_pPlayers.clear();
	m_pEnemies.clear();

	time = 0;
	TimeDelay = -0.8f;
	nextPacket = NULL;
	nextPacketPlayerId = NULL;
	musicStart = false;
}

void ServerSharedData::PlayerJoin(SOCKET sock, char* dataBuf) {
	
	// 플레이어가 PlayerWaiting Scene에 들어왔을 때 실행
	char* selected = reinterpret_cast<char*>(dataBuf);

	// selected가 널포인터면 돌려줌.
	if (selected == nullptr) {
		return;
	}

	// 실제 받은 음악 번호로 변경
	char num = (*selected);

	// 받은 음악 번호가 0 또는 1 즉, 실제로 있는 음악의 주소가 아닌 경우 에러
	if (num != 0 && num != 1)
	{
		std::cout << "Error in PlayerJoin" << std::endl;
		return;
	}

	// ID 결정
	int newId = 0;

	for (int i = 0; i < MAX_USER; ++i) {

		bool isIdOccupied = false;

		for (auto& player : m_pPlayers) {
			if (player.playerId == i) {
				isIdOccupied = true;
				break;
			}
		}

		if (!isIdOccupied)
		{
			newId = i;
			break;
		}

	}

	std::cout << "새 ID 는 " << newId << "입니다." << std::endl;
	// 플레이어 데이터 생성

	CPlayerData newPlayer;

	if (num == 0)
	{
		newPlayer = CPlayerData(sock, PLAYER_STATE::PLAYER_WAITING, SELECTED_MUSIC::BBKKBKK, newId);
		
	}
	else if (num == 1)
	{
		newPlayer = CPlayerData(sock, PLAYER_STATE::PLAYER_WAITING, SELECTED_MUSIC::TRUE_BLUE, newId);
	}

	m_pPlayers.push_back(newPlayer);

	// 다음에 보낼 패킷 설정

	nextPacket = SC_PACKET_LOGIN_CONFIRM;
	nextPacketPlayerId = newId;
}

void ServerSharedData::PlayerLeft(SOCKET sock) {

	//플레이어가 사라졌음을 파악했을 때에 실행

	// 해당 소켓에 해당하는 플레이어 데이터를 지운다
	// 소켓은 UINT_PTR 타입이므로, 비교 연산을 통해 비교 가능
	m_pPlayers.erase(remove_if(m_pPlayers.begin(), m_pPlayers.end(), [=](const CPlayerData& a) {
		return a.playerSocket == sock;
		}), m_pPlayers.end());

}


void ServerSharedData::UpdatePlayerStatus(SOCKET sock, char* dataBuf)
{

	// 데이터를 받았을 때

	PlayerStatusByPacket* packet = reinterpret_cast<PlayerStatusByPacket*>(dataBuf);

	for (auto& player : m_pPlayers) {
		if (packet->playerID == player.playerId)
		{
			player.position = packet->position;
			player.isSkill = packet->isSkill;
			player.isStatusChanged = TRUE;
			std::cout << player.playerId << "번 플레이어 현재 상태 데이터 적용" << std::endl;
			break;
		}
	}

	// 모든 플레이어에게서 스테이터스를 받으면 충돌 체크 스레드를 생성해 충돌처리를 실행한다.
	// while은 계속해서 Context Switching이 일어나게 되므로 좋지 않음. 그러나 마땅한 해결책이 떠오르지 않으므로 일단은 while로 멈춰둔다.

	// 문제를 알게 되었다. 이미 CriticalSection에 들어가 있으므로 영원히 첫 스레드에서만 돌게 된다.
	// 그러니까 Apply 단계에서는 데이터를 변경하는 것만 하는 것이 맞는 것으로 보인다.
	// 그리고 스레드를 만들 때에 해당 충돌체크를 하는 것이 맞는지 확인하여 충돌체크의 순서를 도는것이 맞는듯 하다.
	// 그러니까 항상 스레드를 만드는 것으로 해야겠다.

	nextPacket = SC_PACKET_OBJECTS_INFO;
	nextPacketPlayerId = packet->playerID;

	ResetEvent(hClientEvent);
	SetEvent(hCollideEvent);
	CreateThread(NULL, 0, Collision_Thread, NULL, 0, NULL);

}

// 모든 플레이어가 다 한번 스테이터스를 바꿨는지 확인.
bool ServerSharedData::CheckAllPlayerStatusReady() {
	for (auto& i : m_pPlayers) {
		if (!(i.isStatusChanged)) {
			return false;
		}
	}

	std::cout << "충돌 체크 이벤트 생성 가능" << std::endl;
	for (auto& i : m_pPlayers) {
		i.isStatusChanged = false;
	}
	return true;
}

void ServerSharedData::CreateNewGame(char* dataBuf) {

	int musicNum = 0;

	if (!musicStart) {
		

		if (dataBuf == NULL) {

			musicNum = 0;

		}
		else {
			char data = (*dataBuf);

			for (auto& player : m_pPlayers) {
				if (player.playerId == data) {
					switch (player.selectedMusic) {
					case SELECTED_MUSIC::BBKKBKK:
					{
						musicNum = 0;
						music = SELECTED_MUSIC::BBKKBKK;
					}
					break;
					case SELECTED_MUSIC::TRUE_BLUE:
					{
						musicNum = 1;
						music = SELECTED_MUSIC::TRUE_BLUE;
					}
					break;
					}
				}
			}
		}

		char Inbuff[3000];
		DWORD read_size = 3000;
		DWORD c = 3000;

		switch (musicNum) {
		case 0:
			hPlayingMusicSpeedFile = CreateFile(L"Data\\bbkkbkkSpeed.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			break;
		case 1:
			hPlayingMusicSpeedFile = CreateFile(L"Data\\TrueBlueSpeed.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			break;
		}
		ReadFile(hPlayingMusicSpeedFile, Inbuff, c, &read_size, NULL);
		CloseHandle(hPlayingMusicSpeedFile);

		int num = 0;

		for (int i = 0; i < 3000; ++i) {
			if (Inbuff[i] >= 48 && Inbuff[i] <= 57) {
				lotateSpeed[num] = Inbuff[i] - 48;
				++num;
			}
		}

		char noteInbuff[36000];
		DWORD read_note = 36000;
		DWORD n = 36000;

		switch (musicNum) {
		case 0:
			hPlayingMusicNoteFile = CreateFile(L"Data\\bbkkbkkNote.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			break;
		case 1:
			hPlayingMusicNoteFile = CreateFile(L"Data\\TrueBlueNote.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			break;
		}
		ReadFile(hPlayingMusicNoteFile, noteInbuff, n, &read_note, NULL);
		CloseHandle(hPlayingMusicNoteFile);

		int numI = 0, numJ = 0;

		for (int i = 0; i < 36000; ++i) {
			if (noteInbuff[i] >= 48 && noteInbuff[i] <= 57) {
				note[numI][numJ] = noteInbuff[i] - 48;
				if (numJ == 11) {
					numJ = 0, numI++;
				}
				else {
					numJ++;
				}
			}
		}

		for (int i = 0; i < NUMBER_OF_ENEMY; ++i)
		{
			m_pEnemies.emplace_back(Enemy(i));
		}

		nextPacket = SC_PACKET_START_GAME;
		nextPacketPlayerId = NO_NEED_PLAYER_ID;

		musicStart = true;
	}
}

int ServerSharedData::GetPlayerRank(SOCKET sock, char* dataBuf) {
	
	char Id = dataBuf[0];
	char hp = dataBuf[1];

	for (auto& player : m_pPlayers) {
		if (player.playerId == Id) {
			player.hp = hp;
		}
	}

	for (auto& player : m_pPlayers) {
		if (player.hp == -1) {
			nextPacket = NULL;
			nextPacketPlayerId = NULL;
			return -1;
		}
	}


	std::sort(m_pPlayers.begin(), m_pPlayers.end(), [](const CPlayerData& a, const CPlayerData& b) {
		return a.hp > b.hp;
		});


	nextPacket = SC_PACKET_RANK;
	nextPacketPlayerId = NULL;

	return 0;
}


int ServerSharedData::GetBulletNum() {
	
	int sum = 0;
	for (auto& enemy : m_pEnemies) {
		sum += enemy.GetBullets().size();
	}

	return sum;
}

void ServerSharedData::Update(float fTimeElapsed) {

	TimeDelay += fTimeElapsed;
	if (TimeDelay >= 0.f) {
		leastTime += fTimeElapsed;

		if (music == SELECTED_MUSIC::BBKKBKK) {
			if (TimeDelay >= (60.f / 680.f) * (float)time) {
				int i = 0;
				for (auto& enemy : m_pEnemies) {
					enemy.SetNextNote(leastTime, lotateSpeed[time], note[time][i], 0);
					++i;
				}
				++time;
				leastTime = 0.f;
			}
		}
		else if (music == SELECTED_MUSIC::TRUE_BLUE) {
			if (TimeDelay >= (60.f / 656.f) * (float)time) {
				int i = 0;
				for (auto& enemy : m_pEnemies) {
					enemy.SetNextNote(leastTime, lotateSpeed[time], note[time][i], 1);
					++i;
				}
				++time;
				leastTime = 0.f;
			}
		}	
	}

	for (auto& player : m_pPlayers) {
		if (player.isInvincible) {
			if (TimeDelay - player.AttackedTime > 2.0f) {
				player.isInvincible = false;
			}
		}
	}

	for (auto& enemy : m_pEnemies) {
		enemy.Update(fTimeElapsed);
	}
	

	if ((music == SELECTED_MUSIC::BBKKBKK && time >= 1450) || (music == SELECTED_MUSIC::TRUE_BLUE && time >= 1310)) {
		nextPacket = SC_PACKET_MUSIC_END;
		nextPacketPlayerId = NO_NEED_PLAYER_ID;
		musicStart = false;
	}
	else {
		nextPacket = SC_PACKET_OBJECTS_INFO;
		nextPacketPlayerId = NO_NEED_PLAYER_ID;
	}

}
