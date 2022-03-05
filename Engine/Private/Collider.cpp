#include "..\Public\Collider.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_pInputLayout(rhs.m_pInputLayout)
	, m_pAABB(rhs.m_pAABB)
	, m_pOBB(rhs.m_pOBB)
	, m_pSphere(rhs.m_pSphere)
	, m_eType(rhs.m_eType)
	, m_pEffect(rhs.m_pEffect)
	, m_pBatch(rhs.m_pBatch)
{
	Safe_AddRef(m_pInputLayout);
}

HRESULT CCollider::NativeConstruct_Prototype(TYPE eType)
{
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*	pShaderByteCodes = nullptr;
	size_t	iShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iShaderByteCodeLength);

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);

	m_eType = eType;

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f));
		break;
	case CCollider::TYPE_OBB:
		m_pOBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(0.5f, 0.5f, 0.5f), _float4(0.f, 0.f, 0.f, 0.f));
		break;
	case CCollider::TYPE_SPHERE:
		m_pSphere = new BoundingSphere(_float3(0.f, 0.f, 0.f), 0.5f);
		break;	
	}

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCollider::NativeConstruct(void * pArg)
{	
	if (nullptr != pArg)
		memcpy(&m_ColliderDesc, pArg, sizeof(COLLIDERDESC));	

	return S_OK;
}

HRESULT CCollider::Render(_fmatrix TransformMatrix)
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);	
	
	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView((pPipeLine->Get_Transform(CPipeLine::TS_VIEW)));
	m_pEffect->SetProjection((pPipeLine->Get_Transform(CPipeLine::TS_PROJ)));

	m_pEffect->Apply(m_pDeviceContext);

	m_pBatch->Begin();

	if (TYPE_AABB == m_eType)
	{
		m_pAABB->Center = m_ColliderDesc.vPivot;
		m_pAABB->Extents = _float3(m_ColliderDesc.vSize.x * 0.5f, m_ColliderDesc.vSize.y * 0.5f, m_ColliderDesc.vSize.z * 0.5f);

		m_pAABB->Transform(*m_pAABB, TransformMatrix);



		DX::Draw(m_pBatch, *m_pAABB);
	}


	m_pBatch->End();


	RELEASE_INSTANCE(CPipeLine);
	
	return S_OK;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType)))
	{
		MSG_BOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Release(m_pInputLayout);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);

		Safe_Delete(m_pAABB);
		Safe_Delete(m_pOBB);
		Safe_Delete(m_pSphere);
	}	
}
