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
	enum CHANNELID { BGM, AMBIENT, WAR, MAXCHANNEL };
private:
	SoundManager();
	~SoundManager();

public:
	void Initialize();

	void Release();
public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume = 1.f);
	void PlayBGM(TCHAR* pSoundKey);
	void PlayAMBIENT(TCHAR* pSoundKey);

	void ForcePlayBGM(TCHAR* pSoundKey);
	void ForcePlayAMBIENT(TCHAR* pSoundKey);

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

