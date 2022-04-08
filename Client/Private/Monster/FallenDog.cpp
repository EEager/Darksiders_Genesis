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
	m_tGameInfo.iMaxHp = 40;
	m_tGameInfo.iHp = m_tGameInfo.iMaxHp;
	m_tGameInfo.iSoul = rand() % 10 + 10;

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
	m_pNextState = "FallenDog_Mesh.ao|FallenDog_Spawn_Channel"; // ��ȯ�ϴ°����� ����.

	// ��� ���ʹ� Navigation �ʱ� �ε����� �������Ѵ�
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	return S_OK;
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
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "MASTER_FallenDog", m_pNaviCom);
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

	// m_pCurState Exit

	// m_pNextState Enter
	_bool isLoop = true;
	_bool useLastLerp = true;
	if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Idle")
	{
		isLoop = true;
	}
	else if (m_pNextState == "FallenDog_Mesh.ao|FallenDog_Spawn_Channel")
	{
		isLoop = false;
		useLastLerp = false;	
		m_bSpawning = true;
	}
	m_pModelCom->SetUp_Animation(m_pNextState, isLoop, useLastLerp);

	m_pCurState = m_pNextState;
}

void CFallenDog::DoState(float fTimeDelta)
{
	if (m_pCurState == "FallenDog_Mesh.ao|FallenDog_Spawn")
	{
		if (m_pModelCom->Get_Animation_isFinished(m_pCurState))
		{
			m_pNextState = "FallenDog_Mesh.ao|FallenDog_Idle";
		}
	}
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
