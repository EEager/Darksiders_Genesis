#include "stdafx.h"
#include "..\public\Monster\Goblin_Armor.h"
#include "GameInstance.h"


CGoblin_Armor::CGoblin_Armor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CGoblin_Armor::CGoblin_Armor(const CGoblin_Armor & rhs)
	: CMonster(rhs)
{
}

HRESULT CGoblin_Armor::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CGoblin_Armor::NativeConstruct(void * pArg)
{
	// GameInfo Init
	m_tGameInfo.iAtt = 1;
	m_tGameInfo.iEnergy = rand() % 10 + 1;
	m_tGameInfo.iMaxHp = 10;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 1;

	m_fSpeed = 5.f;
	// ��� ���ʹ� m_pTransformCom, m_pRendererCom, m_pNaviCom�� ������
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;	

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Armor"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_MonsterHp_PointGS"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIHpBarGsBufferCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 1.f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 2.f, 0.5f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, COL_MONSTER_BODY1);

	// For Weapon
	{
		/* For.Com_Model_Goblin_Spear */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Spear"), TEXT("Com_Model_Goblin_Spear"), (CComponent**)&m_pModelSpearCom)))
			return E_FAIL;
		/* For.Com_Model_Goblin_Quiver */
		if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Quiver"), TEXT("Com_Model_Goblin_Quiver"), (CComponent**)&m_pModelQuiverCom)))
			return E_FAIL;

		/* For.GoblinSpear */
		ColliderDesc.vPivot = _float3(0.f, -0.5f, 0.f); 
		ColliderDesc.fRadius = 0.25f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);

		ZeroMemory(&m_spearDesc, sizeof(SPEARDESC));
		m_spearDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_Goblin_Weapon_Fleamag_Sword");
		m_spearDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_Goblin_Weapon_Fleamag_Sword");
		m_spearDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_spearDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

		/* For.GoblinQuiver */
		ZeroMemory(&m_quiverDesc, sizeof(QUIVERDESC));
		m_quiverDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_Goblin_Quiver");
		m_quiverDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_Goblin_Quiver");
		m_quiverDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_quiverDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	}


	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%10, 0.f, 431.f + rand() % 10, 1.f));

	// Init Anim State
	m_pModelCom->SetUp_Animation(m_pCurState, true);

	// ��� ���ʹ� Navigation �ʱ� �ε����� �������Ѵ�
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	return S_OK;
}

_int CGoblin_Armor::Tick(_float fTimeDelta)
{
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	// Ÿ���� ��������
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
	//Update_Colliders();

	// FSM
	UpdateState();
	CMonster::DoGlobalState(fTimeDelta);
	DoState(fTimeDelta);


	// anim update : �����̵��� -> �����̵��ݿ�
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_Goblin_Root", m_pNaviCom, m_eDir);

	return _int();
}

_int CGoblin_Armor::LateTick(_float fTimeDelta)
{
	// ��� ���ʹ� Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	// ü���� 0���ϰ� �Ǹ� ����. 
	// �ٷ� ���������� �״� ��� �� ������ ������
	if (m_tGameInfo.iHp <= 0 && m_bWillDead == false/* �ѹ��� ���� ��ȭ�ϱ� ���� �ʿ���*/)
	{
		m_bWillDead = true;
		m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_Death_01";
	}

	return _int();
}

HRESULT CGoblin_Armor::Render(_uint iPassIndex)
{
	// ��� ���ʹ� SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	Render_Goblin();

	// HP Bar Render
	if (m_tGameInfo.iHp != m_tGameInfo.iMaxHp) // �ѹ� ������ ��������
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		// Bind Transform
		m_pTransformCom->Bind_OnShader(m_pVIHpBarGsBufferCom, "g_WorldMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pVIHpBarGsBufferCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pVIHpBarGsBufferCom, "g_ProjMatrix");

		// Bind Position
		m_pVIHpBarGsBufferCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_vector));


		// Bind ���� ���� ü�º����� UV x ��ǥ�� �Ѱ�����. ex) 0.01 ~ 0.99 �̴ϱ� 80% �� (0.99 - 0.01) * 0.8;
