#pragma once

#include <XAudio2.h>
#include "../../Sail.h"

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif

class Sound;
class SoundManager {

public:
	enum SoundEffect {
		Explosion,
		Windows95,
		Shock,
		Laser,
		Loop1,
		Loop2,
		Male_Death,
		Goblin_Death,
		NumOfSoundEffects
	};

	enum AmbientSound {
		Night,
		NumOfAmbientSounds
	};

	const static int NUMBER_OF_CHANNELS = 32;


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

	bool loadSoundEffect(const SoundEffect soundID, wchar_t* file);
	bool loadAmbientSound(const AmbientSound soundID, wchar_t* file);

	void setVolume(const float& volume);
	float getVolume();

private:
	IXAudio2* m_audioEngine;
	IXAudio2MasteringVoice* m_masterVoice;
	std::vector<IXAudio2SourceVoice*> m_sourceVoices;
	std::vector<IXAudio2SubmixVoice*> m_submixVoices;

	std::vector<std::unique_ptr<Sound>> m_sounds;

	bool m_retryAudio;
	int	m_currSVIndex;

};




/////////////////////////////////
///////    SOUND CLASS    ///////
/////////////////////////////////

class Sound {

public:
	Sound();
	~Sound();

	HRESULT Initialize(IXAudio2* audioEngine, wchar_t* file);

	HRESULT Play();
	XAUDIO2_BUFFER getBuffer();
	WAVEFORMATEXTENSIBLE getWFX();

private:
	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset);

private:
	XAUDIO2_BUFFER m_buffer;
	WAVEFORMATEXTENSIBLE m_WFX;
	

};