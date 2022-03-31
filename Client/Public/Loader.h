#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	explicit CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLoader() = default;

public:
	CRITICAL_SECTION* Get_CriticalSection() {
		return &m_CS;
	}

	LEVEL Get_NextLevel() {
		return m_eNextLevel;
	}

	const _tchar* Get_LoadingText() {
		return m_szLoading;
	}


	_bool Get_Finished() {
		return m_isFinished;
	}



public:
	HRESULT NativeConstruct(LEVEL eNextLevel);
	HRESULT Loading_ForLogoLevel();
	HRESULT Loading_ForGamePlayLevel();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	LEVEL					m_eNextLevel = LEVEL_END;
	HANDLE					m_hThread;
	CRITICAL_SECTION		m_CS;

	_tchar					m_szLoading[MAX_PATH] = TEXT("Loading");
	_bool					m_isFinished = false;
	
private:
	HRESULT Add_GameObject();
	HRESULT Add_VIBuffer();
	HRESULT Add_Texture();
	HRESULT Add_Model();


public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END