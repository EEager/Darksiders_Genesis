#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Fly.h"

#include "Navigation.h"

#include "SceneChangeEffect.h"
#include "Monster/Legion.h"
#include "MapObject\SoulBarrier.h"
#include "Enviroment.h"

#include "Monster/HollowLord.h"


// -----------------------------
// For.CallBack using in Event
class CGameObject* g_pWar = nullptr;
class CGameObject* g_pCamera = nullptr;
// 해당 이벤트가 완료되었다면 true를 리턴하도록 하자s
bool OnEvent1(_float fTimeDelta); // 바리스타 첫 대면 장면. 퀘스트 추가까지.
bool OnEvent2(_float fTimeDelta); // 모험의 서. 경치 보여주는 장면.
bool OnEvent3(_float fTimeDelta); // 성 문앞에서, 몬스터 삼인방 나오는 장면. 
bool OnEvent4(_float fTimeDelta); // 보스 씬.
bool OnEvent5(_float fTimeDelta); // 보스 씬 Spike
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

		// Breakables를 추가한다. 
		CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_Breakables", LEVEL_GAMEPLAY);

		// Layer_Flag_A
		//CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_Flag_A", LEVEL_GAMEPLAY);

		// Layer_Trees를 추가한다. 
		CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_Trees", LEVEL_GAMEPLAY);
	}

	// Ready Level Event
	{
		// OnEvent1
		m_queueEventCallBack.push(bind(&OnEvent1, placeholders::_1));
		// OnEvent2
		m_queueEventCallBack.push(bind(&OnEvent2, placeholders::_1));
		// OnEvent3
		m_queueEventCallBack.push(bind(&OnEvent3, placeholders::_1));
		// OnEvent4 
		m_queueEventCallBack.push(bind(&OnEvent4, placeholders::_1));
		// OnEvent5
		m_queueEventCallBack.push(bind(&OnEvent5, placeholders::_1));
	}

	// Ready Monster Spawner
	{
		//// MonsterSpanwer1
		//m_queueMonsterSpawner.push(bind(&MonsterSpanwer1, placeholders::_1));
		//// MonsterSpanwer2
		//m_queueMonsterSpawner.push(bind(&MonsterSpanwer2, placeholders::_1));
		//// MonsterSpanwer3
		//m_queueMonsterSpawner.push(bind(&MonsterSpanwer3, placeholders::_1));
	}
	 
	return S_OK;
}

