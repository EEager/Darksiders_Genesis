#include "stdafx.h"
#include "..\public\Monster\Legion.h"
#include "GameInstance.h"


CLegion::CLegion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CLegion::CLegion(const CLegion & rhs)
	: CMonster(rhs)
{
}

HRESULT CLegion::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CLegion::NativeConstruct(void * pArg)
{
	// GameInfo Init
	m_tGameInfo.iAtt = 1.f;
	m_tGameInfo.iEnergy = 1.7f;
	m_tGameInfo.iMaxHp = 10.f;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = 1.f;


	m_fSpeed = 8.f;
	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다. 
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;	

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 1.0f, 0.f);
	ColliderDesc.vSize = _float3(1.5f, 2.0f, 1.5f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, COL_MONSTER_BODY1);


	// For Weapon
	{ 
		/* For.Com_Model_Legion_WeaponL */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion_Axe_L"), TEXT("Com_Model_Legion_WeaponL"), (CComponent**)&m_pModelWeaponLCom)))
			return E_FAIL;
		/* For.Com_Model_Legion_WeaponR */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Legion_Axe_R"), TEXT("Com_Model_Legion_WeaponR"), (CComponent**)&m_pModelWeaponRCom)))
			return E_FAIL;

		/* For.LegionWeapon */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, -.75f); // For Test
		ColliderDesc.fRadius = 0.5f;

		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		//__super::Add_Collider(&ColliderDesc, L"LegionWeapon");
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON);

		ZeroMemory(&m_WarSwordDesc, sizeof(SWORDDESC));
		m_WarSwordDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_LE_Weapon");
		m_WarSwordDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_LE_Weapon");
		m_WarSwordDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_WarSwordDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	}


	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%30, 0.f, 431.f + rand() % 20, 1.f));
	m_pModelCom->SetUp_Animation(m_pCurState, true);

	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));
	
	return S_OK;
}

_int CLegion::Tick(_float fTimeDelta)
{
	if (!m_bTargetingOnce)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		auto pLayer_War = pGameInstance->Get_GameObject_CloneList(TEXT("Layer_War"));
		if (pLayer_War)
		{
			m_pTarget = pLayer_War->front();
			Safe_AddRef(m_pTarget);
			m_pTargetTransform = static_cast<CTransform*>(m_pTarget->Get_ComponentPtr(L"Com_Transform"));
			m_bTargetingOnce = true;
		}
		else
		{
			RELEASE_INSTANCE(CGameInstance);
			return 0;
		}
		RELEASE_INSTANCE(CGameInstance)
	}

	// For Weapon Collider
	Update_Colliders();

	// FSM
	UpdateState();
	CMonster::DoGlobalState(fTimeDelta);
	DoState(fTimeDelta);

	// anim update : 로컬이동값 -> 월드이동반영
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "_Ctrl_World", m_pNaviCom, m_eDir);

	return _int();
}

_int CLegion::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	return _int();
}

HRESULT CLegion::Render(_uint iPassIndex)
{
	// 모든 몬스터는 SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	// Weapon Render : ToDo 최적화
	Render_Weapon(m_pModelWeaponLCom, XMConvertToRadians(-90));
	Render_Weapon(m_pModelWeaponRCom, XMConvertToRadians(-90));

	return S_OK;
}


HRESULT CLegion::Render_Weapon(CModel* pModel, _float fRadian)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Directional Light
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;
	pModel->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));
	// Bind Material
	pModel->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

	_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_WarSwordDesc.pBoneMatrix);
	_matrix		PivotMatrix = XMLoadFloat4x4(&m_WarSwordDesc.PivotMatrix);
	_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_WarSwordDesc.pTargetWorldMatrix);
	_matrix		TransformationMatrix = XMMatrixRotationX(fRadian) * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
	_float4x4 modelWeaponWorldMat;
	XMStoreFloat4x4(&modelWeaponWorldMat, XMMatrixTranspose(TransformationMatrix));
	pModel->Set_RawValue("g_WorldMatrix", &modelWeaponWorldMat, sizeof(_float4x4));

	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, pModel, "g_ViewMatrix");
	pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, pModel, "g_ProjMatrix");

	// Bind Position
	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());
	pModel->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

	// Branch to Use Normal Mapping 
	pModel->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	pModel->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

	RELEASE_INSTANCE(CGameInstance);

	_uint	iNumMaterials = pModel->Get_NumMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		pModel->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		pModel->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
		pModel->Render(i, 0);
	}

	return S_OK;
}


void CLegion::UpdateState()
{
	if (m_pCurState == m_pNextState)
		return;

	_bool isLoop = false;

	// m_eCurState Exit


	// m_eNextState Enter
	if (m_pNextState == "Legion_Mesh.ao|Legion_Idle" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Run_F"
		)
	{
		isLoop = true;
		m_eDir = OBJECT_DIR::DIR_F;
	}
	else if (
		m_pNextState == "Legion_Mesh.ao|Legion_Atk_Flurry" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Atk_Heavy" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Attack_02" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Atk_Slam" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Taunt_01" ||
		m_pNextState == "Legion_Mesh.ao|Legion_Taunt_02"
		)
	{
		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
	}
	else if (m_pNextState == "Legion_Mesh.ao|Legion_Evade_Left")
	{
		m_eDir = OBJECT_DIR::DIR_L;
		isLoop = false;
	}
	else if (m_pNextState == "Legion_Mesh.ao|Legion_Evade_Right")
	{
		m_eDir = OBJECT_DIR::DIR_R;
		isLoop = false;
	}

	m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
	m_pCurState = m_pNextState;
}


