#include "stdafx.h"
#include "..\public\War.h"

#include "GameInstance.h"
#include "PipeLine.h"



// In State_War.cpp
extern CStateMachine* g_pWar_State_Context;
extern CModel* g_pWar_Model_Context;
extern CTransform* g_pWar_Transform_Context;
#include "State_War.h"




CWar::CWar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWar::CWar(const CWar & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWar::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CWar::NativeConstruct(void * pArg)
{
	if (SetUp_Component())
		return E_FAIL;	




	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 10, 0.f, rand() % 10, 1.f));

	return S_OK;
}


_int CWar::Tick(_float fTimeDelta)
{
	// War 애니메이션 키프레임 변환
	m_pModelCom[MODELTYPE_WAR]->Update_Animation(fTimeDelta);

	// War 애니메이션 인덱스 변환 - FSM 머신으로 관리
	if (m_pStateMachineCom)
		m_pStateMachineCom->Tick(fTimeDelta);

	// 방향키 입력받아 회전,이동 처리
	War_Key(fTimeDelta);


	//// ----------------------------
	//// ----------------------------
	//// ----------------------------
	//// ----------------------------
	//// For Test
	//const int CONST_MAX_ANIM_NUM = 72;
	//static int animIdx = 0; 
	//bool dirty = false;
	//const auto dirtyF = [&dirty, &CONST_MAX_ANIM_NUM](bool PlusOrMinus) {
	//	if (!PlusOrMinus)
	//	{
	//		animIdx = animIdx - 1;
	//		if (animIdx < 0)
	//			animIdx = CONST_MAX_ANIM_NUM -1;
	//	}
	//	else
	//	{
	//		animIdx = (animIdx + 1) % (CONST_MAX_ANIM_NUM);
	//	}
	//	dirty = true;
	//};
	//if (CInput_Device::GetInstance()->Key_Down(DIK_MINUS)) // '-'
	//	dirtyF(false);
	//if (CInput_Device::GetInstance()->Key_Down(DIK_EQUALS)) // '+'
	//	dirtyF(true);

	//if (dirty)
	//{
	//	cout << "animIdx : " << animIdx << endl;
	//	m_pModelCom[MODELTYPE_WAR]->SetUp_Animation(animIdx);
	//}
	//// ----------------------------
	//// ----------------------------
	//// ----------------------------
	//// ----------------------------

	return _int();
}

_int CWar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// SetHeight
	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	CVIBuffer_Terrain* pTerrainBuff = (CVIBuffer_Terrain*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer"));
	if (nullptr == pTerrainBuff)
		goto _EXIT;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, pTerrainBuff->Compute_Height(vPosition)));

	// Renderer
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA_WAR, this)))
		goto _EXIT;

_EXIT:
	RELEASE_INSTANCE(CGameInstance);
	return _int();
}

HRESULT CWar::Render()
{
	// 
	// 1. War 원형 렌더하면서, 스텐실 버퍼에 1로 채운다. 
	// 
	m_pDeviceContext->OMSetDepthStencilState(RenderStates::MarkMirrorDSS.Get(), 1);


	/* 장치에 월드변환 행렬을 저장한다. */
	for (int modelIdx = 0; modelIdx < MODELTYPE_END; modelIdx++)
	{
		if (FAILED(SetUp_ConstantTable(false, modelIdx)))
			return E_FAIL;

		// iNumMaterials : 망토, 몸통
		_uint	iNumMaterials = m_pModelCom[modelIdx]->Get_NumMaterials(); 
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE); 
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);

			m_pModelCom[modelIdx]->Render(i, 0);
		}
	}

	// restore default states, as the Shader_AnimMesh.hlsl changes them in the effect file.
	m_pRendererCom->ClearRenderStates();


	// 
	// 2. (스텐실버퍼가 0인경우에) War 외곽선 Draw, Draw 순서는 : 지형->NONALPHA->War 순
	// 
	m_pDeviceContext->OMSetDepthStencilState(RenderStates::DrawReflectionDSS.Get(), 0);

	for (int modelIdx = 0; modelIdx < MODELTYPE_END; modelIdx++)
	{
		if (FAILED(SetUp_ConstantTable(true, modelIdx)))
			return E_FAIL;

		_uint	iNumMaterials = m_pModelCom[modelIdx]->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelCom[modelIdx]->Set_ShaderResourceView("g_EmissiveTexture", i, aiTextureType_EMISSIVE);


			m_pModelCom[modelIdx]->Render(i, 0);
		}
	}

	// Restore default states
	m_pRendererCom->ClearRenderStates();


	return S_OK;
}

