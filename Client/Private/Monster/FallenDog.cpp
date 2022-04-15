#include "stdafx.h"
#include "..\public\Monster\FallenDog.h"
#include "GameInstance.h"
#include "War.h"

#ifdef USE_IMGUI
#include "imgui_Manager.h"
#endif

CFallenDog::CFallenDog(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CFallenDog::CFallenDog(const CFallenDog & rhs)
	: CMonster(rhs)
{
}

HRESULT CFallenDog::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CFallenDog::NativeConstruct(void * pArg)
{
	// GameInfo Init
	m_tGameInfo.iAtt = 2;
	m_tGameInfo.iEnergy = rand() % 10 + 10;
	m_tGameInfo.iMaxHp = 100;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 10;

	m_fFollwingHP = m_tGameInfo.iMaxHp;

	// �ӵ�
	m_fSpeed = 5.f;

	// ��� ���ʹ� m_pTransformCom, m_pRendererCom, m_pNaviCom�� ������
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;	

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FallenDog"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_MonsterHp_PointGS"), TEXT("Com_VIBuffer"), (CComponent**)m_pVIHpBarGsBufferCom.GetAddressOf())))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 1.75f, 0.f);
	ColliderDesc.vSize = _float3(3.f, 3.5f, 3.f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, COL_MONSTER_BODY1);

	// For Weapon
	{
		/* For.Dog Right Hand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f); // For Test
		ColliderDesc.fRadius = 1.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);

		ZeroMemory(&m_DogRightHandDesc, sizeof(WEAPONDESC));
		m_DogRightHandDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_Hand_R_FD");
		m_DogRightHandDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_Hand_R_FD");
		m_DogRightHandDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_DogRightHandDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

		/* For.Dog Left Hand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f); // For Test
		ColliderDesc.fRadius = 1.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);

		ZeroMemory(&m_DogLeftHandDesc, sizeof(WEAPONDESC));
		m_DogLeftHandDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_Hand_L_FD");
		m_DogLeftHandDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_Hand_L_FD");
		m_DogLeftHandDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_DogLeftHandDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

		/* For.Dog Face */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f); // For Test
		ColliderDesc.fRadius = 1.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);

		ZeroMemory(&m_DogFaceDesc, sizeof(WEAPONDESC));
		m_DogFaceDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_Face_FD");
		m_DogFaceDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_Face_FD");
		m_DogFaceDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
		m_DogFaceDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	}

	// pArg�� ���� ��ġ�̴�. w�� 1�� _float4�̴�.
	if (pArg)
	{
		_float4* ArgPos = (_float4*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(ArgPos));
	}
	else
		// Init test
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%20, 0.f, 431.f + rand() % 20, 1.f));

	m_pCurState = "FallenDog_Mesh.ao|Legion_Idle";
	m_pNextState = "FallenDog_Mesh.ao|FallenDog_Sleeping"; // �ڴ� ������ ����.
	m_pImpactState_F = "FallenDog_Mesh.ao|FallenDog_Impact_Front";
	m_pImpactState_B = "FallenDog_Mesh.ao|FallenDog_Impact_Back";

	// ��� ���ʹ� Navigation �ʱ� �ε����� �������Ѵ�
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));


	// ũ��� 1.5��� Ű����
	m_pTransformCom->Set_Scale(_float3(1.5f, 1.5f, 1.5f));


	return S_OK;
}

void CFallenDog::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// ���� ����� �÷��̾� ���� �浹�� ���. 
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_MONSTER_BODY1 &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// �ǰ� ���ߴ�. 
		m_bHitted = true;
		m_fHitPower = .8f;

		if (m_bStiffnessRecovery == false) // ȸ�����¿����� ������������.
			m_fStiffness -= 2.5f;

		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt;

		if (m_bWillDead == false)
		{
			// �������� 0�� �Ǹ� �ִϸ��̼��� �����Ѵ�.
			if (m_fStiffness <= 0)
			{
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Impact_Heavy_F";
			}
			else // �ǰ� ���
			{
				if (m_bSuperArmor == false) // ������ ���۾Ƹ� ���¿����� �ǰݻ��·� õ�̰� �ȵȴ�. �������� �Դ´�. 
				{
					if (m_pImpactState_B != nullptr || m_pImpactState_F != nullptr) // �ǰ� �ִϸ��̼� ������ ����~
					{
						assert(m_pTargetTransform); // Something Wrong...
						if (isTargetFront(m_pTargetTransform))
							// �÷��̾ �� �տ� ������ m_pImpactState_B, �ƴѰ�� m_pImpactState_F
							m_pNextState = m_pImpactState_B;
						else
							m_pNextState = m_pImpactState_F;
					}
				}
			}
		}

		return;
	}
}

