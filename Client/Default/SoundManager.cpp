#include "stdafx.h"
#include"fmod.hpp"
#include"fmod_errors.h"
#include "SoundManager.h"
#include <iostream>

SoundManager* SoundManager::m_pInstance = nullptr;
SoundManager::SoundManager()
{
	m_pSystem = nullptr;
}


SoundManager::~SoundManager()
{
	Release();
}

void SoundManager::Initialize()
{
	FMOD_System_Create(&m_pSystem);

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	LoadSoundFile();
}
void SoundManager::Release()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}




void SoundManager::PlaySound(TCHAR * pSoundKey, CHANNELID eID, float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr,FALSE, &m_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);
	FMOD_System_Update(m_pSystem);
}

void SoundManager::PlayBGM(TCHAR * pSoundKey)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;
	
	FMOD_System_PlaySound(m_pSystem,iter->second,nullptr, FALSE, &m_pChannelArr[BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[BGM], FMOD_LOOP_NORMAL);
	FMOD_System_Update(m_pSystem);
}

void SoundManager::PlayAMBIENT(TCHAR* pSoundKey,float fVolume)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[AMBIENT]);
	FMOD_Channel_SetMode(m_pChannelArr[AMBIENT], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[AMBIENT], fVolume);
	FMOD_System_Update(m_pSystem);
}

void SoundManager::ForcePlayBGM(TCHAR* pSoundKey)
{
	FMOD_Channel_Stop(m_pChannelArr[BGM]);
	PlayBGM(pSoundKey);
}

// 앰비언튼은 사운드 조절하도록 하자
void SoundManager::ForcePlayAMBIENT(TCHAR* pSoundKey, float fVolume)
{
	FMOD_Channel_Stop(m_pChannelArr[AMBIENT]);
	PlayAMBIENT(pSoundKey, fVolume);
}


void SoundManager::ForcePlay(TCHAR * pSoundKey, CHANNELID eID, float fVolume)
{
 	FMOD_Channel_Stop(m_pChannelArr[eID]);
	PlaySound(pSoundKey, eID, fVolume);
}

void SoundManager::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void SoundManager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

bool SoundManager::GetIsPlaying(CHANNELID eID)
{
	FMOD_BOOL bPlay = FALSE;
	FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay);
	return (bool)bPlay;
}

void SoundManager::SetChanelVolume(CHANNELID eID,float volume)
{
	FMOD_RESULT hr = FMOD_Channel_SetVolume(m_pChannelArr[eID], volume);
}

_float SoundManager::GetPlayTime(CHANNELID eID)
{
	unsigned int time;
	FMOD_Channel_GetPosition(m_pChannelArr[eID], &time, FMOD_TIMEUNIT_MS);
	 
	return time / 1000.f;
}

