#include "stdafx.h"
#include "..\public\Monster\FallenDog.h"
#include "GameInstance.h"

// �Ʒ� 3�������� �����ϸ�ǳ� 
// 1. ��Com
// 2. master bone name
// 3. collider 

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

	// �ӵ�
	m_fSpeed = 5.f;

	// ��� ���ʹ� m_pTransformCom, m_pRendererCom, m_pNaviCom�� ������
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;	

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_FallenDog"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, 1.75f, 0.f);
	ColliderDesc.vSize = _float3(3.f, 3.5f, 3.f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, COL_MONSTER_BODY1);

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

		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt;

		// �ǰݽ� �ǰݸ������ õ������.
		// ������ ���۾Ƹ� ���¿����� �ǰݻ��·� õ�̰� �ȵȴ�. �������� �Դ´�. 
		if (m_bSuperArmor == false)
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
	CMonster::DoGlobalState(fTimeDelta);
	UpdateState();
	// update animation
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "MASTER_FallenDog", m_pNaviCom, m_eDir);
	DoState(fTimeDelta);

	return _int();
}

_int CFallenDog::LateTick(_float fTimeDelta)
{
	// ��� ���ʹ� Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	// ü���� 0���ϰ� �Ǹ� ����. 
	if (m_tGameInfo.iHp <= 0)
		m_isDead = true;

	return _int();
}

HRESULT CFallenDog::Render(_uint iPassIndex)
{
	// ��� ���ʹ� SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	return S_OK;
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
		m_bSuperArmor = false;
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
		m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_Slash_R")
	{
		// �÷��̾� �ٶ󺸰� �ѵ�. ���ݼ���.
		m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
		isLoop = false;
		m_eDir = OBJECT_DIR::DIR_F;
		m_bSuperArmor = true;
	}
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Atk_GroundSlam")
	{
		// �÷��̾� �ٶ󺸰� �ѵ�. ���ݼ���.
		m_pTransformCom->LookAt(XMVectorSetY(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION), XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION))));
		isLoop = false;
		m_eDir = OBJECT_DIR::DIR_U;
		m_bSuperArmor = true;
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
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Evade_L")
	{
		m_eDir = OBJECT_DIR::DIR_L;
		isLoop = false;
	}
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Evade_R")
	{
		m_eDir = OBJECT_DIR::DIR_R;
		isLoop = false;
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
			// ���̵�, Evade
			int randState = rand() % 3;
			if (randState==0)
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle";
			else if (randState == 1)
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Evade_L";
			else if (randState == 2)
				m_pNextState = "FallenDog_Mesh.ao|FallenDog_Evade_R";
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
}

int g_rand; // ��¼�� 1������ ��ȯ�Ҳ��� ���� �ӽ÷� �̷��� �մϴ�.
void CFallenDog::ChangeToAtkStateRandom()
{
	int randNextState = g_rand; g_rand = (g_rand + 1) % 6;
	if (randNextState == 0)		 m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_3HitCombo";
	else if (randNextState == 1) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_Breath";
	else if (randNextState == 2) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_GroundSlam";
	else if (randNextState == 3) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_Headbutt";
	else if (randNextState == 4) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_Slash_L";
	else if (randNextState == 5) m_pNextState = "FallenDog_Mesh.ao|FallenDog_Atk_Slash_R";
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
