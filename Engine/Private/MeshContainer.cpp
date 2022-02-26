#include "..\Public\MeshContainer.h"
#include "Model.h"
#include "HierarchyNode.h"

CMeshContainer::CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CMeshContainer::CMeshContainer(const CMeshContainer& rhs)
	: CVIBuffer(rhs)
	, m_pAIMesh(rhs.m_pAIMesh)
	, m_iNumBones(rhs.m_iNumBones)
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
{

}

HRESULT CMeshContainer::NativeConstruct_Prototype(CModel* pModel, _bool isAnimMesh, aiMesh* pMesh, _fmatrix PivotMatrix)
{
	m_pAIMesh = pMesh;

	if (FAILED(SetUp_VerticesDesc(pModel, pMesh, isAnimMesh, PivotMatrix)))
		return E_FAIL;

	if (FAILED(SetUp_IndicesDesc(pMesh)))
		return E_FAIL;

	m_iMaterialIndex = pMesh->mMaterialIndex;

	return S_OK;
}

HRESULT CMeshContainer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CMeshContainer::Render()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	_uint		iOffset = 0;

	m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &m_iStride, &iOffset);
	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pDeviceContext->DrawIndexed(m_iNumPrimitive * m_iNumIndicesPerFigure, 0, 0);

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

HRESULT CMeshContainer::SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix)
{
	_uint			iBoneIndex = 0;

	if (0 == m_iNumBones) // ForkLift
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());

		return S_OK;
	}

	// 현재 정점에 영향을 주고 있는 뼈들을 순회를 도는데...이거보니 사실상 134개 모두 다 도네 ㅎㅎ 
	for (auto& pHierarchyNode : m_Bones)
	{
		_matrix		OffsetMatrix = pHierarchyNode->Get_OffsetMatix();
		_matrix		CombinedTransformationMatrix = pHierarchyNode->Get_CombinedMatix();

		XMStoreFloat4x4(&pBoneMatrices[iBoneIndex++], XMMatrixTranspose(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix));
	}

	return S_OK;
}

HRESULT CMeshContainer::SetUp_VerticesDesc(CModel* pModel, aiMesh* pMesh, _bool isAnim, _fmatrix PivotMatrix)
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


	/* 노말 texUV Tangent */
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		VTXMESH* pVertices = (VTXMESH*)((_byte*)m_pVertices + (i * m_iStride));

		memcpy(&pVertices->vPosition, &pMesh->mVertices[i], sizeof(_float3));

		XMStoreFloat3(&pVertices->vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices->vPosition), PivotMatrix));

		memcpy(&pVertices->vNormal, &pMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices->vTexUV, &pMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices->vTangent, &pMesh->mTangents[i], sizeof(_float3));
	}


	/* 블렌드인덱스와 웨이트 */
	if (true == isAnim)
		SetUp_SkinnedDesc(pModel, pMesh);

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

HRESULT CMeshContainer::Add_Bones(CModel* pModel)
{
	if (0 == m_iNumBones) // Lift와 Sword를 구분하고 싶다면? 
	{
		CHierarchyNode* pHierarchyNode = pModel->Find_HierarchyNode(m_pAIMesh->mName.data);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		// ForkLift의 경우 SetUp_BoneMatrices에서 하이라키 Combined가 쓰레기값이 나온다. 
		// ForkLift의 경우 m_iNumBones를 0으로 만들어, SetUp_BoneMatrices에서 항등행렬을 주도록하자.
		if (!strcmp(pHierarchyNode->Get_Name(), "ForkLift"))
			return S_OK;

		pHierarchyNode->Set_OffsetMatrix(XMMatrixIdentity());
		m_Bones.push_back(pHierarchyNode);

		m_iNumBones = 1;
		return S_OK;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* 현재 이 뼈가 어떤 정점에게 영햐을 미치고 있는지?! */
		/* 이 뼈ㅑ가 얼마나 영향을 주는지?! */
		aiBone* pBone = m_pAIMesh->mBones[i];

		CHierarchyNode* pHierarchyNode = pModel->Find_HierarchyNode(pBone->mName.data);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		_matrix		OffsetMatrix;
		memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_matrix));

		// aiBone* pBone이 가지고 있는 오프셋행렬을 CHierarchyNode에 넣고 내 뼈 목록에 추가
		pHierarchyNode->Set_OffsetMatrix(XMMatrixTranspose(OffsetMatrix));

		m_Bones.push_back(pHierarchyNode);
		Safe_AddRef(pHierarchyNode);
	}

	return S_OK;
}

HRESULT CMeshContainer::SetUp_SkinnedDesc(CModel* pModel, aiMesh* pMesh)
{
	m_iNumBones = pMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* 현재 이 뼈가 어떤 정점에게 영햐을 미치고 있는지?! */
		/* 이 뼈ㅑ가 얼마나 영향을 주는지?! */
		aiBone* pBone = pMesh->mBones[i];

		/* 현재 이 뼈가 몇개의 정점에 영향르 주는지. */
		for (_uint j = 0; j < pBone->mNumWeights; ++j)
		{
			VTXMESH_ANIM* pVertices = (VTXMESH_ANIM*)m_pVertices;

			if (pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.x == 0.0f)
			{
				pVertices[pBone->mWeights[j].mVertexId].vBlendIndex.x = i;
				pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.x = pBone->mWeights[j].mWeight;
			}

			else if (pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.y == 0.0f)
			{
				pVertices[pBone->mWeights[j].mVertexId].vBlendIndex.y = i;
				pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.y = pBone->mWeights[j].mWeight;
			}

			else if (pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.z == 0.0f)
			{
				pVertices[pBone->mWeights[j].mVertexId].vBlendIndex.z = i;
				pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.z = pBone->mWeights[j].mWeight;
			}

			else
			{
				pVertices[pBone->mWeights[j].mVertexId].vBlendIndex.w = i;
				pVertices[pBone->mWeights[j].mVertexId].vBlendWeight.w = pBone->mWeights[j].mWeight;
			}
		}
	}

	return S_OK;
}

CMeshContainer* CMeshContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CModel* pModel, _bool isAnimMesh, aiMesh* pMesh, _fmatrix PivotMatrix)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pModel, isAnimMesh, pMesh, PivotMatrix)))
	{
		MSG_BOX("Failed To Creating CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMeshContainer::Clone(void* pArg)
{
	CMeshContainer* pInstance = new CMeshContainer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CMeshContainer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshContainer::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_Bones)
		Safe_Release(pHierarchyNode);

	m_Bones.clear();

}
