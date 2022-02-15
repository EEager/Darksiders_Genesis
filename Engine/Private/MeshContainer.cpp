#include "..\Public\MeshContainer.h"

CMeshContainer::CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

HRESULT CMeshContainer::NativeConstruct_Prototype(_bool isAnimMesh, aiMesh* pMesh)
{	
	if (FAILED(SetUp_VerticesDesc(pMesh, isAnimMesh)))
		return E_FAIL;	

	if (FAILED(SetUp_IndicesDesc(pMesh)))
		return E_FAIL;


	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Render(_uint iPassIndex)
{
	return S_OK;
}

HRESULT CMeshContainer::Create_VertexIndexBuffer()
{
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeshContainer::SetUp_VerticesDesc(aiMesh* pMesh, _bool isAnim)
{
	m_iNumVertices = pMesh->mNumVertices;	
	m_iNumVertexBuffers = 1;

	if (false == isAnim)
	{
		m_pVertices = new VTXMESH[m_iNumVertices];
		ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);
		m_iStride = sizeof(VTXMESH);
	}

	else
	{
		m_pVertices = new VTXMESH_ANIM[m_iNumVertices];
		ZeroMemory(m_pVertices, sizeof(VTXMESH_ANIM) * m_iNumVertices);
		m_iStride = sizeof(VTXMESH_ANIM);		
	}

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;


	/* 블렌드인덱스와 웨이트는 나중에 채울께.(애니메이션을 위한 데이터) */
	VTXMESH*		pVertices = (VTXMESH*)m_pVertices;

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexUV, &pMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pMesh->mTangents[i], sizeof(_float3));
	}

	m_VBSubresourceData.pSysMem = m_pVertices;

	return S_OK;
}

HRESULT CMeshContainer::SetUp_IndicesDesc(aiMesh * pMesh)
{
	m_iNumPrimitive = pMesh->mNumFaces;	
	m_iIndicesSize = sizeof(FACEINDICES32);
	m_iNumIndicesPerFigure = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pPrimitiveIndices = new FACEINDICES32[m_iNumPrimitive];
	ZeroMemory(m_pPrimitiveIndices, sizeof(FACEINDICES32) * m_iNumPrimitive);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		((FACEINDICES32*)m_pPrimitiveIndices)[i]._0 = pMesh->mFaces[i].mIndices[0];
		((FACEINDICES32*)m_pPrimitiveIndices)[i]._1 = pMesh->mFaces[i].mIndices[1];
		((FACEINDICES32*)m_pPrimitiveIndices)[i]._2 = pMesh->mFaces[i].mIndices[2];
	}

	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;

	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _bool isAnimMesh, aiMesh* pMesh)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(isAnimMesh, pMesh)))
	{
		MSG_BOX("Failed To Creating CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	return nullptr;
}

void CMeshContainer::Free()
{
	__super::Free();


}