_vector CWar::Get_War_Pos()
{
	return m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
}

_float CWar::GetDegree(_ubyte downedKey)
{
	_float retDegree = 0.f;//12 Default는 정면을 바라보게하자.

	// 카메라가 바라보는 위치에 따라 오프셋을 다르게 주어야한다. 
	// 카메라 각도는 +Z축이 0, 시계방향으로 + 이다. 
	_vector CameraToWar_Look = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - CPipeLine::GetInstance()->Get_CamPosition();
	_vector CameraToWar_Look_XZ = XMVectorSet(XMVectorGetX(CameraToWar_Look), 0.f, XMVectorGetZ(CameraToWar_Look), 0.f);
	CameraToWar_Look_XZ = XMVector2Normalize(CameraToWar_Look_XZ);

	_vector vecCameraFromZ = XMVector3AngleBetweenVectors(CameraToWar_Look_XZ, XMVectorSet(0.f, 0.f, 1.f, 0.f)) * (XMVectorGetX(CameraToWar_Look_XZ) < 0.f ? -1.f : 1.f)/* 만약 xz 룩벡터가 -x쪽을 보면 -1.f를 곱하자.*/;
	_float degreeCameraFromZ = XMConvertToDegrees(XMVectorGetX(vecCameraFromZ));

	//cout << degreeCameraFromZ << endl;

	// 왼위오아
	if (downedKey == 0b0001)//6
		retDegree = 180.f;
	else if (downedKey == 0b0011)//4
		retDegree = 135.f;
	else if (downedKey == 0b0010)//3
		retDegree = 90.f;
	else if (downedKey == 0b0110)//2
		retDegree = 45.f;
	else if (downedKey == 0b0100)//12
		retDegree = 0.f;
	else if (downedKey == 0b1100)//11
		retDegree = 315.f;
	else if (downedKey == 0b1000)//9
		retDegree = 270.f;
	else if (downedKey == 0b1001)//7
		retDegree = 225.f;

	return retDegree + degreeCameraFromZ;

}

_bool CWar::KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit)
{
	if (CInput_Device::GetInstance()->Key_Pressing(key))
	{
		_ubyte dirBit = 0b0; // 왼위오아
		switch (key)
		{
		case DIK_A:
			dirBit = 0b1000;
			break;
		case DIK_W:
			dirBit = 0b0100;
			break;
		case DIK_D:
			dirBit = 0b0010;
			break;
		case DIK_S:
			dirBit = 0b0001;
			break;
		default:
			break;
		}

		keyDownCheckBit |= dirBit;
		return true;
	}

	return false;
}

void CWar::War_Key(_float fTimeDelta)
{
	unsigned char keyDownCheckBit = 0b0;
	bool isKeyDown = false;
	auto const dirtyCheck = [&isKeyDown, &keyDownCheckBit](_bool b) { isKeyDown |= b; };
	dirtyCheck(KeyCheck(DIK_A, keyDownCheckBit));
	dirtyCheck(KeyCheck(DIK_W, keyDownCheckBit));
	dirtyCheck(KeyCheck(DIK_D, keyDownCheckBit));
	dirtyCheck(KeyCheck(DIK_S, keyDownCheckBit));

	if (isKeyDown == false)
		return;

	// 누른 키에 맞게 움직이자.
	// 1) 회전
	//m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(GetDegree(keyDownCheckBit))); // 고정
	m_pTransformCom->TurnTo_AxisY_Degree(GetDegree(keyDownCheckBit), fTimeDelta * 10); // 서서히 회전

	// 2) 전진.
	m_pTransformCom->Go_Straight(fTimeDelta);
}

