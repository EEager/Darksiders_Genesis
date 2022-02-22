#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Channel.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)	
	, m_pEffect(rhs.m_pEffect)
	, m_PassesDesc(rhs.m_PassesDesc)
	, m_MeshContainers(rhs.m_MeshContainers)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_isAnimMesh(rhs.m_isAnimMesh)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
{
	for (auto& MaterialDesc : m_Materials)
	{
		for (auto& pTexture : MaterialDesc.pTexture)
		{
			Safe_AddRef(pTexture);
		}
	}


	for (auto& MtrlMeshContainer : m_MeshContainers)
	{
		for (auto& pMeshContainer : MtrlMeshContainer)
			Safe_AddRef(pMeshContainer);
	}

	for (auto& pPassDesc : m_PassesDesc)
	{
		Safe_AddRef(pPassDesc->pInputlayout);
		Safe_AddRef(pPassDesc->pPass);
	}

	Safe_AddRef(m_pEffect);

}

HRESULT CModel::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix)
{
	char		szModelPath[MAX_PATH] = "";

	strcpy_s(szModelPath, pModelFilePath);
	strcat_s(szModelPath, pModelFileName);

	/* 1.파일로부터 읽은 여러 정볼르 assimp타입에 맞도록 정리하낟. */
	/* 2.읽어온 정보들을 내가 렌더링할 수 있는 구조로 정리보관한다. */
	/* 2-1. 정점버퍼, 인덱스버퍼, 셰이더(내가만든다) */


	/* 파일로부터 읽은 여러 정볼르 assimp타입에 맞도록 정리하낟. */
	/* m_pScene이라는 놈이 다 가지고 있다. */
	m_pScene = m_Importer.ReadFile(szModelPath, aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_Triangulate);
	if (nullptr == m_pScene)
		return E_FAIL;	

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

 	m_isAnimMesh = m_pScene->HasAnimations();

	m_MeshContainers.resize(m_pScene->mNumMaterials);


	if (true == m_isAnimMesh)
	{
		if (FAILED(Create_HierarchyNodes(m_pScene->mRootNode)))
			return E_FAIL;

		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
			{
				return pSour->Get_Depth() < pDest->Get_Depth();
			});
	}



	if (FAILED(Create_MeshContainers()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffers()))
		return E_FAIL;

	if (FAILED(Create_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Compile_Shader(pShaderFilePath)))
		return E_FAIL;

	///* 뼈의 상속구조를 표현한다. */
	//m_pScene->mRootNode->
	
	///* 실제뼈의 정보다. */
	///* 이 뼈가 어떤 정점들에게 영향을 주고 있는지? 얼마나 영향르 줘야하는지? */
	//m_pScene->mMeshes[0]->mBones[0];
	
	///* 특정 애님에ㅣ션 재생 시에 뼈의 상태 변환을표현하기위한 데\이터.  */
	//m_pScene->mAnimations[0]->mChannels[0]



	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CModel::Update_Animation(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex > m_iNumAnimation)
		return E_FAIL;

	/* 현재 애니메이션 상태에 맞는 뼈의 행렬들을 모두 갱신한다. */
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta);

	/* 노드들을 순회하면서 노드에 컴바인트 트랜스포메이션 행려을 만든다. */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		// pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimIndex);
	}



	return S_OK;
}

HRESULT CModel::Bind_Shader(_uint iPassIndex)
{
	m_pDeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout);
	m_PassesDesc[iPassIndex]->pPass->Apply(0, m_pDeviceContext);

	return S_OK;
}


HRESULT CModel::Render(_uint iMtrlIndex, _uint iPassIndex)
{
	if (iPassIndex >= m_PassesDesc.size())
		return E_FAIL;

	_matrix		BoneMatrices[128] = {};

	for (auto& pMeshContainer : m_MeshContainers[iMtrlIndex])
	{
		_matrix		BoneMatrices[128] = {};

		/* 현재 메시컨테이너에 영향을 주고있는 뼈들의 최종 렌더링행렬값들을ㅇ 받아온다. */
		// pMeshContainer->SetUp_BoneMatrices(BoneMatrices);

		/* 셰이더에 던진다. */


		/* 그려지는 정점들이 뼈들의 행렬집합에서 현재 정점에 영향ㅇ르 주는 뼈의 행렬을 찾아 곱한다음 그린다. */

		Bind_Shader(iPassIndex);

		pMeshContainer->Render();
	}

	return S_OK;
}

