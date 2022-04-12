#include "..\Public\VIBuffer_Instance.h"


CVIBuffer_Instance::CVIBuffer_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Instance::CVIBuffer_Instance(const CVIBuffer_Instance & rhs)
	: CVIBuffer(rhs)
	, m_pVBInst(rhs.m_pVBInst)
	, m_VBInstDesc(rhs.m_VBInstDesc)
	, m_VBInstSubresourceData(rhs.m_VBInstSubresourceData)
	, m_iNumInstance(rhs.m_iNumInstance)
	, m_pInstanceSpeed(rhs.m_pInstanceSpeed)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
{
	Safe_AddRef(m_pVBInst);
}

HRESULT CVIBuffer_Instance::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 장치에 동시에 여러개의 정점버퍼를 셋팅할 수 있기 때문. */
	ID3D11Buffer*		pBuffers[] = {
		m_pVB,
		m_pVBInst
	};

	_uint		iStrides[] = {
		m_iStride,
		sizeof(VTXMATRIX)
	};

	_uint		iOffset[] = {
		0,
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffers, iStrides, iOffset);

	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	/* 정점셰이더에 입력되는 정점의 구성정보. */
	m_pDeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout.Get());

	if (FAILED(m_PassesDesc[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	// 인스턴스 Render는 아래와 같이 한다.
	// PointEffect에서는 m_iIndexCountPerInstance=1이다. 하지만 모델 인스턴싱에서는 달라질 수도 있다.
	m_pDeviceContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstance, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instance::Update(_float fTimeDelta)
{
}


void CVIBuffer_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pInstanceSpeed);

	Safe_Release(m_pVBInst);
}
