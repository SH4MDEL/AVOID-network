#pragma once
#pragma pack(1)

constexpr short		SERVER_PORT					= 9000;
constexpr char*		SERVER_ADDR					= "127.0.0.1";
constexpr int		BUF_SIZE					= 512;
constexpr int		MAX_USER					= 3;

constexpr char		CS_PACKET_LOGIN				= 1;
constexpr char		CS_PACKET_READY				= 2;
constexpr char		CS_PACKET_PLAYER_STATUS		= 3;
constexpr char		CS_PACKET_PLAYER_HP			= 4;
constexpr char		CS_PACKET_LOGOUT			= 100;

constexpr char		SC_PACKET_LOGIN_CONFIRM		= 1;
constexpr char		SC_PACKET_START_GAME		= 2;
constexpr char		SC_PACKET_OBJECTS_INFO		= 3;
constexpr char		SC_PACKET_MUSIC_END			= 4;
constexpr char		SC_PACKET_RANK				= 5;
constexpr char		SC_PACKET_LOGOUT			= 100;

struct packet 
{
	unsigned char size;
	char type;
};

struct Coord
{
	short x, y;
};

struct PlayerStatus
{
	Coord coord;
	char playerID;
	bool isCollide;
	bool isSkill;
};


struct cs_packet_login : public packet
{
	char selectMusic;
};

struct cs_packet_ready : public packet
{
	char playerID;
};

struct cs_packet_player_status : public packet
{
	Coord coord;
	char playerID;
	bool isSkill;
};

struct cs_packet_player_hp : public packet
{
	char playerID;
	char hp;
};

struct cs_packet_logout : public packet
{
	char playerID;
};


struct sc_packet_login_confirm : public packet
{
	char playerID;
};

struct sc_packet_start_game : public packet
{
	char playerNum;
};

struct sc_packet_objects_info : public packet
{
	char playerNum;
	char enemyNum;
	char bulletNum;
};

struct sc_pakcet_music_end : public packet
{
};

struct sc_packet_rank : public packet
{
	char rank;
};


