#include "stdafx.h"
#include "..\public\Monster\Goblin_Armor.h"
#include "GameInstance.h"
#include "Camera.h"


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


	// pArg�� ���� ��ġ�̴�. w�� 1�� _float4�̴�.
	if (pArg)
	{
		_float4* ArgPos = (_float4*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(ArgPos));
	}
	else 
		// Init Test
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%10, 0.f, 431.f + rand() % 10, 1.f));

	// ī�޶� ��ġ�� �ٶ󺸰��Ѵ�.
	CTransform* pCameraTransform = static_cast<CTransform*>(static_cast<CCamera*>(CObject_Manager::GetInstance()->Get_GameObject_CloneList(L"Layer_Camera")->front())->Get_Camera_Transform());
	m_pTransformCom->LookAt(XMVectorSetY(pCameraTransform->Get_State(CTransform::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION))));

	// Init Anim State
	m_pCurState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle";
	m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Spawn"; // ��ȯ�ϴ°����� ����.
	m_pImpactState_F = "Goblin_Armor_Mesh.ao|Goblin_SnS_Impact_F";
	m_pImpactState_B = "Goblin_Armor_Mesh.ao|Goblin_SnS_Impact_B";
	m_pModelCom->SetUp_Animation(m_pCurState);

	// ��� ���ʹ� Navigation �ʱ� �ε����� �������Ѵ�
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	return S_OK;
}

_int CGoblin_Armor::Tick(_float fTimeDelta)
{
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	// FSM
	CMonster::DoGlobalState(fTimeDelta);
	UpdateState();

	// Update_Animation
	{
		if (m_bSpawning == false)
			m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_Goblin_Root", m_pNaviCom, m_eDir, 0);
		else
		{
			_float4x4 forDontMoveInWorld;
			XMStoreFloat4x4(&forDontMoveInWorld, XMMatrixIdentity());
			m_pModelCom->Update_Animation(fTimeDelta, &forDontMoveInWorld);
		}
	}
	DoState(fTimeDelta);

//#ifdef _DEBUG
//	_uint keyFrameIdx = m_pModelCom->Get_Current_KeyFrame_Index(m_pCurState);
//	cout << "[Tick] : " << m_pCurState << keyFrameIdx << endl;
//#endif

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

	Render_Goblin(iPassIndex);
	
	return S_OK;
}

HRESULT CGoblin_Armor::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
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

		m_pVIHpBarGsBufferCom->Render(0);
		RELEASE_INSTANCE(CGameInstance);
	}

	m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

#ifdef _DEBUG
	// ��� ���ʹ� Collider�� render�Ѵ�
	__super::Render_Colliders();
#endif

	return S_OK;
}


