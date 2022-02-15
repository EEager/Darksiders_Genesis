#include "stdafx.h"
#include "..\public\Level_Loading.h"

#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "GameInstance.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{

}

HRESULT CLevel_Loading::NativeConstruct(LEVEL eNextLevel)
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pDeviceContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_LoadingUI(TEXT("Layer_LoadingUI"))))
		return E_FAIL;

	return S_OK;
}

_int CLevel_Loading::Tick(_float fTimeDelta)
{
	if (0 > (__super::Tick(fTimeDelta)))
		return -1;

	if (nullptr != m_pLoader)
	{
		if (true == m_pLoader->Get_Finished())
		{
			if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
			{
				if (SUCCEEDED(Open_Level()))
					return 0;
			}
		}
	}
	return _int();
}

HRESULT CLevel_Loading::Render()
{
	if (nullptr == m_pLoader)
		return E_FAIL;

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());

	return S_OK;
}

HRESULT CLevel_Loading::Open_Level()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CLevel*			pLevel = nullptr;

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		pLevel = CLevel_Logo::Create(m_pDevice, m_pDeviceContext);
		break;
	case LEVEL_GAMEPLAY:
		pLevel = CLevel_GamePlay::Create(m_pDevice, m_pDeviceContext);
		break;
	}

	if (nullptr == pLevel)
		goto except;

	if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pLevel)))
		goto except;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;


except:
	RELEASE_INSTANCE(CGameInstance);
	return E_FAIL;

}

HRESULT CLevel_Loading::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_LoadingUI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(eNextLevel)))	{
		MSG_BOX("Failed to Created CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();	

	Safe_Release(m_pLoader);
}
