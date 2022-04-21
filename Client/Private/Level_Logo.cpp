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

	// ����
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

	// �����̽��ٸ� ������ ���� �ѱ���. �����̽��� �ѹ� �����ٸ� �Ʒ� if���� ���Ƽ� Ÿ���� m_bPressedSpace�� true�� �ٲ�����
	if (m_bPressedSpace || CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		// CLogo_BackGround�� ���̻� ������ ������� �ʰ� ������
		if (m_bPressedSpace == false)
		{
			static_cast<CLogo_BackGround*>(m_pBackGround)->m_bPressedSpace = true;

			// ��ü������ �־�����
			pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect1"));
			m_bPressedSpace = true;
		}

		// ����Ʈ�� �Ϸᰡ �Ǿ��� ���, ��ü������ CLevel_Logo�� ������ set�Ұ��̴�. 
		if (m_bGoToThe_NextLevel)
		{
			if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_GAMEPLAY))))
				return -1;
		}

		RELEASE_INSTANCE(CGameInstance);

		return 0;
	}

#ifdef _DEBUG
	// EŰ�� ���� ��������
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

	SetWindowText(g_hWnd, TEXT("�ΰ���Դϴ�. "));
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
