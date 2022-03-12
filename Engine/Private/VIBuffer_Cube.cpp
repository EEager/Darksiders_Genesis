#include "..\Public\VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Cube::NativeConstruct_Prototype(const _tchar* pShaderFilePath)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXCUBETEX);
	m_iNumVertices = 8;
	m_iNumVertexBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE; 

	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXCUBETEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXCUBETEX) * m_iNumVertices);

	((VTXCUBETEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[0].vTexUV = ((VTXCUBETEX*)m_pVertices)[0].vPosition;

	((VTXCUBETEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[1].vTexUV = ((VTXCUBETEX*)m_pVertices)[1].vPosition;

	((VTXCUBETEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[2].vTexUV = ((VTXCUBETEX*)m_pVertices)[2].vPosition;

	((VTXCUBETEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	((VTXCUBETEX*)m_pVertices)[3].vTexUV = ((VTXCUBETEX*)m_pVertices)[3].vPosition;


	((VTXCUBETEX*)m_pVertices)[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[4].vTexUV = ((VTXCUBETEX*)m_pVertices)[4].vPosition;

	((VTXCUBETEX*)m_pVertices)[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[5].vTexUV = ((VTXCUBETEX*)m_pVertices)[5].vPosition;

	((VTXCUBETEX*)m_pVertices)[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[6].vTexUV = ((VTXCUBETEX*)m_pVertices)[6].vPosition;

	((VTXCUBETEX*)m_pVertices)[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	((VTXCUBETEX*)m_pVertices)[7].vTexUV = ((VTXCUBETEX*)m_pVertices)[7].vPosition;

	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;	

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = 12;
	m_iIndicesSize = sizeof(FACEINDICES16);
	m_iNumIndicesPerFigure = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pPrimitiveIndices = new FACEINDICES16[m_iNumPrimitive];

	/* +X */
	((FACEINDICES16*)m_pPrimitiveIndices)[0]._0 = 1;
	((FACEINDICES16*)m_pPrimitiveIndices)[0]._1 = 5;
	((FACEINDICES16*)m_pPrimitiveIndices)[0]._2 = 6;
	((FACEINDICES16*)m_pPrimitiveIndices)[1]._0 = 1;
	((FACEINDICES16*)m_pPrimitiveIndices)[1]._1 = 6;
	((FACEINDICES16*)m_pPrimitiveIndices)[1]._2 = 2;

	/* -X */
	((FACEINDICES16*)m_pPrimitiveIndices)[2]._0 = 4;
	((FACEINDICES16*)m_pPrimitiveIndices)[2]._1 = 0;
	((FACEINDICES16*)m_pPrimitiveIndices)[2]._2 = 3;
	((FACEINDICES16*)m_pPrimitiveIndices)[3]._0 = 4;
	((FACEINDICES16*)m_pPrimitiveIndices)[3]._1 = 3;
	((FACEINDICES16*)m_pPrimitiveIndices)[3]._2 = 7;

	/* +Y */
	((FACEINDICES16*)m_pPrimitiveIndices)[4]._0 = 4;
	((FACEINDICES16*)m_pPrimitiveIndices)[4]._1 = 5;
	((FACEINDICES16*)m_pPrimitiveIndices)[4]._2 = 1;
	((FACEINDICES16*)m_pPrimitiveIndices)[5]._0 = 4;
	((FACEINDICES16*)m_pPrimitiveIndices)[5]._1 = 1;
	((FACEINDICES16*)m_pPrimitiveIndices)[5]._2 = 0;

	/* -Y */
	((FACEINDICES16*)m_pPrimitiveIndices)[6]._0 = 3;
	((FACEINDICES16*)m_pPrimitiveIndices)[6]._1 = 2;
	((FACEINDICES16*)m_pPrimitiveIndices)[6]._2 = 6;
	((FACEINDICES16*)m_pPrimitiveIndices)[7]._0 = 3;
	((FACEINDICES16*)m_pPrimitiveIndices)[7]._1 = 6;
	((FACEINDICES16*)m_pPrimitiveIndices)[7]._2 = 7;

	/* +Z */
	((FACEINDICES16*)m_pPrimitiveIndices)[8]._0 = 5;
	((FACEINDICES16*)m_pPrimitiveIndices)[8]._1 = 4;
	((FACEINDICES16*)m_pPrimitiveIndices)[8]._2 = 7;
	((FACEINDICES16*)m_pPrimitiveIndices)[9]._0 = 5;
	((FACEINDICES16*)m_pPrimitiveIndices)[9]._1 = 7;
	((FACEINDICES16*)m_pPrimitiveIndices)[9]._2 = 6;

	/* -Z */
	((FACEINDICES16*)m_pPrimitiveIndices)[10]._0 = 0;
	((FACEINDICES16*)m_pPrimitiveIndices)[10]._1 = 1;
	((FACEINDICES16*)m_pPrimitiveIndices)[10]._2 = 2;
	((FACEINDICES16*)m_pPrimitiveIndices)[11]._0 = 0;
	((FACEINDICES16*)m_pPrimitiveIndices)[11]._1 = 2;
	((FACEINDICES16*)m_pPrimitiveIndices)[11]._2 = 3;


	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};	

	if (FAILED(Compile_Shader(ElementDesc, 2, pShaderFilePath)))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CVIBuffer_Cube::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Cube * CVIBuffer_Cube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath)
{
	CVIBuffer_Cube*	pInstance = new CVIBuffer_Cube(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Cube::Clone(void * pArg)
{
	CVIBuffer_Cube*	pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Cube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();


}