// FSM
void CLegion::DoState(float fTimeDelta)
{
	//-----------------------------------------------------
	if (m_pCurState == "Legion_Mesh.ao|Legion_Idle")
	{
		// 플레이어가 공격 반경 내에 있다면 공격. 근데 바로 하지는 말고 좀 쉬었다하자
		_float disToTarget = Get_Target_Dis();
		if (m_fTimeIdle < fTimeDelta && (disToTarget < ATK_RANGE))
		{
			m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
			int randNextState = rand() % 4;
			if (randNextState == 0)	m_pNextState = "Legion_Mesh.ao|Legion_Atk_Flurry";
			else if (randNextState == 1) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Heavy";
			else if (randNextState == 2) m_pNextState = "Legion_Mesh.ao|Legion_Attack_02";
			else if (randNextState == 3) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Slam";
	
		}
		else if (disToTarget < CHASE_RANGE)
		{
			// 플레이어가 추적 반경 안에 있다면 추적 
			// 그러나 바로 추적하지 말고 1초정도 쉬었다가 추적하자
			m_fTimeIdle += fTimeDelta;

			if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY)
			{
				m_pNextState = "Legion_Mesh.ao|Legion_Run_F";
				m_fTimeIdle = 0.f;
			}
		}

	}
	//-----------------------------------------------------
	else if (
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Flurry" || 
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Heavy" ||
		m_pCurState == "Legion_Mesh.ao|Legion_Attack_02" ||
		m_pCurState == "Legion_Mesh.ao|Legion_Atk_Slam")
	{

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{ 
			// 확률로 도발 모션 ㅋ
			int randTaunt = rand() % 5;
			if (randTaunt == 0)
				m_pNextState = "Legion_Mesh.ao|Legion_Taunt_01";
			else if (randTaunt == 1)
				m_pNextState = "Legion_Mesh.ao|Legion_Taunt_02";
			else if (randTaunt == 2)// 공격 애니메이션 끝나면 Idle로 
				m_pNextState = "Legion_Mesh.ao|Legion_Idle";
			else if (randTaunt == 3)
				m_pNextState = "Legion_Mesh.ao|Legion_Evade_Left";
			else if (randTaunt == 4)
				m_pNextState = "Legion_Mesh.ao|Legion_Evade_Right";
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Run_F")
	{
		// 추적하다가 공격 반경 내에 있다면 공격  
		if (Get_Target_Dis() < ATK_RANGE)
		{
			m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
			int randAtk = rand() % 4;
			if (randAtk == 0) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Flurry";
			else if (randAtk == 1) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Heavy";
			else if (randAtk == 2) m_pNextState = "Legion_Mesh.ao|Legion_Attack_02";
			else if (randAtk == 3) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Slam";
		}
		// 추적은 War 방향으로 돌면서 go Straight
		else
		{
			m_pTransformCom->TurnTo_AxisY_Degree(GetDegree_Target(), fTimeDelta * 10);
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNaviCom);
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Taunt_01" || "Legion_Mesh.ao|Legion_Taunt_02")
	{
		// 도발 애니메이션 끝나면 
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			// 확률로 와가리 털자 ㅎ 
			m_pNextState = "Legion_Mesh.ao|Legion_Idle";
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Legion_Mesh.ao|Legion_Evade_Right" || "Legion_Mesh.ao|Legion_Evade_Left")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			_float disToTarget = Get_Target_Dis();
			if (disToTarget < ATK_RANGE)
			{
				m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
				int randNextState = rand() % 4;
				if (randNextState == 0)	m_pNextState = "Legion_Mesh.ao|Legion_Atk_Flurry";
				else if (randNextState == 1) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Heavy";
				else if (randNextState == 2) m_pNextState = "Legion_Mesh.ao|Legion_Attack_02";
				else if (randNextState == 3) m_pNextState = "Legion_Mesh.ao|Legion_Atk_Slam";
			}
			else if (disToTarget < CHASE_RANGE)
			{
				m_pNextState = "Legion_Mesh.ao|Legion_Run_F";
			}
		}
	}
}

_float CLegion::Get_Target_Dis(float fTimeDelta)
{
	// 타겟간의 거리를 구한다
	return XMVectorGetX(XMVector3Length(
		m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION) - 
		m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION)));
}

_float CLegion::GetDegree_Target()
{
	_vector targetPos = m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);
	_vector myPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	_vector toTarget = XMVector4Normalize(targetPos - myPos);

	XMVECTOR curVecAngleVec = XMVector3AngleBetweenVectors(toTarget,XMVectorSet(0.f, 0.f, 1.f, 0.f)) 
		* (XMVectorGetX(toTarget) < 0.f ? -1.f : 1.f);
	return XMConvertToDegrees(XMVectorGetX(curVecAngleVec));
}

_int CLegion::Update_Colliders(_matrix wolrdMatrix/*not used*/)
{
	for (auto pCollider : m_ColliderList)
	{
		if (pCollider->Get_ColliderTag() == COL_MONSTER_WEAPON)
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_WarSwordDesc.OffsetMatrix); // 뼈->정점
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_WarSwordDesc.pBoneMatrix); // Root->뼈 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_WarSwordDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_WarSwordDesc.pTargetWorldMatrix); // just legion's world matrix
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else
		{
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
		}
	}

	return 0;
}

CLegion * CLegion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLegion*		pInstance = new CLegion(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CLegion");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CLegion::Clone(void* pArg)
{
	CLegion*		pInstance = new CLegion(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CLegion");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLegion::Free()
{
	Safe_Release(m_pTarget);
	Safe_Release(m_pModelWeaponLCom);
	Safe_Release(m_pModelWeaponRCom);
	CMonster::Free();
}
