#include "..\Public\Collider.h"
#include "PipeLine.h"

CCollider::CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CCollider::CCollider(const CCollider& rhs)
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

	const void* pShaderByteCodes = nullptr;
	size_t	iShaderByteCodeLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iShaderByteCodeLength);

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pDeviceContext);

	m_eType = eType;

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;


	return S_OK;
}

HRESULT CCollider::NativeConstruct(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_ColliderDesc, pArg, sizeof(COLLIDERDESC));

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

	return S_OK;
}

HRESULT CCollider::Render()
{
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	m_pDeviceContext->IASetInputLayout(m_pInputLayout);

	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView((pPipeLine->Get_Transform(CPipeLine::TS_VIEW)));
	m_pEffect->SetProjection((pPipeLine->Get_Transform(CPipeLine::TS_PROJ)));

	m_pEffect->Apply(m_pDeviceContext);

	m_pBatch->Begin();

	_vector		vColor = m_isCollision == false ? DirectX::Colors::LightGreen : DirectX::Colors::LightPink;

	if (TYPE_AABB == m_eType)
		DX::Draw(m_pBatch, *m_pAABB, vColor);
	if (TYPE_OBB == m_eType)
		DX::Draw(m_pBatch, *m_pOBB, vColor);


	m_pBatch->End();


	RELEASE_INSTANCE(CPipeLine);

	return S_OK;
}

void CCollider::Update(_fmatrix TransformMatrix)
{
	if (TYPE_AABB == m_eType)
	{
		m_pAABB->Center = m_ColliderDesc.vPivot;
		m_pAABB->Extents = _float3(m_ColliderDesc.vSize.x * 0.5f, m_ColliderDesc.vSize.y * 0.5f, m_ColliderDesc.vSize.z * 0.5f);

		m_pAABB->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
	}

	if (TYPE_OBB == m_eType)
	{
		m_pOBB->Center = m_ColliderDesc.vPivot;
		m_pOBB->Extents = _float3(m_ColliderDesc.vSize.x * 0.5f, m_ColliderDesc.vSize.y * 0.5f, m_ColliderDesc.vSize.z * 0.5f);
		m_pOBB->Orientation = _float4(0.f, 0.f, 0.f, 1.f);

		m_pOBB->Transform(*m_pOBB, TransformMatrix);
	}
}

_bool CCollider::Collision_AABB(CCollider* pTargetCollider)
{
	_vector		vSourMin, vSourMax;
	_vector		vDestMin, vDestMax;

	vSourMin = XMVectorSet(m_pAABB->Center.x - m_pAABB->Extents.x, m_pAABB->Center.y - m_pAABB->Extents.y, m_pAABB->Center.z - m_pAABB->Extents.z, 1.f);
	vSourMax = XMVectorSet(m_pAABB->Center.x + m_pAABB->Extents.x, m_pAABB->Center.y + m_pAABB->Extents.y, m_pAABB->Center.z + m_pAABB->Extents.z, 1.f);

	BoundingBox* pTargetAABB = pTargetCollider->m_pAABB;
	vDestMin = XMVectorSet(pTargetAABB->Center.x - pTargetAABB->Extents.x, pTargetAABB->Center.y - pTargetAABB->Extents.y, pTargetAABB->Center.z - pTargetAABB->Extents.z, 1.f);
	vDestMax = XMVectorSet(pTargetAABB->Center.x + pTargetAABB->Extents.x, pTargetAABB->Center.y + pTargetAABB->Extents.y, pTargetAABB->Center.z + pTargetAABB->Extents.z, 1.f);

	m_isCollision = false;

	/* �ʺ�� */
	if (max(XMVectorGetX(vSourMin), XMVectorGetX(vDestMin)) > min(XMVectorGetX(vSourMax), XMVectorGetX(vDestMax)))
		return false;

	/* ���̺� */
	if (max(XMVectorGetY(vSourMin), XMVectorGetY(vDestMin)) > min(XMVectorGetY(vSourMax), XMVectorGetY(vDestMax)))
		return false;

	/* ���̺� */
	if (max(XMVectorGetZ(vSourMin), XMVectorGetZ(vDestMin)) > min(XMVectorGetZ(vSourMax), XMVectorGetZ(vDestMax)))
		return false;

	m_isCollision = true;

	return true;
}

