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
		, Count			// Sound의 개수를 나타내기 위한 임시 태그이다. 지우지 말자.
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
	FMOD::System*		pFmod;					 // 사운드 시스템 변수
	FMOD::Sound*		Music[Count];            // 사운드 변수
	FMOD::Channel*		ch[Count];				 // 채널변수
	FMOD::ChannelGroup*	chGroup[Count];
	FMOD_RESULT			r;
};

