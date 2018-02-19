#include "SoundManager.h"

#include <xaudio2.h>

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

	HRESULT hr;
	if (FAILED(hr = XAudio2Create(&m_audioEngine, 0, XAUDIO2_DEFAULT_PROCESSOR))) {
		Logger::Error("Failed to initialize the audio engine.");
	}

	if (FAILED(hr = m_audioEngine->CreateMasteringVoice(&m_masterVoice))) {
		Logger::Error("Failed to encapsulate the audio device.");
	}

	m_audioEngine->StartEngine();

	m_sourceVoices.resize(NUMBER_OF_CHANNELS);
	for (int i = 0; i < NUMBER_OF_CHANNELS; i++)
		m_sourceVoices[i] = nullptr;
	m_currSVIndex = 0;

	m_sounds.resize(SoundEffect::NumOfSoundEffects);

	loadSoundEffect(SoundEffect::Explosion, L"res/sounds/effect/explosion.wav");
	loadSoundEffect(SoundEffect::Windows95, L"res/sounds/windows95.wav");
	loadSoundEffect(SoundEffect::Laser, L"res/sounds/effect/laser.wav");
	loadSoundEffect(SoundEffect::Shock, L"res/sounds/effect/shock.wav");
	loadSoundEffect(SoundEffect::Loop1, L"res/sounds/ambient/loop1.wav");
	loadSoundEffect(SoundEffect::Loop2, L"res/sounds/ambient/loop2.wav");
	loadSoundEffect(SoundEffect::Male_Death, L"res/sounds/effect/death/male_death.wav");
	loadSoundEffect(SoundEffect::Goblin_Death, L"res/sounds/effect/death/goblin_death.wav");
}

SoundManager::~SoundManager() {

	/*for (int i = 0; i < NUMBER_OF_CHANNELS; i++) {
		if (m_sourceVoices[i]) {
			m_sourceVoices[i]->Discontinuity();
			m_sourceVoices[i]->DestroyVoice();
		}
	}*/

	//m_masterVoice->DestroyVoice();

	m_audioEngine->StopEngine();
	SAFE_RELEASE(m_audioEngine);

	CoUninitialize();
}