_bool CCollider::Collision_OBB(CCollider* pTargetCollider)
{
	if (pTargetCollider == nullptr)
		return false;

	OBBDESC			OBBDesc[2];

	OBBDesc[0] = Compute_OBBDesc();
	OBBDesc[1] = pTargetCollider->Compute_OBBDesc();

	_float			fDistance[3];

	m_isCollision = false;

	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDirs[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDirs[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vExtentDirs[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDirs[0]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDirs[1]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vExtentDirs[2]), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter), XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			if (fDistance[0] + fDistance[1] < fDistance[2])
				return false;
		}
	}

	m_isCollision = true;
	return true;
}

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		ResultMatrix = XMMatrixIdentity();

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));
	ResultMatrix.r[3] = TransformMatrix.r[3];

	return ResultMatrix;
}

CCollider::OBBDESC CCollider::Compute_OBBDesc()
{
	//m_pOBB->GetCorners();


	_vector		vPoints[8] = {
		XMVectorSet(m_pOBB->Center.x - m_pOBB->Extents.x, m_pOBB->Center.y + m_pOBB->Extents.y, m_pOBB->Center.z - m_pOBB->Extents.z, 1.f),
		XMVectorSet(m_pOBB->Center.x + m_pOBB->Extents.x, m_pOBB->Center.y + m_pOBB->Extents.y, m_pOBB->Center.z - m_pOBB->Extents.z, 1.f),
		XMVectorSet(m_pOBB->Center.x + m_pOBB->Extents.x, m_pOBB->Center.y - m_pOBB->Extents.y, m_pOBB->Center.z - m_pOBB->Extents.z, 1.f),
		XMVectorSet(m_pOBB->Center.x - m_pOBB->Extents.x, m_pOBB->Center.y - m_pOBB->Extents.y, m_pOBB->Center.z - m_pOBB->Extents.z, 1.f),
		XMVectorSet(m_pOBB->Center.x - m_pOBB->Extents.x, m_pOBB->Center.y + m_pOBB->Extents.y, m_pOBB->Center.z + m_pOBB->Extents.z, 1.f),
		XMVectorSet(m_pOBB->Center.x + m_pOBB->Extents.x, m_pOBB->Center.y + m_pOBB->Extents.y, m_pOBB->Center.z + m_pOBB->Extents.z, 1.f),
		XMVectorSet(m_pOBB->Center.x + m_pOBB->Extents.x, m_pOBB->Center.y - m_pOBB->Extents.y, m_pOBB->Center.z + m_pOBB->Extents.z, 1.f),
		XMVectorSet(m_pOBB->Center.x - m_pOBB->Extents.x, m_pOBB->Center.y - m_pOBB->Extents.y, m_pOBB->Center.z + m_pOBB->Extents.z, 1.f)
	};

	OBBDESC		OBBDesc;
	ZeroMemory(&OBBDesc, sizeof(OBBDESC));

	(XMFLOAT3)OBBDesc.vCenter = m_pOBB->Center;

	XMStoreFloat3(&OBBDesc.vExtentDirs[0], (vPoints[5] + vPoints[2]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vExtentDirs[1], (vPoints[5] + vPoints[0]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));
	XMStoreFloat3(&OBBDesc.vExtentDirs[2], (vPoints[5] + vPoints[7]) * 0.5f - XMLoadFloat3(&OBBDesc.vCenter));

	XMStoreFloat3(&OBBDesc.vAlignAxis[0], XMVector3Normalize(vPoints[5] - vPoints[4]));
	XMStoreFloat3(&OBBDesc.vAlignAxis[1], XMVector3Normalize(vPoints[5] - vPoints[6]));
	XMStoreFloat3(&OBBDesc.vAlignAxis[2], XMVector3Normalize(vPoints[5] - vPoints[1]));

	return OBBDesc;
}


CCollider* CCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType)
{
	CCollider* pInstance = new CCollider(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType)))
	{
		MSG_BOX("Failed to Created CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

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

	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere);

	if (false == m_isCloned)
	{
		Safe_Delete(m_pEffect);
		Safe_Delete(m_pBatch);
	}
}
