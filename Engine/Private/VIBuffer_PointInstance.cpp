#include "..\Public\VIBuffer_PointInstance.h"

CVIBuffer_PointInstance::CVIBuffer_PointInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer_Instance(pDevice, pDeviceContext)
{

}

CVIBuffer_PointInstance::CVIBuffer_PointInstance(const CVIBuffer_PointInstance & rhs)
	: CVIBuffer_Instance(rhs)

{

}

HRESULT CVIBuffer_PointInstance::NativeConstruct_Prototype(const _tchar* pShaderFilePath, _uint iNumInstance)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXPOINT);
	m_iNumVertices = 1;
	m_iNumVertexBuffers = 2;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	((VTXPOINT*)m_pVertices)[0].vPosition = _float3(0.f, 0.f, 0.f);
	((VTXPOINT*)m_pVertices)[0].fPSize = 1.0f;

	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;	

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	/* For.Instance Vertex Buffer */
	m_iNumInstance = iNumInstance;

	ZeroMemory(&m_VBInstDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBInstDesc.ByteWidth = sizeof(VTXMATRIX) * m_iNumInstance;
	m_VBInstDesc.Usage = D3D11_USAGE_DYNAMIC; // 맵 언맵을 위한 DYNAMIC 플래그
	m_VBInstDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 맵 언맵을 위한 WRITE 플래그
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

	/* For. Index Buffer */
	m_iNumPrimitive = m_iNumInstance;
	m_iIndicesSize = sizeof(_ushort);
	m_iNumIndicesPerFigure = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pPrimitiveIndices = new _ushort[m_iNumPrimitive];	

	for (_uint i = 0; i < m_iNumInstance; ++i) // 버텍스를 점으로 만들었으니 인덱스는 인스턴스 개수만큼 할당해주자
	{
		((_ushort*)m_pPrimitiveIndices)[0] = 0;		
	}

	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;


	// 인스턴스당 인덱스 개수. 나중에 render할때 사용된다
	m_iIndexCountPerInstance = 1;

	// Shader_PointInstance.hlsl 시멘틱 
	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 

		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "TEXCOORD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};	

	if (FAILED(Compile_Shader(ElementDesc, 6, pShaderFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_PointInstance::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_PointInstance::Update(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResourceData;

	m_pDeviceContext->Map(m_pVBInst, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceData);

	// 인스턴스 개수만큼. 정점 버퍼가 가지고 있는 vPosition를 변경시켜주자.
	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y -= m_pInstanceSpeed[i] * fTimeDelta;

		if (((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y <= 0.f)
			((VTXMATRIX*)SubResourceData.pData)[i].vPosition.y = 15.f;


	}
	m_pDeviceContext->Unmap(m_pVBInst, 0);
}

CVIBuffer_PointInstance * CVIBuffer_PointInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, _uint iNumInstance)
{
	CVIBuffer_PointInstance*	pInstance = new CVIBuffer_PointInstance(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, iNumInstance)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_PointInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_PointInstance::Clone(void * pArg)
{
	CVIBuffer_PointInstance*	pInstance = new CVIBuffer_PointInstance(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_PointInstance");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_PointInstance::Free()
{
	__super::Free();




}
