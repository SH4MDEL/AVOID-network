#pragma once

#include "stdafx.h"

class Sound
{
public:
	enum SoundTag {
		MainSound
		, bbkkbkk
		, TrueBlue
		, Select
		, PlayerWaiting
		, MusicStart
		, GameStart
		, Hitsound
		, Count			// Sound�� ������ ��Ÿ���� ���� �ӽ� �±��̴�. ������ ����.
	};

	Sound();
	~Sound();

	void init();
	void loading();
	void play(int _type);
	void stop(int _chNum);
	void Release();
	void update(int _chNum);
	bool isPlaying;

private:
	FMOD::System*		pFmod;					 // ���� �ý��� ����
	FMOD::Sound*		Music[Count];            // ���� ����
	FMOD::Channel*		ch[Count];				 // ä�κ���
	FMOD::ChannelGroup*	chGroup[Count];
	FMOD_RESULT			r;
};

