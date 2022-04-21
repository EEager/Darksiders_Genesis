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

		WAR, 
		WAR_ATK, WAR_DASH, WAR_FLAME, WAR_BLADEGEYSER, WAR_STEP,

		RUIN, 
		RUIN_STEP1, RUIN_STEP2, RUIN_STEP3,

		LEGION, 
		LEGION_ATK1, LEGION_ATK2, LEGION_ATK3, LEGION_ATK4, LEGION_ATK5, LEGION_ATK6, LEGION_ATK7,
		LEGION_VO1, LEGION_VO2, LEGION_VO3, LEGION_VO4,
		LEGION_DEATH, LEGION_KNOCKBACK, LEGION_LAND, LEGION_TAUNT,

		MON_DEATH1, MON_DEATH2, MON_DEATH3, MON_DEATH4, MON_DEATH5, MON_DEATH6, MON_DEATH7, MON_DEATH8, 

		GOBLIN1, GOBLIN2, GOBLIN3, GOBLIN4, GOBLIN5,

		FALLENDOG, FALLENDOG_ATK, FALLENDOG_VO, 

		HOLLOLORD, 

		BREAKABLE1, BREAKABLE2, BREAKABLE3, BREAKABLE4, BREAKABLE5, BREAKABLE6, BREAKABLE7,

		IMPACT1, IMPACT2, IMPACT3, IMPACT4, IMPACT5, IMPACT6, IMPACT7,
		
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
	// 사운드 리소스 정보를 갖는 객체 
	map<TCHAR*, FMOD_SOUND*> m_mapSound;
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL];
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD_SYSTEM* m_pSystem;
};

