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

HRESULT CMeshContainer::NativeConstruct_Prototype(CModel* pModel, aiMesh* pMesh, _fmatrix PivotMatrix, OUT XMVECTOR* pMin, OUT XMVECTOR* pMax)
{
	m_pAIMesh = pMesh;

	if (FAILED(SetUp_VerticesDesc(pModel, pMesh, PivotMatrix, pMin, pMax)))
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

	if (0 == m_iNumBones) 
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());

		return S_OK;
	}

	// ���� ������ ������ �ְ� �ִ� ������ ��ȸ�� ���µ�...�̰ź��� ��ǻ� 134�� ��� �� ���� ���� 
	for (auto& pHierarchyNode : m_Bones)
	{
		_matrix		OffsetMatrix = pHierarchyNode->Get_OffsetMatix(); // ��->����
		_matrix		CombinedTransformationMatrix = pHierarchyNode->Get_CombinedMatix(); // ��Ʈ->��


		// ���������� ���̴��� ������ �� = ������ * �Ĺ��ε� * �Ǻ� 
		XMStoreFloat4x4(&pBoneMatrices[iBoneIndex++], 
			XMMatrixTranspose(OffsetMatrix * CombinedTransformationMatrix * PivotMatrix));
	}

	return S_OK;
}

HRESULT CMeshContainer::SetUp_VerticesDesc(CModel* pModel, aiMesh* pMesh, _fmatrix PivotMatrix, OUT XMVECTOR* pMin, OUT XMVECTOR* pMax)
{
	m_iNumVertices = pMesh->mNumVertices;	
	m_iNumVertexBuffers = 1;

	// ----------
	// Enviroment�ΰ��. m_iNumVertices == 6¥���� Always Enviroment�� �������� �ȴ�. 
	// pModel ���� Set������.
	bool isEnviromentBase = false;
	if (!strncmp(pMesh->mName.data, "Enviroment", strlen("Enviroment")) && m_iNumVertices == 6)
	{
		XMStoreFloat3(&pModel->m_vEnviromentBase, XMVector3TransformCoord(XMLoadFloat3(&_float3(pMesh->mVertices[0].x, pMesh->mVertices[0].y, pMesh->mVertices[0].z)), PivotMatrix));
		isEnviromentBase = true;
	}

	// ----------

	CModel::TYPE		eMeshType = pModel->Get_MeshType();

	if (CModel::TYPE_NONANIM == eMeshType) // �ִϸ��̼� ���� ���.
	{
		m_pVertices = new VTXMESH[m_iNumVertices];
		ZeroMemory(m_pVertices, sizeof(VTXMESH) * m_iNumVertices);
		m_iStride = sizeof(VTXMESH);
	}

	else // �ִϸ��̼� �ִ� ���.
	{
		m_pVertices = new VTXMESH_ANIM[m_iNumVertices];
		ZeroMemory(m_pVertices, sizeof(VTXMESH_ANIM) * m_iNumVertices);
		m_iStride = sizeof(VTXMESH_ANIM);		

		/* �����ε����� ����Ʈ */
		SetUp_SkinnedDesc(pModel, pMesh);
	}

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	/* �븻 texUV Tangent */
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		VTXMESH* pVertices = (VTXMESH*)((_byte*)m_pVertices + (i * m_iStride));

		memcpy(&pVertices->vPosition, &pMesh->mVertices[i], sizeof(_float3));

		// For.Collider
		XMVECTOR P;
		if (CModel::TYPE_NONANIM == eMeshType) // �ִϸ��̼Ǿ��°�� �Ǻ��� ��������.
		{
			XMStoreFloat3(&pVertices->vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices->vPosition), PivotMatrix));
			P = XMLoadFloat3(&pVertices->vPosition);
		}
		else // �ִϸ��̼��� ������ �Ǻ��� ���߿� Render�ϱ� ���� ���ϴ���
		{
			XMStoreFloat3(&pVertices->vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices->vPosition), XMMatrixIdentity()));
			P = XMVector3TransformCoord(XMLoadFloat3(&pVertices->vPosition), PivotMatrix);
		}

		if (isEnviromentBase == false) // ȯ�� �������� ��� �ݶ��̴� ������ ���Խ�Ű������
		{
			*pMin = XMVectorMin(*pMin, P);
			*pMax = XMVectorMax(*pMax, P);
		}


		memcpy(&pVertices->vNormal, &pMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices->vTexUV, &pMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices->vTangent, &pMesh->mTangents[i], sizeof(_float3));
	}

	_float4 tmpMin, tmpMax;
	XMStoreFloat4(&tmpMin, *pMin);
	XMStoreFloat4(&tmpMax, *pMax);

	//printf("tmpMin : %lf, %lf, %lf\n", tmpMin.x, tmpMin.y, tmpMin.z);
	//printf("tmpMax : %lf, %lf, %lf\n", tmpMax.x, tmpMax.y, tmpMax.z);

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

