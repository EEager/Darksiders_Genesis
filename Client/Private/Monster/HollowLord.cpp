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
	// GameInfo Init
	m_tGameInfo.iAtt = 2;
	m_tGameInfo.iEnergy = rand() % 10 + 10;
	m_tGameInfo.iMaxHp = 100;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 10;

	// ��� ���ʹ� m_pTransformCom, m_pRendererCom, m_pNaviCom�� ������
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HollowLord"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ZeroMemory(&m_Lord_RightHandDesc, sizeof(BONEDESC));
	m_Lord_RightHandDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_HL_Hand_R");
	m_Lord_RightHandDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_HL_Hand_R");
	m_Lord_RightHandDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
	m_Lord_RightHandDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();
	ZeroMemory(&m_Lord_LeftHandDesc, sizeof(BONEDESC));
	m_Lord_LeftHandDesc.pBoneMatrix = m_pModelCom->Get_CombinedMatrixPtr("Bone_HL_Hand_L");
	m_Lord_LeftHandDesc.OffsetMatrix = m_pModelCom->Get_OffsetMatrix("Bone_HL_Hand_L");
	m_Lord_LeftHandDesc.PivotMatrix = m_pModelCom->Get_PivotMatrix_Bones();
	m_Lord_LeftHandDesc.pTargetWorldMatrix = m_pTransformCom->Get_WorldFloat4x4Ptr();

	// For Hit Box
	{
		// ���� ��Ʈ�ڽ��� COL_BALLISTA_BODY�� ����� �÷��̾ �����Ͽ����� go back �ϵ�������
		/* For.LordBody */
		ColliderDesc.vPivot = _float3(0.f, 15.f, 0.f);
		ColliderDesc.vSize = _float3(20.f, 30.f, 20.f);
		ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
		__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);

		/* For.LordRightHand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f);
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);

		/* For.LordLeftHand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f);
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_BALLISTA_BODY);
	}

	// For Weapon
	{
		/* For.LordRightHand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f); 
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);

		/* For.LordLeftHand */
		ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f);
		ColliderDesc.fRadius = 5.f;
		ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
		__super::Add_Collider(&ColliderDesc, COL_MONSTER_WEAPON, true);
	}

	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(614.f, 11.f, 142.f, 1.f));

	// Init Anim State
	m_pCurState = "HollowLord.ao|HollowLord_Emerge";
	m_pNextState = "HollowLord.ao|HollowLord_Emerge";
	m_pModelCom->SetUp_Animation("HollowLord.ao|HollowLord_Emerge", false, false);

	// ��� ���ʹ� Navigation �ʱ� �ε����� �������Ѵ�
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	// ���� �¿�������
	m_bHeight = false;

	m_pTransformCom->Set_Scale(_float3(0.5f * 1.5f, 0.5f * 1.5f, 0.5f * 1.5f));

	// Hp Bar�� ����������.
	m_pHpBar = CUI_HollowLord_HpBar::Create(m_pDevice, m_pDeviceContext);
	m_pHpBar->m_pHollowLord = this; 
	Safe_AddRef(m_pHpBar->m_pHollowLord); // �̰� ���߿� hpBar���� ����������

	return S_OK;
}

_int CHollowLord::Tick(_float fTimeDelta)
{
	// ��� ���ʹ� Collider list �� update�ؾ��Ѵ�
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	if (m_isDead) // CMonster::Tick���� -1�� �������ٰ��̴�.
		return 0; 

	// FSM
	CMonster::DoGlobalState(fTimeDelta); // ��Ʈ�Ŀ� ����
	UpdateState();
	// ������ġ��ȭ�� ���࿡ �����Ű��
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_HL_Root", m_pNaviCom, m_eDir);
	DoState(fTimeDelta);

	// Hp�� Tick
	m_pHpBar->Tick(fTimeDelta);

	return _int();
}

_int CHollowLord::LateTick(_float fTimeDelta)
{
	// ��� ���ʹ� Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	// ü���� 0���ϰ� �Ǹ� �״� ����� ��������.
	if (m_tGameInfo.iHp <= 0 && m_bWillDead == false)
	{
		m_bWillDead = true;
		m_pNextState = "HollowLord.ao|HollowLord_Death";
	}
	
	// Hp�� LateTick
	m_pHpBar->LateTick(fTimeDelta);

	return _int();
}

HRESULT CHollowLord::Render(_uint iPassIndex)
{
	// ��� ���ʹ� SetUp_ConstantTable
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	return S_OK;
}


HRESULT CHollowLord::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	// collider Render And ImGUI Render
	CMonster::PostRender(m_spriteBatch, m_spriteFont);

	return S_OK;
}


