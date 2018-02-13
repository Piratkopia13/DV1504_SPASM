#pragma once

#include "../../Sail.h"

class SoundManager {

public:
	enum SoundEffect {
		Explosion,
		NumOfSoundEffects
	};

	enum AmbientSound {
		Night,
		NumOfAmbientSounds
	};


public:
	SoundManager();
	~SoundManager();

	void update(const float dt);

	void playSoundEffect(const SoundEffect soundID);
	void playAmbientSound(const AmbientSound soundID, bool looping = false);
	void pauseAmbientSound(const AmbientSound soundID);
	void resumeAmbientSound(const AmbientSound soundID);
	void suspendAllSound();
	void resumeAllSound();
	bool loadSoundEffect(const SoundEffect soundID, const wchar_t* file);
	bool loadAmbientSound(const AmbientSound soundID, const wchar_t* file);

private:
	std::vector<std::unique_ptr<DirectX::SoundEffect>> m_soundEffects;
	std::vector<std::unique_ptr<DirectX::SoundEffect>> m_ambientSound;
	std::vector<std::unique_ptr<DirectX::SoundEffectInstance>> m_ambientSoundInstances;
	std::unique_ptr<DirectX::AudioEngine> m_audioEngine;

	bool m_retryAudio;
};