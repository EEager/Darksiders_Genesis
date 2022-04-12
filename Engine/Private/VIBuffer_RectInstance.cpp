#include "..\Public\VIBuffer_RectInstance.h"

#include "GameInstance.h"


CVIBuffer_RectInstance::CVIBuffer_RectInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer_Instance(pDevice, pDeviceContext)
{

}

CVIBuffer_RectInstance::CVIBuffer_RectInstance(const CVIBuffer_RectInstance& rhs)
	: CVIBuffer_Instance(rhs)

{

}

HRESULT CVIBuffer_RectInstance::NativeConstruct_Prototype(const _tchar* pShaderFilePath, _uint iNumInstance)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 4;
	m_iNumVertexBuffers = 2;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXTEX) * m_iNumVertices);

	((VTXTEX*)m_pVertices)[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	((VTXTEX*)m_pVertices)[0].vTexUV = _float2(0.0f, 0.f);

	((VTXTEX*)m_pVertices)[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	((VTXTEX*)m_pVertices)[1].vTexUV = _float2(1.0f, 0.f);

	((VTXTEX*)m_pVertices)[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	((VTXTEX*)m_pVertices)[2].vTexUV = _float2(1.0f, 1.f);

	((VTXTEX*)m_pVertices)[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	((VTXTEX*)m_pVertices)[3].vTexUV = _float2(0.0f, 1.f);

	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;	

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	/* For.Instance Vertex Buffer */
	m_iNumInstance = iNumInstance;

	ZeroMemory(&m_VBInstDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstDesc.ByteWidth = sizeof(VTXMATRIX) * m_iNumInstance;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstDesc.MiscFlags = 0;
	m_VBInstDesc.StructureByteStride = sizeof(VTXMATRIX);	

	VTXMATRIX*		pInstanceVtx = new VTXMATRIX[m_iNumInstance];
	ZeroMemory(pInstanceVtx, sizeof(VTXMATRIX) * m_iNumInstance);

	m_pInstanceSpeed = new _float[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{	
		m_pInstanceSpeed[i] = rand() % 10 + 5;
		pInstanceVtx[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVtx[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVtx[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVtx[i].vPosition = _float4(rand() % 10, 15.f, rand() % 10, 1.f);
	}



	ZeroMemory(&m_VBInstSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBInstSubresourceData.pSysMem = pInstanceVtx;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstDesc, &m_VBInstSubresourceData, &m_pVBInst)))
		return E_FAIL;

	Safe_Delete_Array(pInstanceVtx);

	m_iNumPrimitive = 2 * m_iNumInstance;
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

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((FACEINDICES16*)m_pPrimitiveIndices)[0]._0 = 0;
		((FACEINDICES16*)m_pPrimitiveIndices)[0]._1 = 1;
		((FACEINDICES16*)m_pPrimitiveIndices)[0]._2 = 2;

		((FACEINDICES16*)m_pPrimitiveIndices)[1]._0 = 0;
		((FACEINDICES16*)m_pPrimitiveIndices)[1]._1 = 2;
		((FACEINDICES16*)m_pPrimitiveIndices)[1]._2 = 3;
	}

	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	m_iIndexCountPerInstance = 6;

	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 

		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 4, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};	

	if (FAILED(Compile_Shader(ElementDesc, 6, pShaderFilePath)))  
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_RectInstance::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_RectInstance::Update(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResourceData;

	m_pDeviceContext->Map(m_pVBInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		// Speed는 서서히 내려가게하자.
		((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y -= m_pInstanceSpeed[i] * fTimeDelta;

		// 다 내려왔다면 다시 위로 올려주자.
		if (((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y <= 0.f)
			((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y = 15.f;
	}
	m_pDeviceContext->Unmap(m_pVBInst, 0);
}

CVIBuffer_RectInstance * CVIBuffer_RectInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, _uint iNumInstance)
{
	CVIBuffer_RectInstance*	pInstance = new CVIBuffer_RectInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, iNumInstance)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_RectInstance::Clone(void * pArg)
{
	CVIBuffer_RectInstance*	pInstance = new CVIBuffer_RectInstance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_RectInstance::Free()
{
	__super::Free();

}
