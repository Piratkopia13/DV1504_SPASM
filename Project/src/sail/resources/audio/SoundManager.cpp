#include "SoundManager.h"

SoundManager::SoundManager() {
	m_audioEngine = std::make_unique<DirectX::AudioEngine>();
	m_retryAudio = false;

	m_soundEffects.resize(SoundEffect::NumOfSoundEffects);
	m_ambientSound.resize(AmbientSound::NumOfAmbientSounds);
	m_ambientSoundInstances.resize(AmbientSound::NumOfAmbientSounds);

	/* TESTING */
	//m_soundEffects[SoundEffect::Explosion] = std::make_unique<DirectX::SoundEffect>(m_audioEngine.get(), L"explo1.wav");
	loadSoundEffect(SoundEffect::Explosion, L"explo1.wav");
	//m_ambientSound[AmbientSound::Ambient] = std::make_unique<DirectX::SoundEffect>(m_audioEngine.get(), L"NightAmbienceSimple_02.wav");
	loadAmbientSound(AmbientSound::Night, L"NightAmbienceSimple_02");
	playAmbientSound(AmbientSound::Night, true);
	/* TESTING */
}

SoundManager::~SoundManager() {
	if (m_audioEngine)
		m_audioEngine->Suspend();
}

void SoundManager::update(const float dt) {
	if (m_retryAudio) {
		if (m_audioEngine->Reset()) {
			m_retryAudio = false;
			// Restart the soundloops here
		}
		else {
			Logger::Warning("Audio Engine did not manage to reset!");
		}
	}
	else if (!m_audioEngine->Update()) {
		if (m_audioEngine->IsCriticalError()) {
			Logger::Warning("Critical error in the DX Audio Engine!");
			m_retryAudio = true;
		}
	}
}

void SoundManager::playSoundEffect(const SoundEffect soundID) {

	if (soundID < 0 || soundID >= SoundEffect::NumOfSoundEffects) {
		Logger::Warning("Failed to play soundeffect since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	if(m_soundEffects[soundID])
		m_soundEffects[soundID]->Play();
}

void SoundManager::playAmbientSound(const AmbientSound soundID, bool looping) {

	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to play ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	if(m_ambientSoundInstances[soundID])
		if(m_ambientSoundInstances[soundID]->GetState() != DirectX::SoundState::STOPPED)
			m_ambientSoundInstances[soundID]->Play(looping);
}

void SoundManager::pauseAmbientSound(const AmbientSound soundID) {

	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to pause ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	if (m_ambientSoundInstances[soundID])
		if (m_ambientSoundInstances[soundID]->GetState() == DirectX::SoundState::PLAYING)
			m_ambientSoundInstances[soundID]->Pause();
}

void SoundManager::resumeAmbientSound(const AmbientSound soundID) {

	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to resume ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	if (m_ambientSoundInstances[soundID])
		if (m_ambientSoundInstances[soundID]->GetState() == DirectX::SoundState::PAUSED)
			m_ambientSoundInstances[soundID]->Resume();
}

void SoundManager::suspendAllSound() {
	m_audioEngine->Suspend();
}

void SoundManager::resumeAllSound() {
	m_audioEngine->Resume();
}

bool SoundManager::loadSoundEffect(const SoundEffect soundID, const wchar_t* file) {

	m_soundEffects[soundID] = std::make_unique<DirectX::SoundEffect>(m_audioEngine.get(), file);
	if (!m_soundEffects[soundID]) {
		Logger::Warning("Failed to load soundeffect with id: " + soundID);
		return false;
	}

	return true;
}

bool SoundManager::loadAmbientSound(const AmbientSound soundID, const wchar_t* file) {
	
	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to load ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return false;
	}

	m_ambientSound[soundID] = std::make_unique<DirectX::SoundEffect>(m_audioEngine.get(), file);
	if (!m_ambientSound[soundID]) {
		Logger::Warning("Failed to load ambient sound with id: " + soundID);
		return false;
	}

	m_ambientSoundInstances[soundID] = m_ambientSound[soundID]->CreateInstance();
	if (!m_ambientSoundInstances[soundID]) {
		Logger::Warning("Failed to create an instance of soundID: " + soundID);
		return false;
	}

	return true;
}