_int CFallenDog::Tick(_float fTimeDelta)
{
	// ��� ���ʹ� Collider list �� update�ؾ��Ѵ�
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;
	// 
	// FSM
	// 
	DoGlobalState(fTimeDelta);
	UpdateState();

	if (m_bExecutionAnimEnd == false) // ó�� �ִϸ��̼� ������ ������������ ������ �ִ�.
	{
		// update animation
		if (m_bExecutionAnim) // ó������϶��� ���� ������������.
		{
			m_pModelCom->Update_Animation(fTimeDelta);
		}
		else
		{
			m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "MASTER_FallenDog", m_pNaviCom, m_eDir, 4);
		}
	}
	else
	{
		m_fFinalDeadTime += fTimeDelta;
		if (m_fFinalDeadTime > 3.f)
		{
			// ���ʵڿ� ��¥�� ����. 
			m_isDead = true;
			m_fFinalDeadTime = 0.f;
		}
	}
	DoState(fTimeDelta);

	// �׷α� ���°� �Ǿ��� ���, �������� ������ ������Ű��.
	if (m_bStiffnessRecovery)
	{
		m_fStiffness += 0.03f;
		if (m_fStiffness >= MAX_STIFFNESS)
		{
			m_bStiffnessRecovery = false;
			m_fStiffness = MAX_STIFFNESS;
		}
	}

	// m_fFollwingHP�� ���� ü���� ���󰣴�.
	m_fFollwingHP -= 0.1f;
	if (m_fFollwingHP <= (_float)m_tGameInfo.iHp)
		m_fFollwingHP = (_float)m_tGameInfo.iHp;

	return _int();
}

_int CFallenDog::LateTick(_float fTimeDelta)
{
	// ��� ���ʹ� Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	// ü���� 0���ϰ� �Ǹ� ó�� ����� ��������.
	if (m_tGameInfo.iHp <= 0 && m_bWillDead == false)
	{
		m_bWillDead = true;
		m_pNextState = "FallenDog_Mesh.ao|FallenDog_IA_Death_War";
	}

	return _int();
}

HRESULT CFallenDog::Render(_uint iPassIndex)
{
	// ��� ���ʹ� SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	return S_OK;
}

HRESULT CFallenDog::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	if (m_tGameInfo.iHp != m_tGameInfo.iMaxHp) // �ǰ� �� �޸� ��������
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		//---------------------
		// HP Bar Render
		// Bind Transform
		m_pTransformCom->Bind_OnShader(m_pVIHpBarGsBufferCom.Get(), "g_WorldMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_VIEW, m_pVIHpBarGsBufferCom.Get(), "g_ViewMatrix");
		pGameInstance->Bind_Transform_OnShader(CPipeLine::TS_PROJ, m_pVIHpBarGsBufferCom.Get(), "g_ProjMatrix");
		// Bind Position
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_vector));

		// Bind ���� ���� ü�º����� UV x ��ǥ�� �Ѱ�����. ex) 0.01 ~ 0.99 �̴ϱ� 80% �� (0.99 - 0.01) * 0.8;
		_float fCurHpRatio = (_float)m_tGameInfo.iHp / (_float)m_tGameInfo.iMaxHp;
		_float fUVx = MAX_LEN_HP * fCurHpRatio;
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fMonsterHpUVX", &fUVx, sizeof(_float));
		_float fHpBarHeight = 3.25f;
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fHpBarHeight", &fHpBarHeight, sizeof(_float));
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_vHpBarColorBorder", &XMVectorSet(1.f, 0.f, 0.f, 1.f), sizeof(_vector));
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_vHpBarColor", &XMVectorSet(1.f, 0.f, 0.f, 1.f), sizeof(_vector));

		// ���� ü���� ����ٴϴ� ��� ü�µ� ������
		_float fCurWhiteHpRatio = m_fFollwingHP / (_float)m_tGameInfo.iMaxHp;
		fUVx = MAX_LEN_HP * fCurWhiteHpRatio;
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fMonsterHpUVX_White_Follow", &fUVx, sizeof(_float));

		m_pVIHpBarGsBufferCom.Get()->Render(0);


		m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);

		//---------------------
		// Stiffness Bar Render
		// Bind Transform
		_float fCurStiffnessRatio = (_float)m_fStiffness / (_float)MAX_STIFFNESS;
		fUVx = MAX_LEN_HP * fCurStiffnessRatio;
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fMonsterHpUVX", &fUVx, sizeof(_float));
		fHpBarHeight = 3.4f;
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fHpBarHeight", &fHpBarHeight, sizeof(_float));
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_vHpBarColorBorder", &XMVectorSet(.5f, .5f, .5f, 1.f), sizeof(_vector));
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_vHpBarColor", &XMVectorSet(.5f, .5f, .5f, 1.f), sizeof(_vector));
		fUVx = -1.f; // ��� ü�� ������.
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fMonsterHpUVX_White_Follow", &fUVx, sizeof(_float));


		m_pVIHpBarGsBufferCom.Get()->Render(0);
		m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
		RELEASE_INSTANCE(CGameInstance);
	}

