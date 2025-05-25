#pragma once

#include <SDL3_mixer/SDL_mixer.h>
#include <iostream>
#include <vector>

enum class AudioEnum
{
	Music = 0,     // I
	ShotgunShoot,  // I I
	ShotgunReload, // I I
	EnemyKilled,   // I I
	AmmoPickedUp,  // I I
	KeyPickedUp,   // I I
	GameOver,      // I I
	AUDIO_ENUM_COUNT = 7
};

class AudioManager
{
public:
	static AudioManager& GetInstance();
	bool Init();
	void Destroy();

	bool LoadAudio(AudioEnum audioID);
	bool LoadAllAudio();
	void UnloadAudio(AudioEnum audioID);
	void UnloadAllAudio();

	int Play(AudioEnum audioID);
	void Stop(int channel);
	void Stop(AudioEnum audioID);
	void StopAll();

private:
	AudioManager()  = default;//: m_audioChunks{};
	~AudioManager() = default;

	Mix_Chunk* m_audioChunks[static_cast<int>(AudioEnum::AUDIO_ENUM_COUNT)];

private:
	std::string GetAudioFilepath(AudioEnum audioID) const;

	// Prevent copy and assignment
	AudioManager(const AudioManager&)            = delete;
	AudioManager& operator=(const AudioManager&) = delete;
};