#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Fly.h"



CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_GamePlay::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_War"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_LightDesc()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Enviroment()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Object()))
		return E_FAIL;

	// Load Dat Files
	{
		// Layer_Ballista.dat 파일을 읽어서 해당 레이어에 추가한다
		CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_Ballista", LEVEL_GAMEPLAY);
	}

	 
	return S_OK;
}

_int CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (0 > (__super::Tick(fTimeDelta)))
		return -1;

#ifdef _DEBUG 
	// R 키를 눌러서 Legion 을 생성하자
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (CInput_Device::GetInstance()->Key_Down(DIK_R))
	{
		pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"));
	}

	// E키를 눌러 시험하자
	if (CInput_Device::GetInstance()->Key_Down(DIK_E))
	{
		pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect2"));
	}
	RELEASE_INSTANCE(CGameInstance);
#endif

	return _int(0);
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("게임플레이입니다. "));
#endif

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LightDesc()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Directional Light
	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vAmbient = XMFLOAT4(133/255.f, 133 / 255.f, 133 / 255.f, 1.0f);
	LightDesc.vDiffuse = XMFLOAT4(0.85f, 0.85f, 0.85f, 0.85f);
	LightDesc.vSpecular = XMFLOAT4(199/255.f, 64 / 255.f, 0 / 255.f, 1.0f);
	LightDesc.vDirection = XMFLOAT3(1.4f, -1.9f, 0.8f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;


	// Point Light
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = tagLightDesc::TYPE_POINT;
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.0f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.0f);
	LightDesc.vSpecular = _float4(0.7f, 0.7f, 0.7f, 1.0f);
	LightDesc.vPosition = _float3(5.0f, 3.0f, 5.f);
	LightDesc.fRadiuse = 25.f;
	/*
	mSpotLight.Att      = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mSpotLight.Spot     = 96.0f;
	*/

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;


	// Spot Light
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = tagLightDesc::TYPE_SPOT;
	LightDesc.vDiffuse = _float4(1.0f, 1.0f, 0.0f, 1.0f);
	LightDesc.vAmbient = _float4(0.0f, 0.0f, 0.0f, 1.0f);
	LightDesc.vSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	LightDesc.fRadiuse = 10000.0f;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pDeviceContext, LightDesc)))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_Enviroment()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_Component_Model_Enviroment1 */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment1")))
		return E_FAIL;
	/* For.Prototype_Component_Model_Enviroment2 */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment2")))
		return E_FAIL;
	///* For.Prototype_Component_Model_Enviroment3 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment3")))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_Ballista */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Ballista", TEXT("Prototype_GameObject_Ballista"))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* 월드스페이스 상에서의 카메라 상태를 셋팅하자. */
	CCamera::CAMERADESC			CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));
	CameraDesc.vEye = _float3(10.f, 10.f, 430.f);
	CameraDesc.vAt = _float3(17.f, 0.f, 430.f);;
	CameraDesc.vAxisy = _float3(0.f, 1.f, 0.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 700.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 15.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Fly"), &CameraDesc)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	///* For.Terrain */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Terrain"))))
	//	return E_FAIL;

	/* For.Prototype_GameObject_Sky */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Layer_War*/
	for (int i = 0; i < 1; i++)
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_War", TEXT("Prototype_GameObject_War"))))
			return E_FAIL;

#ifndef ONLY_WAR
	/* For.Layer_Player*/
	for (int i = 0; i < 2; i++)
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Player", TEXT("Prototype_GameObject_Player"))))
			return E_FAIL;

	/* For.Layer_Player's Sword*/
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Player_Sword", TEXT("Prototype_GameObject_Sword"))))
		return E_FAIL;

	/* For.Layer_Fork*/
	for (_uint i = 0; i < 1; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Fork", TEXT("Prototype_GameObject_Fork"))))
			return E_FAIL;
	}
#endif

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_RectEffect"))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_PointEffect"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar* pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_UI_War_Hp_n_Wrath_Bar*/
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI_War_Hp_n_Wrath_Bar"))))
		return E_FAIL;

	/* For.Prototype_GameObject_UI_War_Hp_n_Wrath_Bar*/
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_UI_War_Skills"))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	/* For.Prototype_GameObject_Legion*/
	for (int i = 0; i < 2; i++)
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"))))
			return E_FAIL;

	/* For.Prototype_GameObject_Goblin_Armor*/
	for (int i = 0; i < 4; i++)
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Goblin", TEXT("Prototype_GameObject_Goblin_Armor"))))
			return E_FAIL;

	/* For.Prototype_GameObject_FallenDog*/
	//for (int i = 0; i < 2; i++)
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_FallenDog", TEXT("Prototype_GameObject_FallenDog"))))
	//		return E_FAIL;

	/* For.Prototype_GameObject_HollowLord*/
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_HollowLord", TEXT("Prototype_GameObject_HollowLord"))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_GamePlay* pInstance = new CLevel_GamePlay(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed to Created CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
