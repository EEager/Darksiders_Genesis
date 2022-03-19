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

CHollowLord::CHollowLord(const CHollowLord & rhs)
	: CMonster(rhs)
{
}

HRESULT CHollowLord::NativeConstruct_Prototype()
{	

	return S_OK;
}

HRESULT CHollowLord::NativeConstruct(void * pArg)
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
	ColliderDesc.vPivot = _float3(0.f, 5.f, 0.f);
	ColliderDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColliderDesc.eColType = CCollider::COL_TYPE::COL_TYPE_AABB;
	__super::Add_Collider(&ColliderDesc, L"HollowBody");

	// Init test
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(100.f, 0.f, 470.f, 1.f));
	m_pModelCom->SetUp_Animation((_uint)0);

	// ��� ���ʹ� Navigation �ʱ� �ε����� �������Ѵ�
	m_pNaviCom->SetUp_CurrentIdx(m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION));

	return S_OK;
}

_int CHollowLord::Tick(_float fTimeDelta)
{
	// ��� ���ʹ� Collider list �� update�ؾ��Ѵ�
	if (CMonster::Tick(fTimeDelta) < 0)
		return -1;

	// ������ġ��ȭ�� ���࿡ �����Ű�� 
	m_pModelCom->Update_Animation(fTimeDelta, static_cast<CTransform*>(m_pTransformCom)->Get_WorldMatrix_4x4(), "Bone_HL_Root", m_pNaviCom);


	return _int();
}

_int CHollowLord::LateTick(_float fTimeDelta)
{
	// ��� ���ʹ� Height, Renderer, Add_Collider
	if (CMonster::LateTick(fTimeDelta) < 0)
		return -1;

	return _int();
}

HRESULT CHollowLord::Render()
{
	// ��� ���ʹ� SetUp_ConstantTable, RenderColliders
	if (CMonster::Render() < 0)
		return -1;

	return S_OK;
}


CHollowLord * CHollowLord::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CHollowLord*		pInstance = new CHollowLord(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CHollowLord");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CHollowLord::Clone(void* pArg)
{
	CHollowLord*		pInstance = new CHollowLord(*this);

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