#ifdef _DEBUG
	// ��� ���ʹ� Collider�� render�Ѵ�
	__super::Render_Colliders();
#endif

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI ���� ��ġ�ҰŴ�
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);
	}
#endif

	return S_OK;
}

void CFallenDog::DoGlobalState(float fTimeDelta)
{
	// �ǰ� ���̴�.
	if (m_bHitted)
	{
		// �ǰ����̸� ���̴��� m_fHitPower�� ����, �ǰ� ȿ���� ����.
		m_fHitPower -= 0.01f;
		if (m_fHitPower < 0)
		{
			m_fHitPower = 0.f;
			m_bHitted = false;
		}
	}
}


void CFallenDog::UpdateState()
{
	if (m_pCurState == m_pNextState)
		return;

	// -----------------------------------------------------------------
	// m_pCurState Exit
	if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_3HitCombo" ||
		m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_Breath" ||
		m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_Headbutt" ||
		m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_Slash_L" ||
		m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_Slash_R" ||
		m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_GroundSlam")
	{
		// �ش� ���¿��� ���� �ݶ��̴� ����
		Set_Collider_Attribute(COL_MONSTER_WEAPON, true);
	}
	// ���۾Ƹ� �ٽ� On ����
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Evade_L" ||
			 m_pCurState == "FallenDog_Mesh.ao|FallenDog_Evade_R" || 
			 m_pCurState == "FallenDog_Mesh.ao|FallenDog_Idle")
	{
		m_bSuperArmor = true;
	}

	// -----------------------------------------------------------------
	// m_pNextState Enter
	_bool isLoop = true;
	if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Sleeping_GetUp" || 
		m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_GroundSlam")
	{
		isLoop = false;
	}
	// Atk State
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_3HitCombo" ||
		m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_Breath" ||
		m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_Headbutt" ||
		m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_Slash_L" ||
		m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_Slash_R" ||
		m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_GroundSlam")
	{
		// �÷��̾� �ٶ󺸰� �ѵ�. ���ݼ���.
		m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
		isLoop = false;
		m_eDir = OBJECT_DIR::DIR_F;
		
		// �ش� ���¿��� ���� �ݶ��̴� Ű��
		Set_Collider_Attribute(COL_MONSTER_WEAPON, false);
	}
	// Impact State
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
	// ---
	// Idle
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Idle")
	{
		m_bSuperArmor = false;
	}
	// ---
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Evade_L")
	{
		m_eDir = OBJECT_DIR::DIR_L;
		isLoop = false;
		m_bSuperArmor = false; 
	}
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Evade_R")
	{
		m_eDir = OBJECT_DIR::DIR_R;
		isLoop = false;
		m_bSuperArmor = false;
	}
	// ---
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Impact_Heavy_F")
	{
		m_eDir = OBJECT_DIR::DIR_B;
		isLoop = false;
	}
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_IA_Death_War")
	{
		// War ���� ó�� ������� �ٲ�����.
		static_cast<CWar*>(m_pTarget)->Get_StateMachine()->ChangeState(CState_War_IA_Death_FallenDog::GetInstance());
		m_bExecutionAnim = true; // ó���϶��� ���� ������������.
		isLoop = false;

		// War �������� Dog ��ġ�� ��������.
		{	
			// War�� ���ֺ��� �Ѵ�.
			m_pTransformCom->Set_Look(-1*m_pTargetTransform->Get_State(CTransform::STATE_LOOK));

			auto toPosition = m_pTargetTransform->Get_State(CTransform::STATE_POSITION)
				+ m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * .55f
				+ m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -.2f
				+ XMVectorSet(0.f, -1.f, 0.f, 0.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, toPosition);
		}
	}


	m_pModelCom->SetUp_Animation(m_pNextState, isLoop);

	m_pCurState = m_pNextState;
}