void SoundManager::LoadSoundFile()
{
	// 배경
	{
		_tfinddata64_t fd;
		__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/BackGround/*.*", &fd);
		if (handle == -1 || handle == 0)
			return;

		int iResult = 0;

		char szCurPath[128] = "../Bin/Resources/Sound/BackGround/";
		char szFullPath[128] = "";
		char szFilename[MAX_PATH];
		while (iResult != -1)
		{
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, szCurPath);
			strcat_s(szFullPath, szFilename);
			FMOD_SOUND* pSound = nullptr;

			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = (int)strlen(szFilename) + 1;

				TCHAR* pSoundKey = new TCHAR[iLength];
				ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
			iResult = _tfindnext64(handle, &fd);
		}
		FMOD_System_Update(m_pSystem);

		_findclose(handle);
	}

	// War
	{
		_tfinddata64_t fd;
		__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/War/*.*", &fd);
		if (handle == -1 || handle == 0)
			return;

		int iResult = 0;

		char szCurPath[128] = "../Bin/Resources/Sound/War/";
		char szFullPath[128] = "";
		char szFilename[MAX_PATH];
		while (iResult != -1)
		{
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, szCurPath);
			strcat_s(szFullPath, szFilename);
			FMOD_SOUND* pSound = nullptr;

			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = (int)strlen(szFilename) + 1;

				TCHAR* pSoundKey = new TCHAR[iLength];
				ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
			iResult = _tfindnext64(handle, &fd);
		}
		FMOD_System_Update(m_pSystem);

		_findclose(handle);
	}

	// Ruin
	{
		_tfinddata64_t fd;
		__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/Ruin/*.*", &fd);
		if (handle == -1 || handle == 0)
			return;

		int iResult = 0;

		char szCurPath[128] = "../Bin/Resources/Sound/Ruin/";
		char szFullPath[128] = "";
		char szFilename[MAX_PATH];
		while (iResult != -1)
		{
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, szCurPath);
			strcat_s(szFullPath, szFilename);
			FMOD_SOUND* pSound = nullptr;

			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = (int)strlen(szFilename) + 1;

				TCHAR* pSoundKey = new TCHAR[iLength];
				ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
			iResult = _tfindnext64(handle, &fd);
		}
		FMOD_System_Update(m_pSystem);

		_findclose(handle);
	}

	// FallenDog
	{
		_tfinddata64_t fd;
		__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/FallenDog/*.*", &fd);
		if (handle == -1 || handle == 0)
			return;

		int iResult = 0;

		char szCurPath[128] = "../Bin/Resources/Sound/FallenDog/";
		char szFullPath[128] = "";
		char szFilename[MAX_PATH];
		while (iResult != -1)
		{
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, szCurPath);
			strcat_s(szFullPath, szFilename);
			FMOD_SOUND* pSound = nullptr;

			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = (int)strlen(szFilename) + 1;

				TCHAR* pSoundKey = new TCHAR[iLength];
				ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
			iResult = _tfindnext64(handle, &fd);
		}
		FMOD_System_Update(m_pSystem);

		_findclose(handle);
	}


	// General
	{
		_tfinddata64_t fd;
		__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/General/*.*", &fd);
		if (handle == -1 || handle == 0)
			return;

		int iResult = 0;

		char szCurPath[128] = "../Bin/Resources/Sound/General/";
		char szFullPath[128] = "";
		char szFilename[MAX_PATH];
		while (iResult != -1)
		{
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, szCurPath);
			strcat_s(szFullPath, szFilename);
			FMOD_SOUND* pSound = nullptr;

			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = (int)strlen(szFilename) + 1;

				TCHAR* pSoundKey = new TCHAR[iLength];
				ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
			iResult = _tfindnext64(handle, &fd);
		}
		FMOD_System_Update(m_pSystem);

		_findclose(handle);
	}

	// Legion
	{
		_tfinddata64_t fd;
		__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/Legion/*.*", &fd);
		if (handle == -1 || handle == 0)
			return;

		int iResult = 0;

		char szCurPath[128] = "../Bin/Resources/Sound/Legion/";
		char szFullPath[128] = "";
		char szFilename[MAX_PATH];
		while (iResult != -1)
		{
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, szCurPath);
			strcat_s(szFullPath, szFilename);
			FMOD_SOUND* pSound = nullptr;

			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = (int)strlen(szFilename) + 1;

				TCHAR* pSoundKey = new TCHAR[iLength];
				ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
			iResult = _tfindnext64(handle, &fd);
		}
		FMOD_System_Update(m_pSystem);

		_findclose(handle);
	}

	// HollowLord
	{
		_tfinddata64_t fd;
		__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/HollowLord/*.*", &fd);
		if (handle == -1 || handle == 0)
			return;

		int iResult = 0;

		char szCurPath[128] = "../Bin/Resources/Sound/HollowLord/";
		char szFullPath[128] = "";
		char szFilename[MAX_PATH];
		while (iResult != -1)
		{
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, szCurPath);
			strcat_s(szFullPath, szFilename);
			FMOD_SOUND* pSound = nullptr;

			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = (int)strlen(szFilename) + 1;

				TCHAR* pSoundKey = new TCHAR[iLength];
				ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
			iResult = _tfindnext64(handle, &fd);
		}
		FMOD_System_Update(m_pSystem);

		_findclose(handle);
	}

	// Impacts
	{
		_tfinddata64_t fd;
		__int64 handle = _tfindfirst64(L"../Bin/Resources/Sound/Impacts/*.*", &fd);
		if (handle == -1 || handle == 0)
			return;

		int iResult = 0;

		char szCurPath[128] = "../Bin/Resources/Sound/Impacts/";
		char szFullPath[128] = "";
		char szFilename[MAX_PATH];
		while (iResult != -1)
		{
			WideCharToMultiByte(CP_UTF8, 0, fd.name, -1, szFilename, sizeof(szFilename), NULL, NULL);
			strcpy_s(szFullPath, szCurPath);
			strcat_s(szFullPath, szFilename);
			FMOD_SOUND* pSound = nullptr;

			FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);
			if (eRes == FMOD_OK)
			{
				int iLength = (int)strlen(szFilename) + 1;

				TCHAR* pSoundKey = new TCHAR[iLength];
				ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
				MultiByteToWideChar(CP_ACP, 0, szFilename, iLength, pSoundKey, iLength);

				m_mapSound.emplace(pSoundKey, pSound);
			}
			iResult = _tfindnext64(handle, &fd);
		}
		FMOD_System_Update(m_pSystem);

		_findclose(handle);
	}

}
