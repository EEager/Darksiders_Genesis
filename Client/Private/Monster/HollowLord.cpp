#include "stdafx.h"
#include "..\public\Monster\HollowLord.h"
#include "GameInstance.h"

// �Ʒ� 3�������� �����ϸ�ǳ� 
// 1. ��Com
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
	// ��� ���ʹ� m_pTransformCom, m_pRendererCom, m_pNaviCom�� ������
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

	// ��� ���ʹ� Navigation �ʱ� �ε����� �������Ѵ�
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));


	return S_OK;
}

_int CHollowLord::Tick(_float fTimeDelta)
{
	// ��� ���ʹ� Collider list �� update�ؾ��Ѵ�
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	// �÷��̾� �Ÿ� üũ�ؼ�, �����Ÿ� ���Ϸ� ���ö� Lord�� �����Ű��.
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
	// ������ġ��ȭ�� ���࿡ �����Ű�� 
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_HL_Root", m_pNaviCom, m_eDir);

	DoState(fTimeDelta);

	return _int();
}

_int CHollowLord::LateTick(_float fTimeDelta)
{
	// ��� ���ʹ� Height, Renderer, Add_Collider 
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// �÷��̾�� �����Ÿ� ���ϰ� �Ǹ� �׶����� ����������
	if (m_bBattleStart == true)
	{
		//if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		//	return 0;

		// ��� ���ʹ� �ڱⰡ ������ �ִ� Collider list�� collider manager�� ����Ͽ� �浹ó���� �����Ѵ�
		pGameInstance->Add_Collision(this, true, m_pTransformCom, L"Layer_War", 20.f);

	}

	// ü���� 0���ϰ� �Ǹ� ����. 
	//if (m_tGameInfo.iHp <= 0)
	//	m_isDead = true;


	RELEASE_INSTANCE(CGameInstance);

	return _int();
}

HRESULT CHollowLord::Render(_uint iPassIndex)
{
	// ��� ���ʹ� SetUp_ConstantTable, RenderColliders
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
		// idle ���¿��� ��� �����ٰ� �������� 
		if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY)
		{
			//m_pTransformCom->LookAt(m_pTargetTransform->Get_State(CTransform::STATE::STATE_POSITION));

			_float disToTarget = Get_Target_Dis();

			// �Ÿ��� ��� ���� �ָ� ���Ÿ� ����
			if (disToTarget > ATK_RANGE)
			{
				int randNextState = rand() % 2;
				if (randNextState == 0)	m_pNextState = "HollowLord.ao|HollowLord_Atk_Barrage";
				else if (randNextState == 1) m_pNextState = "HollowLord.ao|HollowLord_Atk_DoubleSlam";
			}
			else // ���� �������� ���� ������ ����
			{
				// �ϴ��� �����ϰ� : TODO : �÷��̾� ���� üũ�ؼ� ����, ���� �ϴ°�����
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
			// ���� �ִϸ��̼� ������ Idle�� 
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
	// Ÿ�ٰ��� �Ÿ��� ���Ѵ�
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
