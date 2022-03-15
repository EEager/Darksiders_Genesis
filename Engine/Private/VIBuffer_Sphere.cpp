#include "..\Public\VIBuffer_Sphere.h"

CVIBuffer_Sphere::CVIBuffer_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Sphere::CVIBuffer_Sphere(const CVIBuffer_Sphere & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Sphere::NativeConstruct_Prototype(const _tchar* pShaderFilePath, float radiust, _uint stackCount, _uint sliceCount)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	vector<_float3> vertices;
	m_radius = radiust;
	m_stackCount = stackCount;
	m_sliceCount = sliceCount;
	//m_pVertices = &vertices

	float phiStep = MathHelper::Pi / stackCount;
	float thetaStep = 2.0f * MathHelper::Pi / sliceCount;
	vertices.push_back(_float3(0.f, m_radius, 0.f)); // Top Vertex
	for (UINT i = 1; i <= stackCount - 1; i++)
	{
		float phi = i * phiStep;

		for (UINT j = 0; j <= sliceCount; j++)
		{
			float theta = j * thetaStep;

			_float3 p;
			p.x = m_radius * sinf(phi) * cosf(theta);
			p.y = m_radius * cosf(phi);
			p.z = m_radius * sinf(phi) * sinf(theta);

			vertices.push_back(p);
		}
	}
	vertices.push_back(_float3(0.f, -m_radius, 0.f)); // Bottom Vertex

	m_iStride = sizeof(_float3);
	m_iNumVertices = (_uint)vertices.size();
	m_iNumVertexBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = &vertices[0];
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	vector<UINT> indices;
	for (UINT i = 1; i <= sliceCount; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);	
	}

	UINT baseIndex = 1;
	UINT ringVertexCount = sliceCount + 1;
	for (UINT i = 0; i < stackCount - 2; i++)
	{
		for (UINT j = 0; j < sliceCount; j++)
		{
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	UINT southPoleIndex = (_uint)vertices.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (UINT i = 0; i < sliceCount; i++)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}


	m_iNumPrimitive = (_uint)indices.size();
	m_iIndicesSize = sizeof(UINT);
	m_iNumIndicesPerFigure = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	int indexCount = (_uint)indices.size();
	std::vector<_uint> indices32;
	indices32.assign(indices.begin(), indices.end());
	//m_pPrimitiveIndices = new UINT[indexCount];
	//copy(indices.begin(), indices.end(), stdext::checked_array_iterator<UINT*>((UINT*)m_pPrimitiveIndices, indexCount));
	//(UINT*)m_pPrimitiveIndices; // debug

	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	//m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;
	m_IBSubresourceData.pSysMem = &indices32[0];

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};	

	if (FAILED(Compile_Shader(ElementDesc, 1, pShaderFilePath)))
		return E_FAIL;


	return S_OK;
}

HRESULT CVIBuffer_Sphere::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Sphere * CVIBuffer_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, float radiust, _uint stackCount, _uint sliceCount)
{
	CVIBuffer_Sphere*	pInstance = new CVIBuffer_Sphere(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, radiust, stackCount, sliceCount)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Sphere::Clone(void * pArg)
{
	CVIBuffer_Sphere*	pInstance = new CVIBuffer_Sphere(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Sphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Sphere::Free()
{
	__super::Free();


}
