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

	/* ��ġ�� ���ÿ� �������� �������۸� ������ �� �ֱ� ����. */
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

	/* �������̴��� �ԷµǴ� ������ ��������. */
	m_pDeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout.Get());

	if (FAILED(m_PassesDesc[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	// �ν��Ͻ� Render�� �Ʒ��� ���� �Ѵ�.
	// PointEffect������ m_iIndexCountPerInstance=1�̴�. ������ �� �ν��Ͻ̿����� �޶��� ���� �ִ�.
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