#define MAX_LEN_HP (0.99f - 0.01f)
		_float fCurHpRatio = (_float)m_tGameInfo.iHp / (_float)m_tGameInfo.iMaxHp;
		_float fUVx = MAX_LEN_HP * fCurHpRatio;
		m_pVIHpBarGsBufferCom->Set_RawValue("g_fMonsterHpUVX", &fUVx, sizeof(_float));

		_float fHpBarHeight = 1.2f;
		m_pVIHpBarGsBufferCom->Set_RawValue("g_fHpBarHeight", &fHpBarHeight, sizeof(_float));

		m_pVIHpBarGsBufferCom->Render(iPassIndex);
		RELEASE_INSTANCE(CGameInstance);
	}
	
	return S_OK;
}

void CGoblin_Armor::Render_Goblin()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	LIGHTDESC		dirLightDesc = *pGameInstance->Get_LightDesc(0);
	DirectionalLight mDirLight;
	mDirLight.Ambient = dirLightDesc.vAmbient;
	mDirLight.Diffuse = dirLightDesc.vDiffuse;
	mDirLight.Specular = dirLightDesc.vSpecular;
	mDirLight.Direction = dirLightDesc.vDirection;

	_float4			vCamPosition;
	XMStoreFloat4(&vCamPosition, pGameInstance->Get_CamPosition());

	// Render Sphere
	{
		// Bind Directional Light

		m_pModelSpearCom->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));
		// Bind Material
		m_pModelSpearCom->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix);
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
		_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix);

		// �̰� �����ΰ� ����...
		_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) /** OffsetMatrix*/ * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
		_float4x4	modelWorldMat;
		XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(TransformationMatrix));
		m_pModelSpearCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));

		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelSpearCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelSpearCom, "g_ProjMatrix");

		// Bind Position
		m_pModelSpearCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

		// Branch to Use Normal Mapping 
		m_pModelSpearCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
		m_pModelSpearCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

		_uint	iNumMaterials = m_pModelSpearCom->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelSpearCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelSpearCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelSpearCom->Render(i, 0);
		}
	}


	// Render Quiver
	{
		m_pModelQuiverCom->Set_RawValue("g_DirLight", &mDirLight, sizeof(DirectionalLight));
		// Bind Material
		m_pModelQuiverCom->Set_RawValue("g_Material", &m_tMtrlDesc, sizeof(MTRLDESC));

		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_quiverDesc.pBoneMatrix);
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_quiverDesc.OffsetMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_quiverDesc.PivotMatrix);
		_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_quiverDesc.pTargetWorldMatrix);

		_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(90)) * XMMatrixRotationY(XMConvertToRadians(180)) * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
		_float4x4	modelWorldMat;
		XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(TransformationMatrix));
		m_pModelQuiverCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));

		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelQuiverCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelQuiverCom, "g_ProjMatrix");

		// Bind Position
		m_pModelQuiverCom->Set_RawValue("g_vCamPosition", &vCamPosition, sizeof(_float4));

		// Branch to Use Normal Mapping 
		m_pModelQuiverCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
		m_pModelQuiverCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

		_uint	iNumMaterials = m_pModelQuiverCom->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelQuiverCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelQuiverCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelQuiverCom->Render(i, 0);
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

_int CGoblin_Armor::Update_Colliders(_matrix wolrdMatrix)
{
	// For Weapon Collider
	for (auto pCollider : m_ColliderList)
	{
		if (pCollider->Get_ColliderTag() == COL_MONSTER_WEAPON)
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix);
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix);
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix);
			_matrix		TransformationMatrix =
				(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) *
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

void CGoblin_Armor::UpdateState()
{
	if (m_pCurState == m_pNextState)
		return;

	_bool isLoop = false;

	// -----------------------------
	// m_pCurState Exit
	if (
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_01" ||
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_02" ||
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear"
		)
	{
		// �ش� ���� Exit�� ���� �ݶ��̴� ����.
		Set_Collider_Attribute(COL_MONSTER_WEAPON, true);
	}

	// -----------------------------
	// m_pNextState Enter
	if (m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle" ||
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Run_F"
		)
	{
		isLoop = true;
		m_eDir = OBJECT_DIR::DIR_F;
	}
	else if (
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_01" ||
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_02" ||
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear"
		)
	{
		// �ش� ���¿��� ���� �ݶ��̴� Ű��
		Set_Collider_Attribute(COL_MONSTER_WEAPON, false);
		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
	}
	else if (
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Dash_Back" ||
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_Death_01"
		)
	{
		m_eDir = OBJECT_DIR::DIR_B;
		isLoop = false;
	}


	m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
	m_pCurState = m_pNextState;
}

