#include "stdafx.h"
#include "..\public\Monster\FallenDog.h"
#include "GameInstance.h"

// 아래 3개정도만 수정하면되네 
// 1. 모델Com
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
	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다
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
	__super::Add_Collider(&ColliderDesc, L"FallenDogBody");

	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%20, 0.f, 431.f + rand() % 20, 1.f));
	m_pModelCom->SetUp_Animation((_uint)0);

	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	return S_OK;
}

_int CFallenDog::Tick(_float fTimeDelta)
{
	// 모든 몬스터는 Collider list 를 update해야한다
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	// Legiondms 
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "MASTER_FallenDog", m_pNaviCom);

	// for test
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.001f);

	return _int();
}

_int CFallenDog::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	return _int();
}

HRESULT CFallenDog::Render(_uint iPassIndex)
{
	// 모든 몬스터는 SetUp_ConstantTable, RenderColliders
	if (CMonster::Render(iPassIndex) < 0)
		return -1;

	return S_OK;
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
