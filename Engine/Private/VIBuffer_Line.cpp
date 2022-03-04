#include "..\Public\VIBuffer_Line.h"

CVIBuffer_Line::CVIBuffer_Line(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Line::CVIBuffer_Line(const CVIBuffer_Line & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Line::NativeConstruct_Prototype(const _tchar* pShaderFilePath, _float3* pPoints, _uint iNumVertices)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	

	m_iStride = sizeof(VTXCOL);
	m_iNumVertices = iNumVertices;
	m_iNumVertexBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXCOL[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXCOL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		((VTXCOL*)m_pVertices)[i].vPosition = pPoints[i];
		((VTXCOL*)m_pVertices)[i].vColor = _float4(1.f, 1.f, 1.f, 1.f);
	}

	
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;	

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;


	//m_iNumPrimitive = m_iNumVertices - 1;
	//m_iIndicesSize = sizeof(FACEINDICES16);
	//m_iNumIndicesPerFigure = 2;
	//m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	//

	//ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	//m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	//m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	//m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//m_IBDesc.CPUAccessFlags = 0;
	//m_IBDesc.MiscFlags = 0;
	//m_IBDesc.StructureByteStride = 0;



	//ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	//m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;

	//if (FAILED(__super::Create_IndexBuffer()))
	//	return E_FAIL;


	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};	

	if (FAILED(Compile_Shader(ElementDesc, 2, pShaderFilePath)))
		return E_FAIL;	

	return S_OK;
}

HRESULT CVIBuffer_Line::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Line::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 장치에 동시에 여러개의 정점버퍼를 셋팅할 수 있기 때문. */
	ID3D11Buffer*		pBuffers[] = {
		m_pVB
	};

	_uint		iStrides[] = {
		m_iStride
	};

	_uint		iOffset[] = {
		0
	};

	m_pDeviceContext->IASetVertexBuffers(m_iNumVertexBuffers - 1, m_iNumVertexBuffers, pBuffers, iStrides, iOffset);	
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	/* 정점셰이더에 입력되는 정점의 구성정보. */
	m_pDeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout.Get());

	if (FAILED(m_PassesDesc[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_pDeviceContext->Draw(m_iNumVertices, 0);

	return S_OK;
}

CVIBuffer_Line * CVIBuffer_Line::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float3* pPoints, _uint iNumVertices, const _tchar* pShaderFilePath)
{
	CVIBuffer_Line*	pInstance = new CVIBuffer_Line(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pPoints, iNumVertices)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Line");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Line::Clone(void * pArg)
{
	CVIBuffer_Line*	pInstance = new CVIBuffer_Line(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Line");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Line::Free()
{
	__super::Free();


}
