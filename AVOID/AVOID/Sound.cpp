#include "stdafx.h"
#include "Sound.h"

Sound::Sound()
{
}


Sound::~Sound()
{
}

void Sound::init()
{
	r = FMOD::System_Create(&pFmod);
	r = pFmod->init(100, FMOD_INIT_NORMAL, NULL);
}

void Sound::loading()
{
	r = pFmod->createSound("Sound\\MainSound.mp3", FMOD_LOOP_NORMAL, NULL, &Music[MainSound]);
	r = pFmod->createSound("Sound\\bbkkbkk.mp3", FMOD_LOOP_NORMAL, NULL, &Music[bbkkbkk]);
	r = pFmod->createSound("Sound\\TrueBlue.mp3", FMOD_LOOP_NORMAL, NULL, &Music[TrueBlue]);
	r = pFmod->createSound("Sound\\Select.mp3", FMOD_DEFAULT, NULL, &Music[Select]);
	r = pFmod->createSound("Sound\\MusicStart.mp3", FMOD_DEFAULT, NULL, &Music[MusicStart]);
	r = pFmod->createSound("Sound\\GameStart.mp3", FMOD_DEFAULT, NULL, &Music[GameStart]);
	r = pFmod->createSound("Sound\\Hitsound.ogg", FMOD_DEFAULT, NULL, &Music[Hitsound]);
}

void Sound::play(int _type)
{
	r = pFmod->playSound(Music[_type], chGroup[_type], false, &ch[_type]);		// �뷡�� �ȳ����������� ����
}

void Sound::stop(int _chNum)
{
	ch[_chNum]->stop();
}

void Sound::Release()
{
	Music[SoundTag::MainSound]->release();			// ���� ����
	Music[SoundTag::bbkkbkk]->release();
	Music[SoundTag::TrueBlue]->release();
	Music[SoundTag::Select]->release();
	Music[SoundTag::MusicStart]->release();
	Music[SoundTag::GameStart]->release();
	Music[SoundTag::Hitsound]->release();
	pFmod->close();									// ���� �ý��� ����
	pFmod->release();
}

void Sound::update(int _chNum)						// ��������� �˻��ϰ� ������̸� �ý����� ������Ʈ��
{
	chGroup[_chNum]->isPlaying(&isPlaying);
	if (isPlaying)
		pFmod->update();
}