void CFallenDog::DoState(float fTimeDelta)
{
	// -----------------------------------------------------------------
	// �ڴٰ� �÷��̾ ��ó�� ���� �Ͼ ��, ���� �����ѹ�������.
	if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Sleeping")
	{
		_float disToTarget = Get_Target_Dis(m_pTargetTransform);
		if (disToTarget <= WAKEUP_RANGE)
		{
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Sleeping_GetUp";
			return;
		}
	}
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Sleeping_GetUp")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_GroundSlam";
		}
	}
	// ------------------------------------------------------------------
	// Idle
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Idle")
	{
		m_fTimeIdle += fTimeDelta;
		// �÷��̾ �ݰ� ���� �ִٸ� �����ϰų� ����. �ٵ� �ٷ� ������ ���� �� �����ٰ� ����.
		if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY)
		{
			_float disToTarget = Get_Target_Dis(m_pTargetTransform);
			if (disToTarget < ATK_RANGE)
			{
				ChangeToAtkStateRandom();
				m_fTimeIdle = 0.f;
			}
			else if (disToTarget < CHASE_RANGE)
			{
				// �÷��̾ ���� �ݰ� �ȿ� �ִٸ� ���� 
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Run_F";
				m_fTimeIdle = 0.f;
			}
		}
	}
	// -----------------------------------------------------------------------
	// Atk States
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_3HitCombo" ||
		     m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_Breath" ||
		     m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_GroundSlam" ||
		     m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_Headbutt" ||
		     m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_Slash_L" ||
		     m_pCurState == "FallenDog_Mesh.ao|FallenDog_Atk_Slash_R")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			if (m_bFirstGoIdle == false) // ��ó������ Idle�� ��������
			{
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle";
				m_bFirstGoIdle = true;
				return;
			}

			// ���̵�, Evade
			int randState = rand() % 4;
			if (randState == 1)
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Evade_L";
			else if (randState == 2)
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Evade_R";
			else
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle";

		}
	}
	// ----------------------------------------------------------------------
	// Run
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Run_F")
	{
		// �����ϴٰ� ���� �ݰ� ���� �ִٸ� ����  
		if (Get_Target_Dis(m_pTargetTransform) < ATK_RANGE)
		{
			ChangeToAtkStateRandom();
		}
		// ������ War �������� ���鼭 go Straight
		else
		{
			m_pTransformCom->TurnTo_AxisY_Degree(GetDegree_Target(m_pTargetTransform), fTimeDelta * 10);
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNaviCom);
		}
	}
	//-------------------------------------------------------
	// �ǰ� ���
	else if (m_pCurState == m_pImpactState_F || m_pCurState == m_pImpactState_B)
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle"; 
		}
	}
	//-------------------------------------------------------
	// Evade
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Evade_L")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle";
		}
	}
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Evade_R")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle";
		}
	}
	//-----------------------------------------------------------
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Impact_Heavy_F")
	{
		// �׷α� ���� �Ͼ�� �Ҷ� �������� ������Ű��
		if (m_bStiffnessRecovery == false)
		{
			_uint iKeyFrameIdx = m_pModelCom->Get_Current_KeyFrame_Index(m_pCurState);
			if (iKeyFrameIdx > 106)
				m_bStiffnessRecovery = true;
		}

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle";
		}
	}
	//-----------------------------------------------------------
	else if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_IA_Death_War")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_bExecutionAnimEnd = true;
		}
	}
}

void CFallenDog::ChangeToAtkStateRandom()
{
	int randNextState = m_AtkRandNum; m_AtkRandNum = (m_AtkRandNum + 1) % 4;
	if (randNextState == 0)		 m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_3HitCombo";
	//else if (randNextState == 1) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_Breath";
	//else if (randNextState == 2) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_GroundSlam";
	else if (randNextState == 1) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_Headbutt";
	else if (randNextState == 2) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_Slash_L";
	else if (randNextState == 3) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_Slash_R";
}

_int CFallenDog::Update_Colliders(_matrix wolrdMatrix/*not used*/)
{
	int idx = 0;
	for (auto& pCollider : m_ColliderList)
	{
		if (idx == 1) // ������ ��
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_DogRightHandDesc.OffsetMatrix); // ��->����
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_DogRightHandDesc.pBoneMatrix); // Root->�� 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_DogRightHandDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_DogRightHandDesc.pTargetWorldMatrix); // just legion's world matrix
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix�� �Ȱ��ϴϰ� ���� �Ǵ°� ����... ��?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else if (idx == 2) // ���� ��
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_DogLeftHandDesc.OffsetMatrix); // ��->����
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_DogLeftHandDesc.pBoneMatrix); // Root->�� 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_DogLeftHandDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_DogLeftHandDesc.pTargetWorldMatrix); // just legion's world matrix
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix�� �Ȱ��ϴϰ� ���� �Ǵ°� ����... ��?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else if (idx == 3) // ��
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_DogFaceDesc.OffsetMatrix); // ��->����
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_DogFaceDesc.pBoneMatrix); // Root->�� 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_DogFaceDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_DogFaceDesc.pTargetWorldMatrix); // just legion's world matrix
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix�� �Ȱ��ϴϰ� ���� �Ǵ°� ����... ��?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else // idx == 0 ����
		{
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
		}
		idx++;
	}

	return 0;
}

CFallenDog * CFallenDog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CFallenDog*		pInstance = new CFallenDog(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CFallenDog");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CFallenDog::Clone(void* pArg)
{
	CFallenDog*		pInstance = new CFallenDog(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CFallenDog");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFallenDog::Free()
{
	CMonster::Free();
}
