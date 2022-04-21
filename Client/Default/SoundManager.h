#pragma once

#include "Engine_Defines.h"
#include <io.h>
#include "fmod.h"
#pragma comment (lib, "fmod64_vc.lib")

class SoundManager 
{
public:
	static SoundManager* Get_Instance()
	{
		if (nullptr == m_pInstance)
			m_pInstance = new SoundManager;

		return m_pInstance;
	}
	static void Destroy_Instance()
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}
public:
	enum CHANNELID { BGM, AMBIENT, 

		WAR, WAR_ATK, WAR_DASH, WAR_FLAME, WAR_BLADEGEYSER, WAR_STEP,

		RUIN, RUIN_STEP1, RUIN_STEP2, RUIN_STEP3,
		
		MAXCHANNEL };
private:
	SoundManager();
	~SoundManager();

public:
	void Initialize();

	void Release();
public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume = 1.f);
	void PlayBGM(TCHAR* pSoundKey);
	void PlayAMBIENT(TCHAR* pSoundKey, float fVolume = 1.f);

	void ForcePlayBGM(TCHAR* pSoundKey);
	void ForcePlayAMBIENT(TCHAR* pSoundKey, float fVolume = 1.f);

	void ForcePlay(TCHAR* pSoundKey, CHANNELID eID, float v = 1.f);

	void StopSound(CHANNELID eID);
	void StopAll();

	bool GetIsPlaying(CHANNELID eID);

	void SetChanelVolume(CHANNELID eID, float volume);

	_float GetPlayTime(CHANNELID eID);

private:
	void LoadSoundFile();

private:
	static SoundManager* m_pInstance;
	// ���� ���ҽ� ������ ���� ��ü 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem;
};