void CHollowLord::UpdateState()
{
	if (m_pCurState == m_pNextState)
		return;

	_bool isLoop = false;

	// m_eCurState Exit
	if (
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Barrage" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_DoubleSlam" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Slam_L" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Slam_R" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Swipe_L" ||
		m_pCurState == "HollowLord.ao|HollowLord_Atk_Swipe_R"
		)
	{
		// �ش� ���¿��� ���� �ݶ��̴� ����
		Set_Collider_Attribute(COL_MONSTER_WEAPON, true);
	}

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
		// �ش� ���¿��� ���� �ݶ��̴� Ű��
		Set_Collider_Attribute(COL_MONSTER_WEAPON, false);
		isLoop = false;
	}
	else if (m_pNextState == "HollowLord.ao|HollowLord_Emerge" || 
			m_pNextState == "HollowLord.ao|HollowLord_Death")
	{
		isLoop = false;
	}

	m_pModelCom->SetUp_Animation(m_pNextState, isLoop);
	m_pCurState = m_pNextState;
}


int randNextState;
void CHollowLord::DoState(float fTimeDelta)
{
	//-----------------------------------------------------
	if (m_pCurState == "HollowLord.ao|HollowLord_Idle")
	{
		m_fTimeIdle += fTimeDelta;
		// idle ���¿��� ��� �����ٰ� �������� 
		if (m_fTimeIdle > IDLE_TIME_TO_ATK_DELAY)
		{
			if (randNextState == 0)	m_pNextState = "HollowLord.ao|HollowLord_Atk_Barrage";
			else if (randNextState == 1) m_pNextState = "HollowLord.ao|HollowLord_Atk_DoubleSlam";
			else if (randNextState == 2)	m_pNextState = "HollowLord.ao|HollowLord_Atk_Slam_L";
			else if (randNextState == 3) m_pNextState = "HollowLord.ao|HollowLord_Atk_Slam_R";
			else if (randNextState == 4) m_pNextState = "HollowLord.ao|HollowLord_Atk_Swipe_L";
			else if (randNextState == 5) m_pNextState = "HollowLord.ao|HollowLord_Atk_Swipe_R";
			randNextState = (randNextState + 1) % 6; // ���������� �ϳ��� ��������.
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
	////-----------------------------------------------------
	else if (m_pCurState == "HollowLord.ao|HollowLord_Emerge")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "HollowLord.ao|HollowLord_Idle";
		}
	}
	//-----------------------------------------------------
	else if (m_pCurState == "HollowLord.ao|HollowLord_Death")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_isDead = true;
		}
	}
}


_int CHollowLord::Update_Colliders(_matrix wolrdMatrix/*not used*/)
{
	int idx = 0;
	for (auto& pCollider : m_ColliderList)
	{
		if (idx == 1 || idx ==3) // m_Lord_RightHandDesc
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_Lord_RightHandDesc.OffsetMatrix); // ��->����
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_Lord_RightHandDesc.pBoneMatrix); // Root->�� 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_Lord_RightHandDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_Lord_RightHandDesc.pTargetWorldMatrix);
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix�� �Ȱ��ϴϰ� ���� �Ǵ°� ����... ��?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else if (idx == 2 || idx == 4) // m_Lord_LeftHandDesc
		{
			_matrix		OffsetMatrix = XMLoadFloat4x4(&m_Lord_LeftHandDesc.OffsetMatrix); // ��->����
			_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_Lord_LeftHandDesc.pBoneMatrix); // Root->�� 
			_matrix		PivotMatrix = XMLoadFloat4x4(&m_Lord_LeftHandDesc.PivotMatrix);
			_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_Lord_LeftHandDesc.pTargetWorldMatrix);
			_matrix		TransformationMatrix =
				(CombinedTransformationMatrix * PivotMatrix) * //OffsetMatrix�� �Ȱ��ϴϰ� ���� �Ǵ°� ����... ��?
				TargetWorldMatrix;
			pCollider->Update(TransformationMatrix);
		}
		else // LordBody 
		{
			pCollider->Update(m_pTransformCom->Get_WorldMatrix());
		}
		idx++;
	}

	return 0;
}

void CHollowLord::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
	// ���� ����� �÷��̾� ���� �浹�� ���. 
	if (m_bHitted == false && pSrc->Get_ColliderTag() == COL_BALLISTA_BODY &&
		pDst->Get_ColliderTag() == COL_WAR_WEAPON)
	{
		// �ǰ� ���ߴ�. 
		m_bHitted = true;
		m_fHitPower = .8f;

		m_tGameInfo.iHp -= pDst->Get_Owner()->m_tGameInfo.iAtt * 10;
		return;
	}
}

void CHollowLord::UI_Init()
{
	m_pHpBar->m_bInit = true;
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

	Safe_Delete(m_pHpBar);
}