void SoundManager::update(const float dt) {
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

void SoundManager::playSoundEffect(const SoundEffect soundID) {

	if (soundID < 0 || soundID >= SoundEffect::NumOfSoundEffects) {
		Logger::Warning("Failed to play soundeffect since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	if(m_sourceVoices[m_currSVIndex])
		m_sourceVoices[m_currSVIndex]->DestroyVoice();

	WAVEFORMATEXTENSIBLE wfx = m_sounds[soundID]->getWFX();
	XAUDIO2_BUFFER buffer = m_sounds[soundID]->getBuffer();
	m_audioEngine->CreateSourceVoice(&m_sourceVoices[m_currSVIndex], (WAVEFORMATEX*)&wfx);
	if (soundID == SoundManager::SoundEffect::Laser)
		m_sourceVoices[m_currSVIndex]->SetVolume(0.2f);
	else
		m_sourceVoices[m_currSVIndex]->SetVolume(1.0f);

	m_sourceVoices[m_currSVIndex]->SubmitSourceBuffer(&buffer);
	float pitch = Utils::rnd() * 0.4f + 0.5f;
	m_sourceVoices[m_currSVIndex]->SetFrequencyRatio(pitch);
	m_sourceVoices[m_currSVIndex]->Start(0);
	//m_sourceVoices[m_currSVIndex]->SetFrequencyRatio(1.f);
	m_currSVIndex++;
	m_currSVIndex = m_currSVIndex % NUMBER_OF_CHANNELS;

	//m_sourceVoices[m_currSVIndex] = m_audioEngine->crea m_sounds[soundID]->getSourceVoice();


	//if (m_sounds[soundID])
	//	m_sounds[soundID]->Play();

	/*if(m_soundEffects[soundID])
		m_soundEffects[soundID]->Play();*/
}

void SoundManager::playAmbientSound(const AmbientSound soundID, bool looping) {

	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to play ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	/*if(m_ambientSoundInstances[soundID])
		if(m_ambientSoundInstances[soundID]->GetState() != DirectX::SoundState::STOPPED)
			m_ambientSoundInstances[soundID]->Play(looping);*/
}

void SoundManager::pauseAmbientSound(const AmbientSound soundID) {

	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to pause ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	/*if (m_ambientSoundInstances[soundID])
		if (m_ambientSoundInstances[soundID]->GetState() == DirectX::SoundState::PLAYING)
			m_ambientSoundInstances[soundID]->Pause();*/
}

void SoundManager::resumeAmbientSound(const AmbientSound soundID) {

	if (soundID < 0 || soundID >= AmbientSound::NumOfAmbientSounds) {
		Logger::Warning("Failed to resume ambient sound since sound id was out of bounds. ID tried: " + soundID);
		return;
	}

	/*if (m_ambientSoundInstances[soundID])
		if (m_ambientSoundInstances[soundID]->GetState() == DirectX::SoundState::PAUSED)
			m_ambientSoundInstances[soundID]->Resume();*/
}

void SoundManager::suspendAllSound() {
	/*m_audioEngine->Suspend();*/
}

void SoundManager::resumeAllSound() {
	/*m_audioEngine->Resume();*/
}

bool SoundManager::loadSoundEffect(const SoundEffect soundID, wchar_t* file) {

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
	m_masterVoice->SetVolume(volume);
}

float SoundManager::getVolume() {
	float volume;
	m_masterVoice->GetVolume(&volume);

	return volume;
}


/////////////////////////////////
/////// PRIVATE FUNCTIONS ///////
/////////////////////////////////
















/////////////////////////////////
///////    SOUND CLASS    ///////
/////////////////////////////////

Sound::Sound() { }
Sound::~Sound() {
	delete m_buffer.pAudioData;
}


HRESULT Sound::Initialize(IXAudio2* audioEngine, wchar_t* file) {

	////////////////////////////////////////////
	//// Populating XAudio2 structures with ////
	////    the contents of RIFF chunks	    ////
	////////////////////////////////////////////

	// Declare WAVEFORMATEXTENSIBLE and XAUDIO2_BUFFER structures.
	m_WFX = { 0 };
	m_buffer = { 0 };

	// Open the audio file with CreateFile.
	TCHAR * strFileName = file;

	// Open the file
	HANDLE hFile = CreateFile(
		strFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	// Locate the 'RIFF' chunk in the audio file, and check the file type.
	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	if (filetype != fourccWAVE)
		return S_FALSE;



	// Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
	FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	ReadChunkData(hFile, &m_WFX, dwChunkSize, dwChunkPosition);



	// Locate the 'data' chunk, and read its contents into a buffer.
	//fill out the audio data buffer with the contents of the fourccDATA chunk
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	BYTE * pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);



	// Populate an XAUDIO2_BUFFER structure.
	m_buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
	m_buffer.pAudioData = pDataBuffer;  //buffer containing audio data
	m_buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer


	// Gets the length in seconds of the audio
	m_playtime = float(m_buffer.AudioBytes) / float(m_WFX.Format.nAvgBytesPerSec);

	return S_OK;
}


HRESULT Sound::FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition) {
	
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK) {
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType) {
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc) {
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;
}


HRESULT Sound::ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset) {
	HRESULT hr = S_OK;

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());

	return hr;
}

XAUDIO2_BUFFER Sound::getBuffer() {
	return m_buffer;
}

WAVEFORMATEXTENSIBLE Sound::getWFX() {
	return m_WFX;
}

HRESULT Sound::Play() {
	HRESULT hr = NULL;

	// Use the Start function to start the source voice. 
	// Since all XAudio2 voices send their output to the mastering voice by default, 
	//  audio from the source voice automatically makes its way to the audio device selected at initialization. 
	// In a more complicated audio graph, the source voice would have to specify the voice to which its 
	//  output should be sent
	// Submit an XAUDIO2_BUFFER to the source voice using the function SubmitSourceBuffer.
	//if (FAILED(hr = m_sourceVoice->SubmitSourceBuffer(&m_buffer)))
	//	return hr;

	return hr;
}