_int CLevel_GamePlay::Tick(_float fTimeDelta)
{
	if (0 > (__super::Tick(fTimeDelta)))
		return -1;

#ifdef _DEBUG 
	// R 키를 눌러서 Legion과 고블린을 생성하자
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (CInput_Device::GetInstance()->Key_Down(DIK_E))
	{
		pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"));
		pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Goblin", TEXT("Prototype_GameObject_Goblin_Armor"));
		//pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_FallenDog", TEXT("Prototype_GameObject_FallenDog")); 

		//pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_SpikeGate", TEXT("Prototype_GameObject_SpikeGate"));
		//pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Flag_A", TEXT("Prototype_GameObject_Flag_A"));
	}

	//// Trees 설치하자
	//if (CInput_Device::GetInstance()->Key_Down(DIK_2)) 
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Trees", TEXT ("Prototype_GameObject_TreeA"))))
	//		return E_FAIL;
	//}
	//if (CInput_Device::GetInstance()->Key_Down(DIK_3))
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Trees", TEXT("Prototype_GameObject_TreeB"))))
	//		return E_FAIL;
	//}
	//if (CInput_Device::GetInstance()->Key_Down(DIK_4))
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Trees", TEXT("Prototype_GameObject_TreeC"))))
	//		return E_FAIL;
	//}

	// E키를 눌러 시험하자
	
	// 영화관 이펙트를 시험하자
	//if (CInput_Device::GetInstance()->Key_Down(DIK_E))
	//{
	//	pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect3"));
	//}

	//// Breakalbe 설치하자
	//if (CInput_Device::GetInstance()->Key_Down(DIK_2))
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Breakables_1", TEXT("Prototype_GameObject_Breakable1"))))
	//		return E_FAIL;
	//}
	//if (CInput_Device::GetInstance()->Key_Down(DIK_3))
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Breakables_1", TEXT("Prototype_GameObject_Breakable2"))))
	//		return E_FAIL;
	//}
	//if (CInput_Device::GetInstance()->Key_Down(DIK_4))
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Breakables_1", TEXT("Prototype_GameObject_Breakable3"))))
	//		return E_FAIL;
	//}
	//if (CInput_Device::GetInstance()->Key_Down(DIK_5))
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Breakables_1", TEXT("Prototype_GameObject_Breakable4"))))
	//		return E_FAIL;
	//}
	//if (CInput_Device::GetInstance()->Key_Down(DIK_6))
	//{
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Breakables_1", TEXT("Prototype_GameObject_Breakable5"))))
	//		return E_FAIL;
	//}

	//if (CInput_Device::GetInstance()->Key_Down(DIK_7))
	//{ 
	//	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_SoulBarrier", TEXT("Prototype_GameObject_SoulBarrier"))))
	//		return E_FAIL;
	//}
	RELEASE_INSTANCE(CGameInstance);
#endif

	// Event Q에서 하나씩 꺼내 사용하자. 
	// 이벤트가 없다면 넘기자.
	if (m_queueEventCallBack.empty() == false)
	{
		auto pEvent = m_queueEventCallBack.front();
		if (pEvent(fTimeDelta))
		{
			// 만약 해당 이벤트가 완료 하였다. pop front
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

	CGameObject* pEnviroment = nullptr;

	/* For.Prototype_Component_Model_Enviroment1 */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment1")))
		return E_FAIL;
	///* For.Prototype_Component_Model_Enviroment2 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment2")))
	//	return E_FAIL;
	///* For.Prototype_Component_Model_Enviroment3_1 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(&pEnviroment, LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment3_1")))
	//	return E_FAIL;
	//static_cast<CEnviroment*>(pEnviroment)->Set_CullingRadian(180.f);
	///* For.Prototype_Component_Model_Enviroment3_2 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(&pEnviroment, LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment3_2")))
	//	return E_FAIL;
	//static_cast<CEnviroment*>(pEnviroment)->Set_CullingRadian(180);
	///* For.Prototype_Component_Model_Enviroment3_3 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(&pEnviroment, LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment3_3")))
	//	return E_FAIL;
	//static_cast<CEnviroment*>(pEnviroment)->Set_CullingRadian(180);
	///* For.Prototype_Component_Model_Enviroment3_4 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(&pEnviroment, LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment3_4")))
	//	return E_FAIL;
	//static_cast<CEnviroment*>(pEnviroment)->Set_CullingRadian(180);

	///* For.Prototype_Component_Model_Enviroment4 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(&pEnviroment, LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment4")))
	//	return E_FAIL;
	//static_cast<CEnviroment*>(pEnviroment)->Set_CullingRadian(150.f);

#if 0 // not use
	///* For.Prototype_Component_Model_Foliage0 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Foliage0")))
	//	return E_FAIL;
#endif
	/* For.Prototype_Component_Model_Foliage1 */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Foliage1")))
		return E_FAIL;
	/* For.Prototype_Component_Model_Foliage2 */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Foliage2")))
		return E_FAIL;
	///* For.Prototype_Component_Model_Foliage3 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Foliage3")))
	//	return E_FAIL;
	///* For.Prototype_Component_Model_Foliage4 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Foliage4")))
	//	return E_FAIL;
	///* For.Prototype_Component_Model_Foliage5 */
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Foliage5")))
	//	return E_FAIL;
	///* For.Prototype_Component_Model_Foliage6 */
	//CGameObject* pFoliage6 = nullptr;
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(&pFoliage6, LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Foliage6")))
	//	return E_FAIL;
	//static_cast<CEnviroment*>(pFoliage6)->Set_CullingRadian(100.f);

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

	if (FAILED(pGameInstance->Add_GameObjectToLayer(&g_pCamera, LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_Camera_Fly"), &CameraDesc)))
		return E_FAIL;
	Safe_AddRef(g_pCamera);

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
	if (FAILED(pGameInstance->Add_GameObjectToLayer(&g_pWar, LEVEL_GAMEPLAY, L"Layer_War", TEXT("Prototype_GameObject_War"))))
		return E_FAIL;
	Safe_AddRef(g_pWar);

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

	// RectEffect
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_RectEffect"))))
		return E_FAIL;


	// PointEffect
	//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_PointEffect"))))
	//	return E_FAIL;

	// FireEffect
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, pLayerTag, TEXT("Prototype_GameObject_FireEffect"))))
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

	Safe_Release(g_pWar);
	Safe_Release(g_pCamera);
}







// ---------------------------------------------------
// 바리스타 첫 대면 장면. 퀘스트 추가는 ToDo.
bool event1_event0;
bool event1_event1;
bool event1_event2;
bool event1_event3;
CGameObject* pEffect = nullptr;
CGameObject* pLegion = nullptr;
CGameObject* pSceneChangeEffect3 = nullptr;
bool OnEvent1(_float fTimeDelta)
{
	// [조건]
	// War가 Navi 14번을 탈 때 이벤트를 실행한다
	if (event1_event0 == false)
	{
		CNavigation* pWarNavi = static_cast<CNavigation*>(g_pWar->Get_ComponentPtr(L"Com_Navi"));
		if (pWarNavi->m_iCurrentIndex != 14)
			return false;
		event1_event0 = true;
	}

	if (event1_event1 == false)
	{
		// 씬전환 이펙트 실행
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObjectToLayer(&pEffect, LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect2"));
		Safe_AddRef(pEffect);

		// UI 전부 Render 하지말자.
		auto pUIList = pGameInstance->Get_GameObject_CloneList(L"Layer_UI");
		for (auto& pUI : *pUIList)
		{
			pUI->Set_NotRender(true);
		}

		event1_event1 = true;
		RELEASE_INSTANCE(CGameInstance);
	}

	// Prototype_GameObject_SceneChangeEffect2 가 내려갈때 실행하자
	if (pEffect && static_cast<CSceneChangeEffect2*>(pEffect)->Get_Type() == CSceneChangeEffect2::EFFECT2_TYPE::DESCENT)
	{
		// 이펙트 죽는것은 계속해서 체크를 ㅎ라자.
		if (!event1_event3 && pEffect->IsDead()) // 죽었으면 Release하자.
		{
			Safe_Release(pEffect);
			event1_event3 = true;
		}

		// 아래는 한번만 실행한다.
		// Legion + 고블린 해당 위치에 생성. 
		if (event1_event2 == false)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			// Layer_Legion 생성 
			if (FAILED(pGameInstance->Add_GameObjectToLayer(&pLegion, LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"), &_float4(600.f, 21.7f, 402.0f, 1.f))))
				return false;

			// Layer_FallenDog 생성
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_FallenDog", TEXT("Prototype_GameObject_FallenDog"), &_float4(618.f, 12.47f, 152.0f, 1.f))))
				assert(0);

			// Layer_SoulBarrier 생성
			CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_SoulBarrier", LEVEL_GAMEPLAY);

			// Layer_Breakables_1 목록들도 이때 생성하자.
			CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_Breakables_1", LEVEL_GAMEPLAY);

			// 카메라 포지션 + lookAk + 타겟 설정
			auto pCameraTransform = static_cast<CCamera_Fly*>(g_pCamera)->Get_Camera_Transform();
			pCameraTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(595.4f, 28.1f, 381.2f, 1.f));
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Type(CCamera_Fly::CAMERA_MODE::MODE_TARGET);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Target(pLegion);
			auto pLegionTransform = static_cast<CTransform*>(pLegion->Get_ComponentPtr(L"Com_Transform"));
			pCameraTransform->LookAt(pLegionTransform->Get_State(CTransform::STATE_POSITION));
			// Legion 이 카메라를 바라보도록하자.
			pLegionTransform->LookAt(XMVectorSetY(pCameraTransform->Get_State(CTransform::STATE_POSITION), XMVectorGetY(pCameraTransform->Get_State(CTransform::STATE_POSITION))));

			// 카메라 m_fRadius, m_fRadian, m_fHeight 설정
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Radius(23.f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Radian(6.2f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Height(9.5f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Position_Ratio(0.001f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_LookAt_Ratio(0.001f);

			// ToDo : 추가적으로 LightManager에서 Object 그림자도 변경하면좋다.. 
			// 변경하였다. LightManager에서 카메라에 세팅된 타겟을 따라가도록말이다.


			// 영화관 effect 추가
			pGameInstance->Add_GameObjectToLayer(&pSceneChangeEffect3, LEVEL_GAMEPLAY, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect3"));

			RELEASE_INSTANCE(CGameInstance);
			event1_event2 = true;
		}
	}

	// Legion이 가지고 있는 바리스타가 있는 경우 애니메이션 인덱스를 체크한다. 
	if (pLegion == nullptr)
		return false;

	CGameObject* pLegion_Ballista = static_cast<CLegion*>(pLegion)->Get_Ballista();
	if (pLegion_Ballista) // 만약 바리스타가 있는 경우
	{
		_uint iKeyFrameIdx = static_cast<CLegion*>(pLegion)->Get_Model()->Get_Current_KeyFrame_Index("Legion_Mesh.ao|Legion_Ballista_Full");

		// 254 : 화살 발사하는 순간. 카메라가 화살을 따라가게 하자.
		if (254 <= iKeyFrameIdx && iKeyFrameIdx < 300)
		{
			// 바리스타 모델을 가져와야한다.
			CModel* pBallista_Model = static_cast<CModel*>(pLegion_Ballista->Get_ComponentPtr(L"Com_Model"));
			CTransform* pBallista_Transform = static_cast<CTransform*>(pLegion_Ballista->Get_ComponentPtr(L"Com_Transform"));

			// #1. 화살 발사를 했다. 카메라 타겟를 변경하자
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Target(pLegion_Ballista);

			// 카메라 행렬 설정해주자. 발리스타의 화살을 따라가게 하고 싶기때문이다.
			_matrix		OffsetMatrix = XMLoadFloat4x4(&pBallista_Model->Get_OffsetMatrix("Bone_BB_Bolt"));
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(pBallista_Model->Get_CombinedMatrixPtr("Bone_BB_Bolt"));
			_matrix		PivotMatrix = XMLoadFloat4x4(&pBallista_Model->Get_PivotMatrix_Bones());

			_matrix		TargetWorldMatrix = XMLoadFloat4x4(pBallista_Transform->Get_WorldFloat4x4Ptr());
			_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * (CombinedTransformationMatrix * PivotMatrix) * TargetWorldMatrix;
			static_cast<CCamera_Fly*>(g_pCamera)->Set_BoneMat(true, TransformationMatrix);

#if 0 // For.Test
			// #2. 카메라 위치를 변경하자 - 바리스타 화살을 따라간다
			auto pCameraTransform = static_cast<CCamera_Fly*>(g_pCamera)->Get_Camera_Transform();
			_vector cameraPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			cameraPos = XMVector3TransformCoord(cameraPos, TransformationMatrix);
			pCameraTransform->Set_State(CTransform::STATE_POSITION, cameraPos);
#endif
			// #2. 카메라 m_fRadius, m_fRadian, m_fHeight 설정
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Radius(17.f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Radian(3.f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Height(8.f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Position_Ratio(0.01f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_LookAt_Ratio(0.01f);
		}
		else if (iKeyFrameIdx >= 300) // 여기서 부터는 다시 플레이어로 가도록.
		{
			// #1. 화살 발사를 했다. 카메라 타겟을 다시 플레이어로 변경하자. 
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Target(static_cast<CWar*>(pGameInstance->Get_War(LEVEL_GAMEPLAY)));
			// 카메라 뼈 행렬 사용안한다.
			static_cast<CCamera_Fly*>(g_pCamera)->Set_BoneMat(false, {});
			RELEASE_INSTANCE(CGameInstance);

			// #2. 카메라 m_fRadius, m_fRadian, m_fHeight 원복
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Radius(22.060f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Radian(3.109f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Height(13.f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Position_Ratio(0.03f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_LookAt_Ratio(0.05f);

			// #3. UI 다시 보이게.
			auto pUIList = pGameInstance->Get_GameObject_CloneList(L"Layer_UI");
			for (auto& pUI : *pUIList)
				pUI->Set_NotRender(false);

			// #4. pSceneChangeEffect3 죽이자. 
			static_cast<CSceneChangeEffect3*>(pSceneChangeEffect3)->Set_Will_Dead(true);

			// [이벤트 종료]
			// 모든것이 완료하였다. Event1을 종료한다.
			return true;
		}
}

	return false;
}

//  모험의 서. 경치 보여주는 장면. 하지말자
bool OnEvent2(_float fTimeDelta)
{
	return true;
}

// ---------------------------------------------------
// 성 문앞에서, Dog 몬스터 나오는 장면.
bool event3_0;
bool event3_1;
bool event3_2;
bool event3_3;
_float event3TimeAcc;
CGameObject* pSoulBarrier = nullptr;
bool OnEvent3(_float fTimeDelta)
{
	// [이벤트 시작]
	// War가 Navi 93번을 탈 때 이벤트를 실행한다
	if (event3_0 == false)
	{
		CNavigation* pWarNavi = static_cast<CNavigation*>(g_pWar->Get_ComponentPtr(L"Com_Navi"));
		if (pWarNavi->m_iCurrentIndex != 93)
			return false;
		event3_0 = true;
	}

	if (event3_1 == false)
	{
		// 씬전환 이펙트 실행
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObjectToLayer(&pEffect, LEVEL_GAMEPLAY, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect2"));
		Safe_AddRef(pEffect);
		// UI 전부 Render 하지말자.
		auto pUIList = pGameInstance->Get_GameObject_CloneList(L"Layer_UI");
		for (auto& pUI : *pUIList)
		{
			pUI->Set_NotRender(true);
		}
		RELEASE_INSTANCE(CGameInstance);
		event3_1 = true;
	}

	// Prototype_GameObject_SceneChangeEffect2 가 내려갈때 실행하자
	if (pEffect && static_cast<CSceneChangeEffect2*>(pEffect)->Get_Type() == CSceneChangeEffect2::EFFECT2_TYPE::DESCENT)
	{
		// 이펙트 죽는것은 계속해서 체크를 ㅎ라자.
		if (event3_3 == false && pEffect->IsDead()) // 죽었으면 Release하자.
		{
			Safe_Release(pEffect);
			event3_3 = true;
		}

		// 아래는 한번만 실행한다.
		// Legion2마리 생성
		if (event3_2 == false)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"), &_float4(611.f, 11.6f, 154.0f, 1.f))))
				assert(0);
			if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"), &_float4(624.f, 11.6f, 149.0f, 1.f))))
				assert(0);

			auto pSoulBarrier = CObject_Manager::GetInstance()->Get_GameObject_CloneList(L"Layer_SoulBarrier")->front();

			// 카메라 타겟은 pSoulBarrier로 위치 설정. 
			// 카메라 포지션 + lookAk + 타겟 설정
			auto pCameraTransform = static_cast<CCamera_Fly*>(g_pCamera)->Get_Camera_Transform();
			pCameraTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(613.7f, 22.5f, 170.f, 1.f));
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Type(CCamera_Fly::CAMERA_MODE::MODE_TARGET);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Target(pSoulBarrier);
			auto pSoulBarrierTransform = static_cast<CTransform*>(pSoulBarrier->Get_ComponentPtr(L"Com_Transform"));
			pCameraTransform->LookAt(pSoulBarrierTransform->Get_State(CTransform::STATE_POSITION));

			// 카메라 m_fRadius, m_fRadian, m_fHeight 설정
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Radius(19.f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Radian(1.611f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Height(13.f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Position_Ratio(1.f);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_LookAt_Ratio(1.f);

			// 영화관 effect 추가
			pGameInstance->Add_GameObjectToLayer(&pSceneChangeEffect3, LEVEL_GAMEPLAY, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect3"));

			event3_2 = true;
			RELEASE_INSTANCE(CGameInstance);

			return false; // 한번 틱돌리자.
		}
	}

	if (event3_2 == false)
		return false;

	// [이벤트 종료]
	event3TimeAcc += fTimeDelta;

	if (0.7f < event3TimeAcc && event3TimeAcc < 0.9f)
	{
		auto pSoulBarrier = CObject_Manager::GetInstance()->Get_GameObject_CloneList(L"Layer_SoulBarrier")->front();
		static_cast<CSoulBarrier*>(pSoulBarrier)->InitAnimation();
	}

	if (event3TimeAcc > 9.f)
	{
		// 이벤트가 종료되었으니 원복.
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Target(static_cast<CWar*>(g_pWar));
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Radius(22.060f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Radian(1.97f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Height(13.f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Position_Ratio(0.03f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_LookAt_Ratio(0.05f);

		// 영화관 이펙트를 죽이자.
		static_cast<CSceneChangeEffect3*>(pSceneChangeEffect3)->Set_Will_Dead(true);

		// UI 다시 보이게.
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		auto pUIList = pGameInstance->Get_GameObject_CloneList(L"Layer_UI");
		for (auto& pUI : *pUIList)
			pUI->Set_NotRender(false);
		RELEASE_INSTANCE(CGameInstance);

		// [이벤트 종료]
		// 모든것이 완료하였다. Event3을 종료한다.
		return true;
	}

	// 이벤트 종료 전까지 계속 실행되는것.
	// #2. 카메라 m_fRadius, m_fRadian, m_fHeight 설정
	static_cast<CCamera_Fly*>(g_pCamera)->Set_Radius(30.f);
	static_cast<CCamera_Fly*>(g_pCamera)->Set_Radian(1.611f);
	static_cast<CCamera_Fly*>(g_pCamera)->Set_Height(12.f);
	static_cast<CCamera_Fly*>(g_pCamera)->Set_Position_Ratio(0.001f);
	static_cast<CCamera_Fly*>(g_pCamera)->Set_LookAt_Ratio(0.001f);

	return false;
}

// ---------------------------------------------------
// 보스씬.
bool event4_0;
bool event4_1;
bool event4_2;
bool event4_3;
_float m_event4_TimeAcc;

bool OnEvent4(_float fTimeDelta)
{
	// [이벤트 시작]
	// War가 Navi 120번을 탈 때 이벤트를 실행한다
	if (event4_0 == false)
	{
		CNavigation* pWarNavi = static_cast<CNavigation*>(g_pWar->Get_ComponentPtr(L"Com_Navi"));
		if (pWarNavi->m_iCurrentIndex != 120)
			return false;
		event4_0 = true;
	}


	// 씬전환 이펙트 실행
	if (event4_1 == false)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObjectToLayer(&pEffect, LEVEL_GAMEPLAY, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect2"));
		Safe_AddRef(pEffect);
		// UI 전부 Render 하지말자.
		auto pUIList = pGameInstance->Get_GameObject_CloneList(L"Layer_UI");
		for (auto& pUI : *pUIList)
		{
			pUI->Set_NotRender(true);
		}
		RELEASE_INSTANCE(CGameInstance);
		event4_1 = true;
	}

	// Prototype_GameObject_SceneChangeEffect2 가 내려갈때 실행하자
	if (pEffect && static_cast<CSceneChangeEffect2*>(pEffect)->Get_Type() == CSceneChangeEffect2::EFFECT2_TYPE::DESCENT)
	{
		// 이펙트 죽는것은 계속해서 체크를 해서 위에 Safe_AddRef 한것을 해제해주자.
		if (event4_3 == false && pEffect->IsDead()) // 죽었으면 Release하자.
		{
			Safe_Release(pEffect);
			event4_3 = true;
		}

		// 보스 생성 + 카메라 위치 조정
		if (event4_2 == false)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			// Layer_HollowLord 생성
			CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_HollowLord", LEVEL_GAMEPLAY);

			// Layer_BrokenCorner 생성
			CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_BrokenCorner", LEVEL_GAMEPLAY);

			// 카메라 타겟은 Free로.  
			// 카메라 포지션, LookAt Set
			auto pCameraTransform = static_cast<CCamera_Fly*>(g_pCamera)->Get_Camera_Transform();
			pCameraTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(574.f, 24.f, 82.3f, 1.f));
			pCameraTransform->LookAt(XMVectorSet(583.9f, 13.6f, 39.6f, 1.f));
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Type(CCamera_Fly::CAMERA_MODE::MODE_FREE);
			static_cast<CCamera_Fly*>(g_pCamera)->Set_Camera_Speed(1.f);
			static_cast<CTransform*>( g_pWar->Get_ComponentPtr(L"Com_Transform") )->Set_State(CTransform::STATE_POSITION, XMVectorSet(582.548f, 9.5f, 70.709f, 1.f));

			// 영화관 effect 추가
			pGameInstance->Add_GameObjectToLayer(&pSceneChangeEffect3, LEVEL_GAMEPLAY, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect3"));

			// Light Manager가 가지고 있는 OBJ_SHADOW 반경 변경. m_fSceneRadius 
			//CLight_Manager::GetInstance()->m_fSceneRadius = 36.f;

			event4_2 = true;
			RELEASE_INSTANCE(CGameInstance);
			return false; // 한번 틱 돌리자.
		}
	}

	if (event4_2 == false)
		return false;

	// 카메라 서서히 뒤로 빼면서. 시간으로 계산하자. 몇초뒤에 이벤트 종료.
	auto pCameraTransform = static_cast<CCamera_Fly*>(g_pCamera)->Get_Camera_Transform();
	pCameraTransform->Go_Backward(fTimeDelta * 1.f);
	m_event4_TimeAcc += fTimeDelta;
	if (m_event4_TimeAcc > 9.5f) 
	{
		// 이벤트가 종료되었으니 카메라 원복.
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Type(CCamera_Fly::CAMERA_MODE::MODE_TARGET);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Target(static_cast<CWar*>(g_pWar));

		static_cast<CCamera_Fly*>(g_pCamera)->Set_Radius(23.276f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Radian(1.586f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Height(13.f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Position_Ratio(0.03f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_LookAt_Ratio(0.05f);
		static_cast<CCamera_Fly*>(g_pCamera)->Set_Camera_Speed(15.f);


		// 영화관 이펙트를 죽이자.
		static_cast<CSceneChangeEffect3*>(pSceneChangeEffect3)->Set_Will_Dead(true);

		// UI 다시 보이게.
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		auto pUIList = pGameInstance->Get_GameObject_CloneList(L"Layer_UI");
		for (auto& pUI : *pUIList)
			pUI->Set_NotRender(false);

		// OnEvent4가 끝나고 바로 보스 체력을 활성화 시키자.
		CHollowLord* pHollowLord = (CHollowLord*)(CObject_Manager::GetInstance()->Get_GameObject_CloneList(L"Layer_HollowLord")->front());
		pHollowLord->UI_Init();
		RELEASE_INSTANCE(CGameInstance);

		// [이벤트 종료]
		// 모든것이 완료하였다. Event4을 종료한다.
		return true;
	}



	return false;
}


// --------------------------------------------------------
// OnEvent4가 끝나고 1초 뒤에 SpikeGate를 불러오자. 
_float OnEvent5_timeAcc;
bool OnEvent5(_float fTimeDelta)
{
	OnEvent5_timeAcc += fTimeDelta;
	if (OnEvent5_timeAcc > 1.f)
	{
		// Layer Spike 불러오자
		CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_SpikeGate", LEVEL_GAMEPLAY);
		return true;
	}

	return false;
}


// ---------------------------------------------------
// 순서대로 Spawn을 하자
bool MonsterSpanwer1(_float fTimeDelta)
{
	return true;
}

bool MonsterSpanwer2(_float fTimeDelta)
{
	return true;
}

bool MonsterSpanwer3(_float fTimeDelta)
{
	return true;
}

