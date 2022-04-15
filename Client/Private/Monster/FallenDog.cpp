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

	// 속도
	m_fSpeed = 5.f;

	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다
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

	// pArg는 보통 위치이다. w가 1인 _float4이다.
	if (pArg)
	{
		_float4* ArgPos = (_float4*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(ArgPos));
	}
	else
		// Init test
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%20, 0.f, 431.f + rand() % 20, 1.f));

	m_pCurState = "FallenDog_Mesh.ao|Legion_Idle";
	m_pNextState = "FallenDog_Mesh.ao|FallenDog_Sleeping"; // 자는 것으로 시작.
	m_pImpactState_F = "FallenDog_Mesh.ao|FallenDog_Impact_Front";
	m_pImpactState_B = "FallenDog_Mesh.ao|FallenDog_Impact_Back";

	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));


	// 크기는 1.5배로 키우자
	m_pTransformCom->Set_Scale(_float3(1.5f, 1.5f, 1.5f));


	return S_OK;
}

void CFallenDog::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// 몬스터 몸통과 플레이어 검이 충돌한 경우. 
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_MONSTER_BODY1 &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// 피격 당했다. 
		m_bHitted = true;
		m_fHitPower = .8f;

		if (m_bStiffnessRecovery == false) // 회복상태에서는 감소하지말자.
			m_fStiffness -= 2.5f;

		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt;

		if (m_bWillDead == false)
		{
			// 경직도가 0이 되면 애니메이션을 변경한다.
			if (m_fStiffness <= 0)
			{
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Impact_Heavy_F";
			}
			else // 피격 모션
			{
				if (m_bSuperArmor == false) // 하지만 슈퍼아머 상태에서는 피격상태로 천이가 안된다. 데미지만 입는다. 
				{
					if (m_pImpactState_B != nullptr || m_pImpactState_F != nullptr) // 피격 애니메이션 없으면 무시~
					{
						assert(m_pTargetTransform); // Something Wrong...
						if (isTargetFront(m_pTargetTransform))
							// 플레이어가 내 앞에 있으면 m_pImpactState_B, 아닌경우 m_pImpactState_F
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
	// 모든 몬스터는 Collider list 를 update해야한다
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;
	// 
	// FSM
	// 
	DoGlobalState(fTimeDelta);
	UpdateState();

	if (m_bExecutionAnimEnd == false) // 처형 애니메이션 끝나면 움직이지말고 가만히 있다.
	{
		// update animation
		if (m_bExecutionAnim) // 처형모션일때는 월행 움직이지말자.
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
			// 몇초뒤에 진짜로 죽자. 
			m_isDead = true;
			m_fFinalDeadTime = 0.f;
		}
	}
	DoState(fTimeDelta);

	// 그로기 상태가 되었을 경우, 경직도를 서서히 증가시키자.
	if (m_bStiffnessRecovery)
	{
		m_fStiffness += 0.03f;
		if (m_fStiffness >= MAX_STIFFNESS)
		{
			m_bStiffnessRecovery = false;
			m_fStiffness = MAX_STIFFNESS;
		}
	}

	// m_fFollwingHP는 현재 체력을 따라간다.
	m_fFollwingHP -= 0.1f;
	if (m_fFollwingHP <= (_float)m_tGameInfo.iHp)
		m_fFollwingHP = (_float)m_tGameInfo.iHp;

	return _int();
}

_int CFallenDog::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	// 체력이 0이하가 되면 처형 모션을 진행하자.
	if (m_tGameInfo.iHp <= 0 && m_bWillDead == false)
	{
		m_bWillDead = true;
		m_pNextState = "FallenDog_Mesh.ao|FallenDog_IA_Death_War";
	}

	return _int();
}

HRESULT CFallenDog::Render(_uint iPassIndex)
{
	// 모든 몬스터는 SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	return S_OK;
}

HRESULT CFallenDog::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	if (m_tGameInfo.iHp != m_tGameInfo.iMaxHp) // 피가 좀 달면 보여주자
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

		// Bind 몬스터 현재 체력비율을 UV x 좌표로 넘겨주자. ex) 0.01 ~ 0.99 이니깐 80% 면 (0.99 - 0.01) * 0.8;
		_float fCurHpRatio = (_float)m_tGameInfo.iHp / (_float)m_tGameInfo.iMaxHp;
		_float fUVx = MAX_LEN_HP * fCurHpRatio;
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fMonsterHpUVX", &fUVx, sizeof(_float));
		_float fHpBarHeight = 3.25f;
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fHpBarHeight", &fHpBarHeight, sizeof(_float));
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_vHpBarColorBorder", &XMVectorSet(1.f, 0.f, 0.f, 1.f), sizeof(_vector));
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_vHpBarColor", &XMVectorSet(1.f, 0.f, 0.f, 1.f), sizeof(_vector));

		// 현재 체력을 따라다니는 흰색 체력도 만들자
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
		fUVx = -1.f; // 흰색 체력 사용안함.
		m_pVIHpBarGsBufferCom.Get()->Set_RawValue("g_fMonsterHpUVX_White_Follow", &fUVx, sizeof(_float));


		m_pVIHpBarGsBufferCom.Get()->Render(0);
		m_pDeviceContext->GSSetShader(nullptr, nullptr, 0);
		RELEASE_INSTANCE(CGameInstance);
	}

