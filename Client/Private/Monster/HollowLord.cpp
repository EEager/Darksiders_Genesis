#include "stdafx.h"
#include "..\public\Monster\HollowLord.h"
#include "GameInstance.h"

// 아래 3개정도만 수정하면되네 
// 1. 모델Com
// 2. master bone name
// 3. collider 

CHollowLord::CHollowLord(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CHollowLord::CHollowLord(const CHollowLord& rhs)
	: CMonster(rhs)
{
}

HRESULT CHollowLord::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CHollowLord::NativeConstruct(void* pArg)
{
	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HollowLord"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 15.f, 0.f);
	ColliderDesc.vSize = _float3(20.f, 30.f, 20.f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, L"HollowBody");

	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(125.f, -7.f, 467.f, 1.f));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(30.f));

	// Init Anim State

	m_pCurState = "HollowLord.ao|HollowLord_Emerge";
	m_pNextState = "HollowLord.ao|HollowLord_Emerge";
	m_pModelCom->SetUp_Animation("HollowLord.ao|HollowLord_Emerge", false);

	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));


	return S_OK;
}

_int CHollowLord::Tick(_float fTimeDelta)
{
	// 모든 몬스터는 Collider list 를 update해야한다
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	// 플레이어 거리 체크해서, 일정거리 이하로 들어올때 Lord를 등장시키자.
	if (m_pTarget && m_bBattleStart == false && Get_Target_Dis() > INIT_RANGE)
	{
		m_pModelCom->SetUp_Animation("HollowLord.ao|HollowLord_Emerge", false);
		return 0;
	}
	else
		m_bBattleStart = true;

	// FSM
	CMonster::DoGlobalState(fTimeDelta);
	UpdateState();
	// 로컬위치변화를 월행에 적용시키자 
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_HL_Root", m_pNaviCom, m_eDir);

	DoState(fTimeDelta);

	return _int();
}

_int CHollowLord::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider 
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// 플레이어와 일정거리 이하가 되면 그때부터 렌더링하자
	if (m_bBattleStart == true)
	{
		//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		//	return 0;

		// 모든 몬스터는 자기가 가지고 있는 Collider list를 collider manager에 등록하여 충돌처리를 진행한다
		pGameInstance->Add_Collision(this, true, m_pTransformCom, L"Layer_War", 20.f);

	}

	// 체력이 0이하가 되면 죽자. 
	//if (m_tGameInfo.iHp <= 0)
	//	m_isDead = true;


	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CHollowLord::Render(_uint iPassIndex)
{
	// 모든 몬스터는 SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	return S_OK;
}

void CHollowLord::UpdateState()
{
	if (m_pCurState == m_pNextState)
		return;

	_bool isLoop = false;

	// m_eCurState Exit


	// m_eNextState Enter
	if (m_pNextState == "HollowLord.ao|HollowLord_Idle" ||
		m_pNextState == "HollowLord.ao|Legion_Run_F"
		)
	{
		isLoop = true;
	}
	else if (
		m_pNextState == "HollowLord.ao|HollowLord_Atk_Barrage" ||
		m_pNextState == "HollowLord.ao|HollowLord_Atk_DoubleSlam" ||
		m_pNextState == "HollowLord.ao|HollowLord_Atk_Slam_L" ||
		m_pNextState == "HollowLord.ao|HollowLord_Atk_Slam_R" ||
		m_pNextState == "HollowLord.ao|HollowLord_Atk_Swipe_L" ||
		m_pNextState == "HollowLord.ao|HollowLord_Atk_Swipe_R"
		)
	{
		isLoop = false;
	}
	else if (m_pNextState == "HollowLord.ao|HollowLord_Emerge")
	{
		isLoop = false;
	}

	m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
	m_pCurState = m_pNextState;
}


void CHollowLord::DoState(float fTimeDelta)
{
	//-----------------------------------------------------
	if (m_pCurState == "HollowLord.ao|HollowLord_Idle")
	{
		m_fTimeIdle += fTimeDelta;
		// idle 상태에서 잠시 쉬었다가 공격하자 
		if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY)
		{
			//m_pTransformCom->LookAt(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION));

			_float disToTarget = Get_Target_Dis();

			// 거리가 어느 정도 멀면 원거리 공격
			if (disToTarget > ATK_RANGE)
			{
				int randNextState = rand() % 2;
				if (randNextState == 0)	m_pNextState = "HollowLord.ao|HollowLord_Atk_Barrage";
				else if (randNextState == 1) m_pNextState = "HollowLord.ao|HollowLord_Atk_DoubleSlam";
			}
			else // 근접 범위내면 근접 공격을 하자
			{
				// 일단은 랜덤하게 : TODO : 플레이어 방향 체크해서 레훅, 라훅 하는것으로
				int randNextState = rand() % 4;
				if (randNextState == 0)	m_pNextState = "HollowLord.ao|HollowLord_Atk_Slam_L";
				else if (randNextState == 1) m_pNextState = "HollowLord.ao|HollowLord_Atk_Slam_R";
				else if (randNextState == 2) m_pNextState = "HollowLord.ao|HollowLord_Atk_Swipe_L";
				else if (randNextState == 3) m_pNextState = "HollowLord.ao|HollowLord_Atk_Swipe_R";
			}
		}
	}
	//-----------------------------------------------------
	else if (
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Barrage" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_DoubleSlam" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Slam_L" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Slam_R" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Swipe_L" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Swipe_R" ||
		m_pCurState == "HollowLord.ao|HollowLord_Emerge")
	{

		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			// 공격 애니메이션 끝나면 Idle로 
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
	else if (
		m_pCurState == "HollowLord.ao|HollowLord_Emerge"
		)
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
}

_float CHollowLord::Get_Target_Dis(float fTimeDelta)
{
	// 타겟간의 거리를 구한다
	return XMVectorGetX(XMVector3Length(
		m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION) -
		m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION)));
}

_float CHollowLord::GetDegree_Target()
{
	return _float();
}


CHollowLord* CHollowLord::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CHollowLord* pInstance = new CHollowLord(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CHollowLord");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject* CHollowLord::Clone(void* pArg)
{
	CHollowLord* pInstance = new CHollowLord(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CHollowLord");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHollowLord::Free()
{
	CMonster::Free();
}
