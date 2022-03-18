#include "stdafx.h"
#include "..\public\Sword.h"

#include "GameInstance.h"


CSword::CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CSword::CSword(const CSword & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSword::NativeConstruct_Prototype()
{
	

	return S_OK;
}

HRESULT CSword::NativeConstruct(void * pArg)
{
	if (FAILED(SetUp_Component()))
		return E_FAIL;


	if (FAILED(SetUp_BoneMatrix()))
		return E_FAIL;

	return S_OK;
}

_int CSword::Tick(_float fTimeDelta)
{
	_matrix		OffsetMatrix = XMLoadFloat4x4(&m_SwordDesc.OffsetMatrix);
	_matrix		CombinedTransformationMatrix = XMLoadFloat4x4(m_SwordDesc.pBoneMatrix);
	_matrix		PivotMatrix = XMLoadFloat4x4(&m_SwordDesc.PivotMatrix);
	_matrix		TargetWorldMatrix = XMLoadFloat4x4(m_SwordDesc.pTargetWorldMatrix);

	_matrix		TransformationMatrix = m_pTransformCom->Get_WorldMatrix() * (OffsetMatrix * CombinedTransformationMatrix * PivotMatrix) * TargetWorldMatrix;
			
	m_pSphereCom->Update(TransformationMatrix);

	return _int();
}

_int CSword::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this)))
		return 0;

	RELEASE_INSTANCE(CGameInstance);	

	return _int();
}

HRESULT CSword::Render()
{
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;	

#ifdef _DEBUG
	m_pSphereCom->Render();
#endif // _DEBUG
	

	return S_OK;
}

HRESULT CSword::SetUp_Component()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Sphere */
	CCollider::COLLIDERDESC		ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPivot = _float3(0.f, 0.0f, 0.f);
	ColliderDesc.fRadius = 0.1f;
	ColliderDesc.eColType = CCollider::COL_TYPE_SPHERE;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider"), TEXT("Com_Sphere"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;
	

	return S_OK;
}

HRESULT CSword::SetUp_BoneMatrix()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CModel*		pPlayerModel = (CModel*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Model"));
	if (nullptr == pPlayerModel)
		return E_FAIL;

	ZeroMemory(&m_SwordDesc, sizeof(SWORDDESC));

	m_SwordDesc.pBoneMatrix = pPlayerModel->Get_CombinedMatrixPtr("Sword");
	m_SwordDesc.OffsetMatrix = pPlayerModel->Get_OffsetMatrix("Sword");
	m_SwordDesc.PivotMatrix = pPlayerModel->Get_PivotMatrix_Bones();

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_ComponentPtr(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return E_FAIL;

	m_SwordDesc.pTargetWorldMatrix = pPlayerTransform->Get_WorldFloat4x4Ptr();

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CSword::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);	

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


CSword * CSword::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSword*		pInstance = new CSword(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CGameObject * CSword::Clone(void* pArg)
{
	CSword*		pInstance = new CSword(*this);


	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CSword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSword::Free()
{

	__super::Free();

	Safe_Release(m_pSphereCom);
	Safe_Release(m_pTransformCom);	
	Safe_Release(m_pRendererCom);
}
