#include "..\Public\Cell.h"
#include "VIBuffer_Line.h"
#include "PipeLine.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CCell::NativeConstruct(_float3* pPoints, _uint iIndex)
{
	m_iIndex = iIndex;

	ZeroMemory(m_Neighbors, sizeof(CCell*) * LINE_END);

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	XMStoreFloat3(&m_vLine[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	XMStoreFloat3(&m_vLine[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	XMStoreFloat3(&m_vLine[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));


#ifdef _DEBUG
	if (FAILED(Ready_DebugBuffer()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vDestPoint1, _fvector vDestPoint2)
{
	
	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint1))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint2))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint2))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint1))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint2))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint2))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint1))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint2))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint2))
			return true;
	}

	return false;
}

_bool CCell::isIn(_fvector vPoint, _float4x4* pWorldMatrix, CCell** ppNeighbor)
{
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vDirW = vPoint - XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), XMLoadFloat4x4(pWorldMatrix));
		_vector		vNormalW = XMVectorSet(m_vLine[i].z * -1, 0.f, m_vLine[i].x, 0.f);

		vNormalW = XMVector3TransformNormal(vNormalW, XMLoadFloat4x4(pWorldMatrix));

		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirW), XMVector3Normalize(vNormalW))))
		{
			*ppNeighbor = m_Neighbors[i];
			return false;
		}
	}

	*ppNeighbor = this;
	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render(_float4x4* pWorldMatrix, _uint iCurrentIndex)
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	_float4		vColor = _float4(1.f, 1.f, 1.f, 1.f);

	if (m_iIndex == iCurrentIndex)
		vColor = _float4(1.f, 0.f, 0.f, 1.f);

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	m_pVIBuffer->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(pWorldMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(pPipeLine->Get_Transform(CPipeLine::TS_VIEW)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(pPipeLine->Get_Transform(CPipeLine::TS_PROJ)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_vColor", &vColor, sizeof(_float4));

	m_pVIBuffer->Render(0);

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;
}
#endif // _DEBUG

#ifdef _DEBUG
HRESULT CCell::Ready_DebugBuffer()
{
	_float3		vPoints[] = {
		m_vPoints[POINT_A],
		m_vPoints[POINT_B],
		m_vPoints[POINT_C],
		m_vPoints[POINT_A]
	};

	m_pVIBuffer = CVIBuffer_Line::Create(m_pDevice, m_pDeviceContext, vPoints, 4);

	if (nullptr == m_pVIBuffer)
		return E_FAIL;


	return S_OK;
}
#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float3* pPoints, _uint iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pPoints, iIndex)))
	{
		MSG_BOX("Failed To Creating CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
#endif // _DEBUG

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