void CGoblin_Armor::Render_Goblin(_uint iPassIndex)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Render Sphere
	{
		// Bind Matrix
		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix);
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
		_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix);

		// �̰� �����ΰ� ����...
		_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) /** OffsetMatrix*/ * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
		_float4x4	modelWorldMat;
		XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(TransformationMatrix));
		m_pModelSpearCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));

		if (iPassIndex == 3) // shadow map
		{
			m_pModelSpearCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
			m_pModelSpearCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
		}
		else
		{
			pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelSpearCom, "g_ViewMatrix");
			pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelSpearCom, "g_ProjMatrix");
		}

		// Branch to Use Normal Mapping 
		m_pModelSpearCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
		m_pModelSpearCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

		_uint	iNumMaterials = m_pModelSpearCom->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelSpearCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelSpearCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelSpearCom->Render(i, iPassIndex);
		}
	}


	// Render Quiver
	{
		// Bind Matrix
		_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_quiverDesc.pBoneMatrix);
		_matrix		OffsetMatrix = XMLoadFloat4x4(&m_quiverDesc.OffsetMatrix);
		_matrix		PivotMatrix = XMLoadFloat4x4(&m_quiverDesc.PivotMatrix);
		_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_quiverDesc.pTargetWorldMatrix);

		_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(90)) * XMMatrixRotationY(XMConvertToRadians(180)) * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
		_float4x4	modelWorldMat;
		XMStoreFloat4x4(&modelWorldMat, XMMatrixTranspose(TransformationMatrix));
		m_pModelQuiverCom->Set_RawValue("g_WorldMatrix", &modelWorldMat, sizeof(_float4x4));

		if (iPassIndex == 3) // shadow map
		{
			m_pModelQuiverCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
			m_pModelQuiverCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
		}
		else
		{
			pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelQuiverCom, "g_ViewMatrix");
			pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelQuiverCom, "g_ProjMatrix");
		}

		// Branch to Use Normal Mapping 
		m_pModelQuiverCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
		m_pModelQuiverCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));

		_uint	iNumMaterials = m_pModelQuiverCom->Get_NumMaterials();
		for (_uint i = 0; i < iNumMaterials; ++i)
		{
			m_pModelQuiverCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
			m_pModelQuiverCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);
			m_pModelQuiverCom->Render(i, iPassIndex);
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
	_bool useLastLerp = true;

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
		// ���� ���� Exit�� ���۾��۲����Ѵ�.
		//m_bSuperArmor = false; 
	}
	// ��ȯ ����
	else if (m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Spawn")
	{
		m_bSpawning = false;
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
	// Atk
	else if (
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_01" ||
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_02" ||
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear"
		)
	{
		// �ش� ���¿��� ���� �ݶ��̴� Ű��
		Set_Collider_Attribute(COL_MONSTER_WEAPON, false);
		// ���� ���� enter�� ���� ����
		//m_bSuperArmor = true;
		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
	}
	else if (
		m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Dash_Back"
		)
	{
		m_eDir = OBJECT_DIR::DIR_B;
		isLoop = false;
	}
	else if (m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_Death_01")
	{
		m_bSuperArmor = true; // �������� ���۾Ƹӻ���.
		m_eDir = OBJECT_DIR::DIR_B;
		isLoop = false;
	}
	// Impack States
	else if (m_pNextState == m_pImpactState_B)
	{
		m_eDir = OBJECT_DIR::DIR_B;
		isLoop = false;
	}
	else if (m_pNextState == m_pImpactState_F)
	{
		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
	}
	// Spawn State
	else if (m_pNextState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Spawn")
	{
		m_eDir = OBJECT_DIR::DIR_F;
		isLoop = false;
		useLastLerp = false;
		m_bSpawning = true;
	}

	m_pModelCom->SetUp_Animation(m_pNextState, isLoop, useLastLerp);
	// ������ ���� ���°� �ǰ� ���¶��, �������� ������Ʈ�� ���� �ʴ´�. F->B->F->B �ݺ� �̽�
	//if (m_pCurState != m_pImpactState_B && m_pCurState != m_pImpactState_F)
	//	m_pPreState = m_pCurState; // �ǰ� ���°� ������ ��������(m_pPreState)�� �ٽ� ���ư���. 

	m_pCurState = m_pNextState;
}

void CGoblin_Armor::DoState(float fTimeDelta)
{
	//-----------------------------------------------------
	if (m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle")
	{
		m_bSpearFired = false;

		// �÷��̾ ���� �ݰ� ���� �ִٸ� ��������
		_float disToTarget = Get_Target_Dis(m_pTargetTransform);
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
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_02")
	{
		m_bNotSpearAtk = false;
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			//m_pTransformCom->LookAt(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION));

			m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle";
		}
	}
	//-----------------------------------------------------
	else if (
		m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_Spear")
	{
		m_bNotSpearAtk = false;

//#ifdef _DEBUG
//		cout << "[Attack_Spear] : " << keyFrameIdx << endl;
//#endif
		_uint keyFrameIdx = m_pModelCom->Get_Current_KeyFrame_Index(m_pCurState);

		// 1 ������ �϶� Ÿ�����Ѵ�.
		if (keyFrameIdx == 1)
		{
			// �÷��̾� �������� Ÿ�����Ѵ�
			XMStoreFloat4(&m_vTargetDir, (m_pTargetTransform->Get_State(CTransform::STATE_POSITION)+XMVectorSet(0.f, 0.5f, 0.f, 0.f)) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}

		// 29 Ű������ �϶� â�� �����Ѵ�. 
		if (m_bSpearFired == false && (keyFrameIdx == 29))
		{
			// Bind Matrix
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_spearDesc.pBoneMatrix);
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_spearDesc.OffsetMatrix);
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_spearDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_spearDesc.pTargetWorldMatrix);

			// �̰� �����ΰ� ����...
			_matrix		TransformationMatrix = XMMatrixRotationX(XMConvertToRadians(-90)) /** OffsetMatrix*/ * CombinedTransformationMatrix * PivotMatrix * TargetWorldMatrix;
			_float4x4	modelWorldMat;
			XMStoreFloat4x4(&modelWorldMat, TransformationMatrix);

			CGoblin_Spear::CLONEDESC initGoblinSpear;
			// â�� ��ġ�� �����ش�. 
			memcpy(&initGoblinSpear.initPos, (_float4*)modelWorldMat.m[3], sizeof(_float4));
			// â�� ������ �����Ͽ� ��������. 
			initGoblinSpear.initDir = m_vTargetDir;

			if (FAILED(CObject_Manager::GetInstance()->Add_GameObjectToLayer(LEVEL_GAMEPLAY, L"Layer_Goblin_Spear",
				L"Prototype_GameObject_Goblin_Spear", &initGoblinSpear)))
				assert(0);
			m_bSpearFired = true; // Idle �����϶� Ǯ�ų�, �ǰݵǾ����� Ǭ��.
		}

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle";
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Run_F")
	{
		// �����ϴٰ� ���� �ݰ� ���� �ִٸ� ����  
		if (Get_Target_Dis(m_pTargetTransform) < ATK_RANGE)
		{
			m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
			int randAtk = rand() % 2;
			if (randAtk == 0) m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_01";
			else if (randAtk == 1) m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Attack_02";
		}
		// ������ War �������� ���鼭 go Straight
		else
		{
			m_pTransformCom->TurnTo_AxisY_Degree(GetDegree_Target(m_pTargetTransform), fTimeDelta * 10);
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
	//-------------------------------------------------------
	// �ǰ� ���
	else if (m_pCurState == m_pImpactState_F || m_pCurState == m_pImpactState_B)
	{
		m_bSpearFired = false; 
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			//m_pNextState = m_pPreState; // �ǰݾִϸ��̼� ������ �������·� ��������.
			m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle"; // ��.. �׳� IDel�� ����
		}
	}
	//-------------------------------------------------------
	// ��ȯ��
	else if (m_pCurState == "Goblin_Armor_Mesh.ao|Goblin_SnS_Spawn")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle"; 
		}
	}
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
	Safe_Release(m_pModelSpearCom);
	Safe_Release(m_pModelQuiverCom);

	CMonster::Free();
}



















// --------------------------------------------------------------------------------------
// CGoblin_Spear


CGoblin_Spear::CGoblin_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CGoblin_Spear::CGoblin_Spear(const CGoblin_Spear& rhs)
	: CGameObject(rhs)
{
}

HRESULT CGoblin_Spear::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGoblin_Spear::NativeConstruct(void* pArg)
{
	if (SetUp_Component())
		return E_FAIL;

	// pArg�� ���� ��ġ, ����
	if (pArg)
	{
		CLONEDESC* goblinSpearInitDesc = (CLONEDESC*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&goblinSpearInitDesc->initPos));
		m_pTransformCom->Set_Look(XMLoadFloat4(&goblinSpearInitDesc->initDir));
	}

	// ��� â �浹ü
	{
		/* For.GoblinSpear */
		CCollider::COLLIDERDESC		ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vPivot = _float3(0.f, 0.f, .5f);
		ColliderDesc.fRadius = .25f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, false/*���̰� ó�������Ҷ� �ݶ��̴� ���鼭 �����ϴ°���*/);
	}

	return S_OK;
}

_int CGoblin_Spear::Tick(_float fTimeDelta)
{
	if (m_isDead)
		return -1;

	m_fLifeTime += fTimeDelta;

	// ��� â�� ������ ������.
	m_pTransformCom->Go_Straight(fTimeDelta);

	// �浹ü ��ġ Update
	{
		// For Weapon Collider
		for (auto pCollider : m_ColliderList)
		{
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
		}
	}

	return _int();
}

_int CGoblin_Spear::LateTick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (m_fLifeTime > 10.f) // ���� 15�� �̻� ������� ������.
		m_isDead = true;

	if (1)
	{
		if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
			assert(0);
		if (FAILED(m_pRendererCom->Add_PostRenderGroup(this)))
			assert(0);
	}

	// Collider, Layer_War�� 20.f �Ÿ� ������ ��쿡�� �ݶ��̴��Ŵ����� ��ϵȴ�
	pGameInstance->Add_Collision(this, true, m_pTransformCom, L"Layer_War", 20.f);



	RELEASE_INSTANCE(CGameInstance);
	return 0;
}