HRESULT CModel::Set_RawValue(const char* pConstantName, void* pData, _uint iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;
	ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pVariable)
		return E_FAIL;

	return pVariable->SetRawValue(pData, 0, iSize);
}

HRESULT CModel::Set_ShaderResourceView(const char* pConstantName, _uint iMaterialIndex, aiTextureType eTextureType)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	if (!m_Materials[iMaterialIndex].pTexture[eTextureType])
		return E_FAIL;

	return pVariable->SetResource(m_Materials[iMaterialIndex].pTexture[eTextureType]->Get_SRV());
}

CHierarchyNode* CModel::Find_HierarchyNode(const char* pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode)
		{
			return !strcmp(pNodeName, pNode->Get_Name());
		});

	return *iter;
}

HRESULT CModel::Create_MeshContainers()
{
	if (nullptr == m_pScene)
		return E_FAIL;

	m_iNumMeshes = m_pScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];
		if (nullptr == pMesh)
			return E_FAIL;

		/* 파일로 읽어온 정점과인덱스의 정보들을 저장한다.  */
		CMeshContainer* pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, this, m_isAnimMesh, pMesh, XMLoadFloat4x4(&m_PivotMatrix));
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_MeshContainers[pMesh->mMaterialIndex].push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Create_Materials(const char* pModelFilePath)
{
 	m_iNumMaterials = m_pScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		aiMaterial* pMaterial = m_pScene->mMaterials[i];

		MESHMATERIAL		MeshMaterialDesc;
		ZeroMemory(&MeshMaterialDesc, sizeof(MESHMATERIAL));

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		TexturePath;

			if (FAILED(pMaterial->GetTexture(aiTextureType(j), 0, &TexturePath)))
				continue;

			char		szFileName[MAX_PATH];
			char		szExt[MAX_PATH];

			_splitpath_s(TexturePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char		szFullPath[MAX_PATH] = "";

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar		szPerfectPath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szPerfectPath, MAX_PATH);

			MeshMaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pDeviceContext, szPerfectPath);
			if (nullptr == MeshMaterialDesc.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MeshMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Compile_Shader(const _tchar * pShaderFilePath)
{
	_uint		iNumElements = 0;

	D3D11_INPUT_ELEMENT_DESC	Elements[D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT];
	ZeroMemory(Elements, sizeof(D3D11_INPUT_ELEMENT_DESC) * D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT);


	if (false == m_pScene->HasAnimations())
	{
		iNumElements = 4;

		Elements[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}
	else
	{
		iNumElements = 6;

		Elements[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[2] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[4] = { "BLENDINDEX", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		Elements[5] = { "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	}

	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	iFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG	
	if (FAILED(D3DX11CompileEffectFromFile(pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iFlag, 0, m_pDevice, &m_pEffect, nullptr)))
		return E_FAIL;


	ID3DX11EffectTechnique*	pTechnique = m_pEffect->GetTechniqueByIndex(0);

	D3DX11_TECHNIQUE_DESC			TechniqueDesc;
	ZeroMemory(&TechniqueDesc, sizeof(D3DX11_TECHNIQUE_DESC));

	pTechnique->GetDesc(&TechniqueDesc);

	m_PassesDesc.reserve(TechniqueDesc.Passes);

	for (_uint i = 0; i < TechniqueDesc.Passes; ++i)
	{
		PASSDESC*		pPassDesc = new PASSDESC;

		pPassDesc->pPass = pTechnique->GetPassByIndex(i);

		D3DX11_PASS_DESC		PassDesc;
		ZeroMemory(&PassDesc, sizeof(D3DX11_PASS_DESC));

		pPassDesc->pPass->GetDesc(&PassDesc);

		if (FAILED(m_pDevice->CreateInputLayout(Elements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pPassDesc->pInputlayout)))
			return E_FAIL;

		m_PassesDesc.push_back(pPassDesc);
	}

	return S_OK;
}

HRESULT CModel::Create_VertexIndexBuffers()
{
	for (auto& pMtrlMeshContainer : m_MeshContainers)
	{
		for (auto& pMeshContainer : pMtrlMeshContainer)
			pMeshContainer->Create_VertexIndexBuffer();
	}


	return S_OK;
}

HRESULT CModel::Create_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	_matrix			TransformationMatrix;
	memcpy(&TransformationMatrix, &pNode->mTransformation, sizeof(_matrix));

	CHierarchyNode* pHierarchyNode = CHierarchyNode::Create(pNode->mName.data, XMMatrixTranspose(TransformationMatrix), pParent, iDepth);
	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		Create_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth + 1);
	}

	return S_OK;
}

HRESULT CModel::Create_Animation()
{
	m_iNumAnimation = m_pScene->mNumAnimations;

	for (_uint i = 0; i < m_iNumAnimation; ++i)
	{
		aiAnimation* pAnim = m_pScene->mAnimations[i];

		CAnimation* pAnimation = CAnimation::Create(pAnim->mName.data, pAnim->mDuration, pAnim->mTicksPerSecond);
		if (nullptr == pAnimation)
			return E_FAIL;

		/* 현재 애니메이션에 영향을 주는 뼈의 갯수. */
		for (_uint j = 0; j < pAnim->mNumChannels; ++j)
		{
			aiNodeAnim* pNodeAnim = pAnim->mChannels[j];

			CChannel* pChannel = CChannel::Create(pNodeAnim->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;

			CHierarchyNode* pHierarchyNode = Find_HierarchyNode(pNodeAnim->mNodeName.data);
			if (nullptr == pHierarchyNode)
				return E_FAIL;

			pHierarchyNode->Add_Channel(pChannel);

			/* 이 뼈는 몇개의 키프레임에서 사용되고 있는지? */
			_uint	iNumMaxKeyFrames = max(pNodeAnim->mNumScalingKeys, pNodeAnim->mNumRotationKeys);
			iNumMaxKeyFrames = max(iNumMaxKeyFrames, pNodeAnim->mNumPositionKeys);

			_float3		vScale = _float3(1.f, 1.f, 1.f);
			_float4		vRotation = _float4(0.f, 0.f, 0.f, 0.f);
			_float3		vPosition = _float3(0.f, 0.f, 0.f);

			for (_uint k = 0; k < iNumMaxKeyFrames; ++k)
			{
				KEYFRAME* pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				if (pNodeAnim->mNumScalingKeys > k)
				{
					memcpy(&vScale, &pNodeAnim->mScalingKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pNodeAnim->mScalingKeys[k].mTime;
				}

				if (pNodeAnim->mNumRotationKeys > k)
				{
					vRotation.x = pNodeAnim->mRotationKeys[k].mValue.x;
					vRotation.y = pNodeAnim->mRotationKeys[k].mValue.y;
					vRotation.z = pNodeAnim->mRotationKeys[k].mValue.z;
					vRotation.w = pNodeAnim->mRotationKeys[k].mValue.w;
					pKeyFrame->Time = pNodeAnim->mRotationKeys[k].mTime;
				}

				if (pNodeAnim->mNumPositionKeys > k)
				{
					memcpy(&vPosition, &pNodeAnim->mPositionKeys[k].mValue, sizeof(_float3));
					pKeyFrame->Time = pNodeAnim->mPositionKeys[k].mTime;
				}

				pChannel->Add_KeyFrame(pKeyFrame);
			}

			pAnimation->Add_Channels(pChannel);
		}

		m_Animations.push_back(pAnimation);

	}

	return S_OK;
}



CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pModelFilePath, pModelFileName, PivotMatrix)))
	{
 		MSG_BOX("Failed To Creating CModel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*	pInstance = new CModel(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CModel::Free()
{
	// m_HierarchyNodes
	for (auto& hNode : m_HierarchyNodes)
		Safe_Release(hNode);

	// m_Materials
	for (auto& MaterialDesc : m_Materials)
	{
		for (auto& pTexture : MaterialDesc.pTexture)
		{
			Safe_Release(pTexture);
		}
	}
	m_Materials.clear();

	// m_MeshContainers
	for (auto& MtrlMeshContainer : m_MeshContainers)
	{
		for (auto& pMeshContainer : MtrlMeshContainer)
			Safe_Release(pMeshContainer);
		MtrlMeshContainer.clear();
	}
	if (false == m_isCloned)
		m_MeshContainers.clear();


	// m_PassesDesc
	for (auto& pPassDesc : m_PassesDesc)
	{
		Safe_Release(pPassDesc->pInputlayout);
		Safe_Release(pPassDesc->pPass);
	}

	if (false == m_isCloned)
	{
		for (auto& pPassDesc : m_PassesDesc)
			Safe_Delete(pPassDesc);

		m_PassesDesc.clear();
	}


	// m_pEffect
	Safe_Release(m_pEffect);

	m_Importer.FreeScene();
}
