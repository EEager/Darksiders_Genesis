#include "stdafx.h"
#include "..\public\Level_Logo.h"
#include "Level_Loading.h"
#include "GameInstance.h"

#include "Logo_BackGround.h"

CLevel_Logo::CLevel_Logo(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Logo::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	// 사운드
	SoundManager::Get_Instance()->ForcePlayBGM(L"mus_mainmenu.OGG"); 

	return S_OK;
}

_int CLevel_Logo::Tick(_float fTimeDelta)
{
	if (CInput_Device::GetInstance()->Key_Down(DIK_M))
	{

	}
#ifdef _DEBUG

#endif

	if (0 > (__super::Tick(fTimeDelta)))
		return -1;

	// 스페이스바를 눌러서 씬을 넘기자. 스페이스를 한번 눌렀다면 아래 if문을 ㄱㅖ속 타도록 m_bPressedSpace를 true로 바꿔주자
	if (m_bPressedSpace || CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		// CLogo_BackGround가 더이상 문구를 출력하지 않게 해주자
		if (m_bPressedSpace == false)
		{
			static_cast<CLogo_BackGround*>(m_pBackGround)->m_bPressedSpace = true;

			// 씬체인지를 넣어주자
			pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect1"));
			m_bPressedSpace = true;
		}

		// 이펙트가 완료가 되었을 경우, 씬체인지가 CLevel_Logo의 변수를 set할것이다. 
		if (m_bGoToThe_NextLevel)
		{
			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_GAMEPLAY))))
				return -1;
		}

		RELEASE_INSTANCE(CGameInstance);

		return 0;
	}

#ifdef _DEBUG
	// E키를 눌러 시험하자
	if (CInput_Device::GetInstance()->Key_Down(DIK_E))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect2"));
		RELEASE_INSTANCE(CGameInstance);

	}
#endif

	return _int(0);
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG

	SetWindowText(g_hWnd, TEXT("로고씬입니다. "));
#endif

	return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(&m_pBackGround, LEVEL_LOGO, pLayerTag, TEXT("Prototype_GameObject_Logo_BackGround"))))
		return E_FAIL;

	Safe_AddRef(m_pBackGround);
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Logo* pInstance = new CLevel_Logo(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Logo::Free()
{
	__super::Free();
	Safe_Release(m_pBackGround);
}