HRESULT CMeshContainer::Add_Bones(CModel* pModel, CHierarchyNode* parentHierarchyNode)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* ���� �� ���� � �������� ������ ��ġ�� �ִ���?! */
		/* �� ������ �󸶳� ������ �ִ���?! */
		aiBone* pBone = m_pAIMesh->mBones[i];

		CHierarchyNode* pHierarchyNode = pModel->Find_HierarchyNode(pBone->mName.data);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		_matrix		OffsetMatrix;
		memcpy(&OffsetMatrix, &pBone->mOffsetMatrix, sizeof(_matrix));

		// aiBone* pBone�� ������ �ִ� ����������� CHierarchyNode�� �ְ� �� �� ��Ͽ� �߰�
		pHierarchyNode->Set_OffsetMatrix(XMMatrixTranspose(OffsetMatrix));

		m_Bones.push_back(pHierarchyNode);
		Safe_AddRef(pHierarchyNode);
	}

	// �ٸ� ���� Ư�� CHierarchyNode�� m_Bones�� �ְ� �ʹ�. (Į.fbx�� War���� ������ �ִ� ������ġ�� ���󰡰� �ʹ�) 
	if (parentHierarchyNode)
	{
		// parentHierarchyNode->Set_OffsetMatrix(XMMatrixIdentity()); �θ� ���� �������
		Safe_AddRef(parentHierarchyNode);
		m_Bones.push_back(parentHierarchyNode);
		m_iNumBones++;
		return S_OK; // ���⼭ �ٷ� �������ð��, ���� Į �Ž��� ���� ���� ��� ������ �Ǳ��ϴµ� �ϴ� �ѱ���
	}

	if (0 == m_iNumBones) // �� �Ž���, ���� ���� �Ž���(m_iNumBones == pMesh->mNumBones == 0), ������� �־�����.
					  // ex) �ǿ��� Į�� ���ٴڿ� ���������. 
	{
		CHierarchyNode* pHierarchyNode = pModel->Find_HierarchyNode(m_pAIMesh->mName.data); // m_pAIMesh(Į�޽�...) : m_pScene->mMeshes[i]
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		pHierarchyNode->Set_OffsetMatrix(XMMatrixIdentity());
		m_Bones.push_back(pHierarchyNode);

		m_iNumBones = 1;
		return S_OK;
	}

	return S_OK;
}

HRESULT CMeshContainer::SetUp_SkinnedDesc(CModel* pModel, aiMesh* pMesh)
{
	m_iNumBones = pMesh->mNumBones;

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* ���� �� ���� � �������� ������ ��ġ�� �ִ���?! */
		/* �� ������ �󸶳� ������ �ִ���?! */
		aiBone* pBone = pMesh->mBones[i];

		/* ���� �� ���� ��� ������ ���⸣ �ִ���. */
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

CMeshContainer* CMeshContainer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CModel* pModel, aiMesh* pMesh, _fmatrix PivotMatrix, OUT XMVECTOR* pMin, OUT XMVECTOR* pMax)
{
	CMeshContainer*	pInstance = new CMeshContainer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pModel, pMesh, PivotMatrix, pMin, pMax)))
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