void CGoblin_Armor::DoState(float fTimeDelta)
{
	//-----------------------------------------------------
	if (m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle")
	{
		// �÷��̾ ���� �ݰ� ���� �ִٸ� ��������
		_float disToTarget = Get_Target_Dis();
		if (disToTarget < ATK_RANGE) // ���� �Ÿ� ���� ��������
		{
			m_fTimeIdle += fTimeDelta;
			if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY) // ������ idle ���¿��� ��� ���� �ð��� �����ٸ� ���� ��������
			{
				m_fTimeIdle = 0.f;
				m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
				int randNextState = rand() % 4;
				if (randNextState == 0)	m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_01";
				else if (randNextState == 1) m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_02";
				else if (randNextState == 2) m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Dash_Back";
				else if (randNextState == 3) m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear";
			}
		}
		else if (m_bNotSpearAtk == false/*�ؿ� â������������� �÷��� �ɸ��� �� �ķδ� â ������ ��ŵ�Ѵ� */ && disToTarget < SPEAR_RANGE) // ���Ÿ� �� ��� 50%Ȯ���� â ��������
		{
			if (rand() % 4 == 0)
			{
				m_bNotSpearAtk = true; // â���� ���� ����� �÷���
			}
			else
			{
				m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
				m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear";
			}
		}
		else if (disToTarget < CHASE_RANGE)
		{
			// �÷��̾ ���� �ݰ� �ȿ� �ִٸ� ���� 
			// �׷��� �ٷ� �������� ���� 1������ �����ٰ� ��������
			m_fTimeIdle += fTimeDelta;

			if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY)
			{
				m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Run_F";
				m_fTimeIdle = 0.f;
			}
		}
	}
	//-----------------------------------------------------
	else if (
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Dash_Back" // �ڷ� ���� â��������
		)
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
			m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear";
		}
	}
	//-----------------------------------------------------
	else if (
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_01" ||
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_02" ||
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear")
	{
		m_bNotSpearAtk = false;
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			//m_pTransformCom->LookAt(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION));

			m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle";
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Run_F")
	{
		// �����ϴٰ� ���� �ݰ� ���� �ִٸ� ����  
		if (Get_Target_Dis() < ATK_RANGE)
		{
			m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
			int randAtk = rand() % 2;
			if (randAtk == 0) m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_01";
			else if (randAtk == 1) m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_02";
		}
		// ������ War �������� ���鼭 go Straight
		else
		{
			m_pTransformCom->TurnTo_AxisY_Degree(GetDegree_Target(), fTimeDelta * 10);
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNaviCom);
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState ==  "Goblin_Armor_Mesh.ao|Goblin_Death_01")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_isDead = true;
		}
	}
}

_float CGoblin_Armor::Get_Target_Dis(float fTimeDelta)
{
	// Ÿ�ٰ��� �Ÿ��� ���Ѵ�
	return XMVectorGetX(XMVector3Length(
		m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION) -
		m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION)));
}

_float CGoblin_Armor::GetDegree_Target()
{
	_vector targetPos = m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION);
	_vector myPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	_vector toTarget = XMVector4Normalize(targetPos - myPos);

	XMVECTOR curVecAngleVec = XMVector3AngleBetweenVectors(toTarget, XMVectorSet(0.f, 0.f, 1.f, 0.f))
		* (XMVectorGetX(toTarget) < 0.f ? -1.f : 1.f);
	return XMConvertToDegrees(XMVectorGetX(curVecAngleVec));
}


CGoblin_Armor * CGoblin_Armor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGoblin_Armor*		pInstance = new CGoblin_Armor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CGoblin_Armor");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CGoblin_Armor::Clone(void* pArg)
{
	CGoblin_Armor*		pInstance = new CGoblin_Armor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CGoblin_Armor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_Armor::Free()
{
	Safe_Release(m_pVIHpBarGsBufferCom);
	Safe_Release(m_pTarget);

	Safe_Release(m_pModelSpearCom);
	Safe_Release(m_pModelQuiverCom);

	CMonster::Free();
}
