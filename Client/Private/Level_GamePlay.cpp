#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Fly.h"

#include "Navigation.h"


// -----------------------------
// For.CallBack using in Event
class CGameObject* m_pWar = nullptr;
class CGameObject* m_pCamera = nullptr;
// 해당 이벤트가 완료되었다면 true를 리턴하도록 하자s
bool OnEvent1(_float fTimeDelta); // 바리스타 첫 대면 장면. 퀘스트 추가까지.
bool OnEvent2(_float fTimeDelta); // 모험의 서. 경치 보여주는 장면.
bool OnEvent3(_float fTimeDelta); // 성 문앞에서, 몬스터 삼인방 나오는 장면. 
bool OnEvent4(_float fTimeDelta); // 보스 씬.
// ------------------------------


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

	// Ready Event
	{
		// Push OnEvent1
		m_queueEventCallBack.push(bind(&OnEvent1, placeholders::_1));
		// Push OnEvent2
		m_queueEventCallBack.push(bind(&OnEvent2, placeholders::_1));
		// Push OnEvent3
		m_queueEventCallBack.push(bind(&OnEvent3, placeholders::_1));
		// Push OnEvent4
		m_queueEventCallBack.push(bind(&OnEvent4, placeholders::_1));

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


	// Event Q에서 하나씩 꺼내 사용하자. 
	// 이벤트가 없다면 넘기자.
	if (m_queueEventCallBack.empty() == false)
	{
		auto pEvent = m_queueEventCallBack.front();
		if (pEvent(fTimeDelta))
		{
			// 만약 완료 하였다. pop front
			m_queueEventCallBack.pop();
		}
	}

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

	if (FAILED(pGameInstance->Add_GameObjectToLayer(&m_pCamera, LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Fly"), &CameraDesc)))
		return E_FAIL;
	Safe_AddRef(m_pCamera);

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
	if (FAILED(pGameInstance->Add_GameObjectToLayer(&m_pWar, LEVEL_GAMEPLAY, L"Layer_War", TEXT("Prototype_GameObject_War"))))
		return E_FAIL;
	Safe_AddRef(m_pWar);

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


// 바리스타 첫 대면 장면. 퀘스트 추가까지.
bool event1_Effect1;
bool OnEvent1(_float fTimeDelta)
{
	// 조건 : War가 Navi 14번을 탈 때 이벤트를 실행한다
	CNavigation* pWarNavi = static_cast<CNavigation*>(m_pWar->Get_ComponentPtr(L"Com_Navi"));
	if (pWarNavi->m_iCurrentIndex != 14)
		return false; 

	if (event1_Effect1 == false)
	{
		// 씬전환 이펙트 실행
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect2"));
		RELEASE_INSTANCE(CGameInstance);

		// Legion + 고블린 해당 위치에 생성. 
		// 와 결국 시간으로 컨트롤 해야하는것인가? 
		CGameObject* pLegion = nullptr;
		if (FAILED(pGameInstance->Add_GameObjectToLayer(&pLegion, LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"), &_float4(593.7f, 21.7f, 398.4f, 1.f))))
			return false;

		// 카메라 포지션 + lookup 설정
		auto pCameraTransform = static_cast<CCamera_Fly*>(m_pCamera)->Get_Camera_Transform();
		pCameraTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(595.4f, 28.1f, 381.2f, 1.f));
		static_cast<CCamera_Fly*>(m_pCamera)->Set_Type(CCamera_Fly::CAMERA_MODE::MODE_TARGET); 
		static_cast<CCamera_Fly*>(m_pCamera)->Set_Target(pLegion);
		pCameraTransform->LookAt(static_cast<CTransform*>(pLegion->Get_ComponentPtr(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION));

		// 카메라 서서히 움직이기 
		event1_Effect1 = true;
	}
	printf("%s:%d\n", __func__, __LINE__);
	return true;
}

// 모험의 서. 경치 보여주는 장면.
bool OnEvent2(_float fTimeDelta)
{
	printf("%s:%d\n", __func__, __LINE__);
	return true;
}
// 성 문앞에서, 몬스터 삼인방 나오는 장면. 
bool OnEvent3(_float fTimeDelta)
{
	printf("%s:%d\n", __func__, __LINE__);
	return true;
}
// 보스 씬.
bool OnEvent4(_float fTimeDelta)
{
	printf("%s:%d\n", __func__, __LINE__);
	return true;
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

	Safe_Release(m_pWar);
	Safe_Release(m_pCamera);
}