#ifdef _DEBUG
	// 모든 몬스터는 Collider를 render한다
	__super::Render_Colliders();
#endif

#ifdef USE_IMGUI
	if (m_bUseImGui) // IMGUI 툴로 배치할거다
	{
		CImguiManager::GetInstance()->Transform_Control(m_pTransformCom, m_CloneIdx, &m_bUseImGui);
	}
#endif

	return S_OK;
}

void CFallenDog::DoGlobalState(float fTimeDelta)
{
	// 피격 중이다.
	if (m_bHitted)
	{
		// 피격중이면 셰이더에 m_fHitPower를 던져, 피격 효과를 주자.
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
		// 해당 상태에서 무기 콜라이더 끄자
		Set_Collider_Attribute(COL_MONSTER_WEAPON, true);
	}
	// 슈퍼아머 다시 On 하자
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
		// 플레이어 바라보게 한뒤. 공격수행.
		m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
		isLoop = false;
		m_eDir = OBJECT_DIR::DIR_F;
		
		// 해당 상태에서 무기 콜라이더 키자
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
		// War 또한 처형 모션으로 바꿔주자.
		static_cast<CWar*>(m_pTarget)->Get_StateMachine()->ChangeState(CState_War_IA_Death_FallenDog::GetInstance());
		m_bExecutionAnim = true; // 처형일때는 월행 움직이지말자.
		isLoop = false;

		// War 기준으로 Dog 위치를 조정하자.
		{	
			// War와 마주보게 한다.
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
	// 자다가 플레이어가 근처에 오면 일어난 뒤, 점프 공격한번해주자.
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
		// 플레이어가 반경 내에 있다면 공격하거나 추적. 근데 바로 하지는 말고 좀 쉬었다가 하자.
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
				// 플레이어가 추적 반경 안에 있다면 추적 
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
			if (m_bFirstGoIdle == false) // 맨처음에는 Idle로 가게하자
			{
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle";
				m_bFirstGoIdle = true;
				return;
			}

			// 아이들, Evade
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
		// 추적하다가 공격 반경 내에 있다면 공격  
		if (Get_Target_Dis(m_pTargetTransform) < ATK_RANGE)
		{
			ChangeToAtkStateRandom();
		}
		// 추적은 War 방향으로 돌면서 go Straight
		else
		{
			m_pTransformCom->TurnTo_AxisY_Degree(GetDegree_Target(m_pTargetTransform), fTimeDelta * 10);
			m_pTransformCom->Go_Straight(fTimeDelta, m_pNaviCom);
		}
	}
	//-------------------------------------------------------
	// 피격 모션
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
		// 그로기 이후 일어날랑 할때 경직도를 증가시키자
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
		if (idx == 1) // 오른쪽 팔
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_DogRightHandDesc.OffsetMatrix); // 뼈->정점
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_DogRightHandDesc.pBoneMatrix); // Root->뼈 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_DogRightHandDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_DogRightHandDesc.pTargetWorldMatrix); // just legion's world matrix
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else if (idx == 2) // 왼쪽 팔
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_DogLeftHandDesc.OffsetMatrix); // 뼈->정점
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_DogLeftHandDesc.pBoneMatrix); // Root->뼈 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_DogLeftHandDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_DogLeftHandDesc.pTargetWorldMatrix); // just legion's world matrix
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else if (idx == 3) // 뿔
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_DogFaceDesc.OffsetMatrix); // 뼈->정점
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_DogFaceDesc.pBoneMatrix); // Root->뼈 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_DogFaceDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_DogFaceDesc.pTargetWorldMatrix); // just legion's world matrix
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix를 안곱하니간 뭔가 되는거 같다... 왜?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else // idx == 0 몸통
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
