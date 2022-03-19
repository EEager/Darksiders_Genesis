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
	// 모든 몬스터는 m_pTransformCom, m_pRendererCom, m_pNaviCom를 가진다
	if (CMonster::NativeConstruct(pArg))
		return E_FAIL;	

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Goblin_Armor"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Collider */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vPivot = _float3(0.f, .5f, 0.f);
	ColliderDesc.vSize = _float3(0.5f, 1.0f, 0.5f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, L"GoblinBody");

	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(85.f + rand()%10, 0.f, 431.f + rand() % 10, 1.f));
	m_pModelCom->SetUp_Animation((_uint)0);

	// 모든 몬스터는 Navigation 초기 인덱스를 잡아줘야한다
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	return S_OK;
}

_int CGoblin_Armor::Tick(_float fTimeDelta)
{
	// 모든 몬스터는 Collider list 를 update해야한다
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	// Legiondms 
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_Goblin_Root", m_pNaviCom);

	// for test
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.001f);

	return _int();
}

_int CGoblin_Armor::LateTick(_float fTimeDelta)
{
	// 모든 몬스터는 Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	return _int();
}

HRESULT CGoblin_Armor::Render()
{
	// 모든 몬스터는 SetUp_ConstantTable, RenderColliders
	if (CMonster::Render() < 0)
		return -1;

	return S_OK;
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
	CMonster::Free();
}
