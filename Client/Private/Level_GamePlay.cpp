#include "stdafx.h"
#include "..\public\Level_GamePlay.h"
#include "Level_Loading.h"
#include "GameInstance.h"
#include "Camera_Fly.h"

#include "Navigation.h"

#include "SceneChangeEffect.h"
#include "Monster/Legion.h"


// -----------------------------
// For.CallBack using in Event
class CGameObject* m_pWar = nullptr;
class CGameObject* m_pCamera = nullptr;
// �ش� �̺�Ʈ�� �Ϸ�Ǿ��ٸ� true�� �����ϵ��� ����s
bool OnEvent1(_float fTimeDelta); // �ٸ���Ÿ ù ��� ���. ����Ʈ �߰�����.
bool OnEvent2(_float fTimeDelta); // ������ ��. ��ġ �����ִ� ���.
bool OnEvent3(_float fTimeDelta); // �� ���տ���, ���� ���ι� ������ ���. 
bool OnEvent4(_float fTimeDelta); // ���� ��.
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
		// Layer_Ballista.dat ������ �о �ش� ���̾ �߰��Ѵ�
		CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_Ballista", LEVEL_GAMEPLAY);

		// Breakables�� �߰��Ѵ�. 
		CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_Breakables", LEVEL_GAMEPLAY);
		
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
	// R Ű�� ������ Legion�� ����� ��������
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	if (CInput_Device::GetInstance()->Key_Down(DIK_R))
	{
		pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"));
		pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Goblin", TEXT("Prototype_GameObject_Goblin_Armor"));
	}

	// EŰ�� ���� ��������
	if (CInput_Device::GetInstance()->Key_Down(DIK_E))
	{
		pGameInstance->Add_GameObjectToLayer(LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect3"));
	}

	//// Breakalbe ��ġ����
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

	if (CInput_Device::GetInstance()->Key_Down(DIK_7))
	{
		if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_SoulBarrier", TEXT("Prototype_GameObject_SoulBarrier"))))
			return E_FAIL;
	}
	RELEASE_INSTANCE(CGameInstance);
#endif


	// Event Q���� �ϳ��� ���� �������. 
	// �̺�Ʈ�� ���ٸ� �ѱ���.
	if (m_queueEventCallBack.empty() == false)
	{
		auto pEvent = m_queueEventCallBack.front();
		if (pEvent(fTimeDelta))
		{
			// ���� �ش� �̺�Ʈ�� �Ϸ� �Ͽ���. pop front
			m_queueEventCallBack.pop();
		}
	}

	return _int(0);
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
	SetWindowText(g_hWnd, TEXT("�����÷����Դϴ�. "));
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
	/* For.Prototype_Component_Model_Enviroment3 */
	if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Enviroment", TEXT("Prototype_GameObject_CEnviroment"), L"Prototype_Component_Model_Enviroment3")))
		return E_FAIL;

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

	/* ���彺���̽� �󿡼��� ī�޶� ���¸� ��������. */
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


// ---------------------------------------------------
// �ٸ���Ÿ ù ��� ���. ����Ʈ �߰��� ToDo.
bool event1_event1;
bool event1_event2;
CGameObject* pEffect = nullptr;
CGameObject* pLegion = nullptr;
CGameObject* pSceneChangeEffect3 = nullptr;
bool OnEvent1(_float fTimeDelta)
{

	// ���� : War�� Navi 14���� Ż �� �̺�Ʈ�� �����Ѵ�
	CNavigation* pWarNavi = static_cast<CNavigation*>(m_pWar->Get_ComponentPtr(L"Com_Navi"));
	if (pWarNavi->m_iCurrentIndex != 14)
		return false; 

	if (event1_event1 == false)
	{
		// ����ȯ ����Ʈ ����
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		pGameInstance->Add_GameObjectToLayer(&pEffect, LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect2"));
		Safe_AddRef(pEffect);
		event1_event1 = true;

		// UI ���� Render ��������.
		auto pUIList = pGameInstance->Get_GameObject_CloneList(L"Layer_UI");
		for (auto& pUI : *pUIList)
		{
			pUI->Set_NotRender(true);
		}

		RELEASE_INSTANCE(CGameInstance);
	} 

	// Prototype_GameObject_SceneChangeEffect2 �� �������� ��������
	if (pEffect && static_cast<CSceneChangeEffect2*>(pEffect)->Get_Type() == CSceneChangeEffect2::EFFECT2_TYPE::DESCENT)
	{
		// ����Ʈ �״°��� ����ؼ� üũ�� ������.
		if (pEffect->IsDead()) // �׾����� Release����.
		{
			Safe_Release(pEffect);
		}

		// �Ʒ��� �ѹ��� �����Ѵ�.
		// Legion + ��� �ش� ��ġ�� ����. 
		if (event1_event2 == false)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			if (FAILED(pGameInstance->Add_GameObjectToLayer(&pLegion, LEVEL_GAMEPLAY, L"Layer_Legion", TEXT("Prototype_GameObject_Legion"), &_float4(600.f, 21.7f, 402.0f, 1.f))))
				return false;
			//if (FAILED(pGameInstance->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Goblin", TEXT("Prototype_GameObject_Goblin_Armor"), &_float4(601.f, 21.7f, 401.0f, 1.f))))
			//	return false;
			
			// Layer_Breakables_1 ��ϵ鵵 �̶� ��������.
			CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_Breakables_1", LEVEL_GAMEPLAY);

			// Layer_SoulBarrier ��ϵ鵵 �̶� ��������.
			CObject_Manager::GetInstance()->Load_ObjectsFromFile(L"Layer_SoulBarrier", LEVEL_GAMEPLAY);

			// ī�޶� ������ + lookAk + Ÿ�� ����
			auto pCameraTransform = static_cast<CCamera_Fly*>(m_pCamera)->Get_Camera_Transform();
			pCameraTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(595.4f, 28.1f, 381.2f, 1.f));
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Type(CCamera_Fly::CAMERA_MODE::MODE_TARGET);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Target(pLegion);
			pCameraTransform->LookAt(static_cast<CTransform*>(pLegion->Get_ComponentPtr(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION));

			// ī�޶� m_fRadius, m_fRadian, m_fHeight ����
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Radius(23.f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Radian(6.2f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Height(9.5f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Position_Ratio(0.001f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_LookAt_Ratio(0.001f);

			// ToDo : �߰������� LightManager���� Object �׸��ڵ� �����ϸ�����.. 
			// �����Ͽ���. LightManager���� ī�޶� ���õ� Ÿ���� ���󰡵��ϸ��̴�.


			// ��ȭ�� effect �߰�
			pGameInstance->Add_GameObjectToLayer(&pSceneChangeEffect3, LEVEL_LOGO, L"Layer_BackGround", TEXT("Prototype_GameObject_SceneChangeEffect3"));

			RELEASE_INSTANCE(CGameInstance);
			event1_event2 = true;
		}
	}

	// Legion�� ������ �ִ� �ٸ���Ÿ�� �ִ� ��� �ִϸ��̼� �ε����� üũ�Ѵ�. 
	if (pLegion == nullptr)
		return false;

	CGameObject* pLegion_Ballista = static_cast<CLegion*>(pLegion)->Get_Ballista();
	if (pLegion_Ballista) // ���� �ٸ���Ÿ�� �ִ� ���
	{
		_uint iKeyFrameIdx = static_cast<CLegion*>(pLegion)->Get_Model()->Get_Current_KeyFrame_Index("Legion_Mesh.ao|Legion_Ballista_Full");

		// 254 : ȭ�� �߻��ϴ� ����. ī�޶� ȭ���� ���󰡰� ����.
		if (254 <= iKeyFrameIdx && iKeyFrameIdx < 300)
		{ 
			// �ٸ���Ÿ ���� �����;��Ѵ�.
			CModel* pBallista_Model = static_cast<CModel*>(pLegion_Ballista->Get_ComponentPtr(L"Com_Model"));
			CTransform* pBallista_Transform = static_cast<CTransform*>(pLegion_Ballista->Get_ComponentPtr(L"Com_Transform"));

			// #1. ȭ�� �߻縦 �ߴ�. ī�޶� Ÿ�ٸ� ��������
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Target(pLegion_Ballista);

			// ī�޶� ��� ����������. �߸���Ÿ�� ȭ���� ���󰡰� �ϰ� �ͱ⶧���̴�.
			_matrix		OffsetMatrix = XMLoadFloat4x4(&pBallista_Model->Get_OffsetMatrix("Bone_BB_Bolt")); 
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(pBallista_Model->Get_CombinedMatrixPtr("Bone_BB_Bolt")); 
			_matrix		PivotMatrix = XMLoadFloat4x4(&pBallista_Model->Get_PivotMatrix_Bones());

			_matrix		TargetWorldMatrix = XMLoadFloat4x4(pBallista_Transform->Get_WorldFloat4x4Ptr());
			_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) * (CombinedTransformationMatrix * PivotMatrix) * TargetWorldMatrix;
			static_cast<CCamera_Fly*>(m_pCamera)->Set_BoneMat(true, TransformationMatrix);

#if 0 // For.Test
			// #2. ī�޶� ��ġ�� �������� - �ٸ���Ÿ ȭ���� ���󰣴�
			auto pCameraTransform = static_cast<CCamera_Fly*>(m_pCamera)->Get_Camera_Transform();
			_vector cameraPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);
			cameraPos = XMVector3TransformCoord(cameraPos, TransformationMatrix);
			pCameraTransform->Set_State(CTransform::STATE_POSITION, cameraPos);
#endif
			// #2. ī�޶� m_fRadius, m_fRadian, m_fHeight ����
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Radius(17.f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Radian(3.f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Height(8.f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Position_Ratio(0.01f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_LookAt_Ratio(0.01f);
		}
		else if (iKeyFrameIdx >= 300) // ���⼭ ���ʹ� �ٽ� �÷��̾�� ������.
		{
			// #1. ȭ�� �߻縦 �ߴ�. ī�޶� Ÿ���� �ٽ� �÷��̾�� ��������. 
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Target(static_cast<CWar*>(pGameInstance->Get_War(LEVEL_GAMEPLAY)));
			// ī�޶� �� ��� �����Ѵ�.
			static_cast<CCamera_Fly*>(m_pCamera)->Set_BoneMat(false, {});
			RELEASE_INSTANCE(CGameInstance);

			// #2. ī�޶� m_fRadius, m_fRadian, m_fHeight ����
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Radius(22.060f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Radian(3.109f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Height(13.f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_Position_Ratio(0.03f);
			static_cast<CCamera_Fly*>(m_pCamera)->Set_LookAt_Ratio(0.05f);

			// #3. UI �ٽ� ���̰�.
			auto pUIList = pGameInstance->Get_GameObject_CloneList(L"Layer_UI");
			for (auto& pUI : *pUIList)
				pUI->Set_NotRender(false);

			// #4. pSceneChangeEffect3 ������. 
			static_cast<CSceneChangeEffect3*>(pSceneChangeEffect3)->Set_Will_Dead(true);

			// [�̺�Ʈ ����]
			// ������ �Ϸ��Ͽ���. Event1�� �����Ѵ�.
			return true; 
		}
	}

	return false;
}

//  ������ ��. ��ġ �����ִ� ���.
bool OnEvent2(_float fTimeDelta)
{
	return true;
}

// �� ���տ���, ���� ���ι� ������ ���. 
bool OnEvent3(_float fTimeDelta)
{
	return true;
}

// ����.
bool OnEvent4(_float fTimeDelta)
{
	return true;
}


// ---------------------------------------------------
// ������� Spawn�� ����
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
