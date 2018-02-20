#include "SoundManager.h"

#include <xaudio2.h>

#include "Sound.h"
#include "AmbientSound.h"

//--------------------------------------------------------------------------------------
// Helper macros
//--------------------------------------------------------------------------------------
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=nullptr; } }
#endif

/////////////////////////////////
/////// PUBLIC FUNCTIONS ////////
/////////////////////////////////

SoundManager::SoundManager() {
	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	m_playSound = true;

	HRESULT hr;
	if (FAILED(hr = XAudio2Create(&m_audioEngine, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
		Logger::Warning("Failed to initialize the audio engine.");
		m_playSound = false;
	}

	if (FAILED(hr = m_audioEngine->CreateMasteringVoice(&m_masterVoice))) {
		Logger::Warning("Failed to encapsulate the audio device.");
		m_playSound = false;
	}

	if (m_playSound) {

		m_audioEngine->StartEngine();

		m_sourceVoices.resize(NUMBER_OF_CHANNELS);
		for (int i = 0; i < NUMBER_OF_CHANNELS; i++)
			m_sourceVoices[i] = nullptr;
		m_currSVIndex = 0;

		m_sounds.resize(SoundEffect::NumOfSoundEffects);
		m_ambientSounds.resize(AmbientSound::NumOfAmbientSounds);

		loadSoundEffect(SoundEffect::Explosion, L"res/sounds/effect/explosion.wav");
		loadSoundEffect(SoundEffect::Laser, L"res/sounds/effect/laser.wav");
		loadSoundEffect(SoundEffect::Shock, L"res/sounds/effect/shock.wav");
		loadSoundEffect(SoundEffect::Male_Death, L"res/sounds/effect/death/male_death.wav");
		loadSoundEffect(SoundEffect::Goblin_Death, L"res/sounds/effect/death/goblin_death.wav");

		loadAmbientSound(AmbientSound::Windows95, L"res/sounds/ambient/windows95.wav");
		loadAmbientSound(AmbientSound::Loop1, L"res/sounds/ambient/loop1.wav");
		loadAmbientSound(AmbientSound::Loop2, L"res/sounds/ambient/loop2.wav");
	}
}

SoundManager::~SoundManager() {

	m_audioEngine->StopEngine();
	SAFE_RELEASE(m_audioEngine);

	CoUninitialize();
}

void SoundManager::update(const float dt) {

	if (!m_playSound)
		return;

	//if (m_retryAudio) {
	//	if (m_audioEngine->Reset()) {
	//		m_retryAudio = false;
	//		// Restart the soundloops here
	//	}
	//	else {
	//		Logger::Warning("Audio Engine did not manage to reset!");
	//	}
	//}
	//else if (!m_audioEngine->Update()) {
	//	if (m_audioEngine->IsCriticalError()) {
	//		Logger::Warning("Critical error in the DX Audio Engine!");
	//		m_retryAudio = true;
	//	}
	//}
}

void SoundManager::playSoundEffect(const SoundEffect soundID, float volume, float pitch) {
	if (!m_playSound)
		return;

	if (soundID < 0 || soundID >= SoundEffect::NumOfSoundEffects) {
		Logger::Warning("Failed to play soundeffect since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	if (m_sourceVoices[m_currSVIndex])
		m_sourceVoices[m_currSVIndex]->DestroyVoice();

	float pit = min(MAX_PITCH, max(MIN_PITCH, pitch));
	float vol = min(XAUDIO2_MAX_VOLUME_LEVEL, max(-XAUDIO2_MAX_VOLUME_LEVEL, volume));

	WAVEFORMATEXTENSIBLE wfx = m_sounds[soundID]->getWFX();
	XAUDIO2_BUFFER buffer = m_sounds[soundID]->getBuffer();
	m_audioEngine->CreateSourceVoice(&m_sourceVoices[m_currSVIndex], (WAVEFORMATEX*)&wfx);

	m_sourceVoices[m_currSVIndex]->SetVolume(vol);
	m_sourceVoices[m_currSVIndex]->SetFrequencyRatio(pit);

	m_sourceVoices[m_currSVIndex]->SubmitSourceBuffer(&buffer);
	m_sourceVoices[m_currSVIndex]->Start(0);
	m_currSVIndex++;
	m_currSVIndex = m_currSVIndex % NUMBER_OF_CHANNELS;
}

void SoundManager::playAmbientSound(const AmbientSound soundID, bool looping) {
	if (!m_playSound)
		return;

		if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
			Logger::Warning("Failed to play ambient sound since sound id was out of bounds. ID tried: " + soundID);
			return;
		}

		if (m_sourceVoices[m_currSVIndex])
			m_sourceVoices[m_currSVIndex]->DestroyVoice();

		WAVEFORMATEXTENSIBLE wfx = m_ambientSounds[soundID]->getWFX();
		XAUDIO2_BUFFER buffer = m_ambientSounds[soundID]->getBuffer();
		m_audioEngine->CreateSourceVoice(&m_sourceVoices[m_currSVIndex], (WAVEFORMATEX*)&wfx);
		if (soundID == SoundManager::SoundEffect::Laser)
			m_sourceVoices[m_currSVIndex]->SetVolume(0.2f);
		else
			m_sourceVoices[m_currSVIndex]->SetVolume(1.0f);

		m_sourceVoices[m_currSVIndex]->SubmitSourceBuffer(&buffer);
		float pitch = Utils::rnd() * 0.4f + 0.5f;

		// Random stuff
		m_sourceVoices[m_currSVIndex]->SetFrequencyRatio(pitch);

		m_sourceVoices[m_currSVIndex]->Start(0);

		m_currSVIndex++;
		m_currSVIndex = m_currSVIndex % NUMBER_OF_CHANNELS;
	
	
}

void SoundManager::pauseAmbientSound(const AmbientSound soundID) {
	if (!m_playSound)
		return;

	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to pause ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

}

void SoundManager::resumeAmbientSound(const AmbientSound soundID) {
	if (!m_playSound)
		return;

		if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
			Logger::Warning("Failed to resume ambient sound since sound id was out of bounds. ID tried: " + soundID);
			return;
		}
	/*if (m_ambientSoundInstances[soundID])
		if (m_ambientSoundInstances[soundID]->GetState() == DirectX::SoundState::PAUSED)
			m_ambientSoundInstances[soundID]->Resume();*/
}

void SoundManager::suspendAllSound() {
	if (!m_playSound)
		return;
	/*m_audioEngine->Suspend();*/
}

void SoundManager::resumeAllSound() {
	if (!m_playSound)
		return;
	/*m_audioEngine->Resume();*/
}

bool SoundManager::loadSoundEffect(const SoundEffect soundID, wchar_t* file) {
	if (!m_playSound)
		return;

	if (soundID < 0 || soundID >= SoundEffect::NumOfSoundEffects) {
		Logger::Warning("Failed to load sound effect since sound id was out of bounds. ID tried: " + soundID);
		return false;
	}

	m_sounds[soundID] = std::make_unique<Sound>();
	m_sounds[soundID]->Initialize(m_audioEngine, file);
	if (!m_sounds[soundID]) {
		Logger::Warning("Failed to load sound effect with id: " + soundID);
		return false;
	}

	return true;
}

bool SoundManager::loadAmbientSound(const AmbientSound soundID, wchar_t* file) {
	if (!m_playSound)
		return;
	
	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to load ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return false;
	}

	//m_ambientSound[soundID] = std::make_unique<DirectX::SoundEffect>(m_audioEngine.get(), file);
	//if (!m_ambientSound[soundID]) {
	//	Logger::Warning("Failed to load ambient sound with id: " + soundID);
	//	return false;
	//}

	//m_ambientSoundInstances[soundID] = m_ambientSound[soundID]->CreateInstance();
	//if (!m_ambientSoundInstances[soundID]) {
	//	Logger::Warning("Failed to create an instance of soundID: " + soundID);
	//	return false;
	//}

	return true;
}

void SoundManager::setVolume(const float& volume) {
	if (!m_playSound)
		return;

	m_masterVoice->SetVolume(volume);
}

float SoundManager::getVolume() {
	if (!m_playSound)
		return;

	float volume;
	m_masterVoice->GetVolume(&volume);

	return volume;
}


/////////////////////////////////
/////// PRIVATE FUNCTIONS ///////
/////////////////////////////////