HRESULT CWar::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 3.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model_War */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War"), TEXT("Com_Model_War"), (CComponent**)&m_pModelCom[MODELTYPE_WAR])))
		return E_FAIL;
	m_pModelCom[MODELTYPE_WAR]->SetUp_Animation("War_Mesh.ao|War_Idle");

	/* For.Com_Model_War_Gauntlet */
	// 애니메이션은 Com_Model_War를 복사하자
	CModel::MODELDESC	tagModelWarGauntletDesc;
	tagModelWarGauntletDesc.pHierarchyNodes = m_pModelCom[MODELTYPE_WAR]->Get_HierarchyNodes();
	tagModelWarGauntletDesc.pAnimations = m_pModelCom[MODELTYPE_WAR]->Get_Animations();

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Gauntlet"), TEXT("Com_Model_War_Gauntlet"), (CComponent**)&m_pModelCom[MODELTYPE_GAUNTLET], &tagModelWarGauntletDesc)))
		return E_FAIL;

	/* For.Prototype_Component_Model_War_Weapon */
	// Model_War가 갖고 있는 뼈중 이름이 "Bone_War_Weapon_Sword" 인것을 찾아 넣어주자.
	CModel::MODELDESC	tagModelWarWeaponDesc;
	tagModelWarWeaponDesc.pHierarchyNode = m_pModelCom[MODELTYPE_WAR]->Find_HierarchyNode("Bone_War_Weapon_Sword");
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_War_Weapon"), TEXT("Com_Model_War_Weapon"), (CComponent**)&m_pModelCom[MODELTYPE_WEAPON], &tagModelWarWeaponDesc)))
		return E_FAIL;

	/* For.Com_StateMachine : Model 뒤에 해야한다. Model를 사용하기 때문. */
	g_pWar_Model_Context = m_pModelCom[MODELTYPE_WAR];
	g_pWar_Transform_Context = m_pTransformCom;
	CStateMachine::STATEMACHINEDESC fsmDesc;
	fsmDesc.pOwner = this;
	fsmDesc.pInitState = CState_War_Idle::GetInstance();

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom, &fsmDesc)))
		return E_FAIL;

	// 글로벌 상태는 여기서 설정하자
	static_cast<CStateMachine*>(m_pStateMachineCom)->Set_GlobalState(CGlobal_State_War::GetInstance());

	g_pWar_State_Context = m_pStateMachineCom;

	return S_OK;
}



HRESULT CWar::SetUp_ConstantTable(bool drawOutLine, int modelIdx)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	// Bind Directional Light
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;
	m_pModelCom[modelIdx]->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));

	// Bind Material
	m_pModelCom[modelIdx]->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	// Bind Transform
	m_pTransformCom->Bind_OnShader(m_pModelCom[modelIdx], "g_WorldMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom[modelIdx], "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom[modelIdx], "g_ProjMatrix");

	// Bind Position
	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	m_pModelCom[modelIdx]->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	// Branch to Use Normal Mapping 
	// 노멀맵할지 말지 선택을 여기서 하자
	m_pModelCom[modelIdx]->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	m_pModelCom[modelIdx]->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	// Outline 원형은 그리지않는다.
	m_pModelCom[modelIdx]->Set_RawValue("g_DrawOutLine", &drawOutLine, sizeof(bool));


#if 0 // Legacy
	LIGHTDESC		LightDesc = *pGameInstance->Get_LightDesc(0);
	m_pVIBufferCom->Set_RawValue("g_vLightDir", &_float4(LightDesc.vDirection, 0.f), sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightDiffuse", &LightDesc.vDiffuse, sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightAmbient", &LightDesc.vAmbient, sizeof(_float4));
	m_pVIBufferCom->Set_RawValue("g_vLightSpecular", &LightDesc.vSpecular, sizeof(_float4));

	_float4			vCamPosition;	
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	m_pVIBufferCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));
#endif
	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CWar * CWar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CWar*		pInstance = new CWar(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create CWar");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CWar::Clone(void* pArg)
{
	CWar*		pInstance = new CWar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone CWar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWar::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pStateMachineCom);

	for (auto& modelCom : m_pModelCom)
		Safe_Release(modelCom);

	// Destroy the State SingleTon : State_War.cpp 
	g_pWar_State_Context = nullptr;
	g_pWar_Model_Context = nullptr;
	g_pWar_Transform_Context = nullptr;
}