HRESULT CGoblin_Spear::Render(_uint iPassIndex)
{
	if (FAILED(SetUp_ConstantTable(iPassIndex)))
		return E_FAIL;

	// Render
	_uint	iNumMeshContainer = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainer; ++i)
	{
		m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", i, aiTextureType_DIFFUSE);
		m_pModelCom->Set_ShaderResourceView("g_NormalTexture", i, aiTextureType_NORMALS);

		m_pModelCom->Render(i, iPassIndex);
	}

	return S_OK;
}

HRESULT CGoblin_Spear::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	CGameObject::Render_Colliders();

	return S_OK;
}

HRESULT CGoblin_Spear::SetUp_Component()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 20.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(10.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Spear"), TEXT("Com_Model_Goblin_Spear"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoblin_Spear::SetUp_ConstantTable(_uint iPassIndex)
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// Bind Matrix
	m_pTransformCom->Bind_OnShader(m_pModelCom, "g_WorldMatrix");
	if (iPassIndex == 3) // shadow map
	{
		m_pModelCom->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_View())), sizeof(_float4x4));
		m_pModelCom->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(CLight_Manager::GetInstance()->Get_Objects_Light_Proj())), sizeof(_float4x4));
	}
	else
	{
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pModelCom, "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pModelCom, "g_ProjMatrix");
	}

	// Branch to Use Normal Mapping 
	m_pModelCom->Set_RawValue("g_UseNormalMap", &g_bUseNormalMap, sizeof(bool));
	m_pModelCom->Set_RawValue("g_UseEmissiveMap", &g_bUseEmissiveMap, sizeof(bool));
	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}


void CGoblin_Spear::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	if (pDst->Get_ColliderTag() == COL_WAR_BODY1)
	{
		m_isDead = true;
	}
	
}

void CGoblin_Spear::OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
}

void CGoblin_Spear::OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
}

CGoblin_Spear* CGoblin_Spear::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGoblin_Spear* pInstance = new CGoblin_Spear(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CGoblin_Spear");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CGoblin_Spear::Clone(void* pArg)
{
	CGoblin_Spear* pInstance = new CGoblin_Spear(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CGoblin_Spear");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGoblin_Spear::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}