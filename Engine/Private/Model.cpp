#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Channel.h"
#include "GameInstance.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)	
	, m_pEffect(rhs.m_pEffect)
	, m_PassesDesc(rhs.m_PassesDesc)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_eType(rhs.m_eType)
	, m_Materials(rhs.m_Materials)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_HierarchyNodes(rhs.m_HierarchyNodes)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_AniNameKey_IdxValue_Map(rhs.m_AniNameKey_IdxValue_Map)
	, m_iNumAnimation(rhs.m_iNumAnimation)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_Center(rhs.m_Center)
	, m_Extents(rhs.m_Extents)
	, m_vEnviromentBase(rhs.m_vEnviromentBase)
{
	for (auto& MaterialDesc : m_Materials)
	{
		for (auto& pTexture : MaterialDesc.pTexture)
		{
			Safe_AddRef(pTexture);
		}
	}


	// m_MeshContainers(정점, 인덱스 버퍼들)는 얕은 복사를 하지 않지말자. 원본 모델의 CMeshContainer를 clone을 하여 내꺼에 넣어주자. 왜? Get_MaterialIndex()을 하기 위해... 
	m_MeshContainers.resize(m_iNumMaterials);
	for (auto& MtrlMeshContainer : rhs.m_MeshContainers)
	{
		for (auto& pPrototypeMeshContainer : MtrlMeshContainer)
		{
			CMeshContainer* pMeshContainer = (CMeshContainer*)pPrototypeMeshContainer->Clone(nullptr);
			if (nullptr == pMeshContainer)
				return;

			m_MeshContainers[pMeshContainer->Get_MaterialIndex()].push_back(pMeshContainer);

		}

	}

	// 얕.복
	//for (auto& pPassDesc : m_PassesDesc)
	//{
	//	Safe_AddRef(pPassDesc->pInputlayout);
	//	Safe_AddRef(pPassDesc->pPass);
	//}

	// 얕.복
	Safe_AddRef(m_pEffect);

	// m_Animations는 얕은 복사를 하지말고, Clone을 하자. 왜? 아래 내용들은 Monster1, Monster2가 각자 가지고 가야하기때문이다
	/*
	CAnimation:
		m_Duration, m_fTimeAcc, m_isFinished, m_TickPerSecond, m_szName, m_Channels
	*/
	for (auto& pPrototypeAnim : rhs.m_Animations)
	{
		m_Animations.push_back(pPrototypeAnim->Clone());
	}

}

_float4x4* CModel::Get_CombinedMatrixPtr(const char* pBoneName)
{
	CHierarchyNode* pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		return nullptr;

	return pNode->Get_CombinedMatixPtr();
}

_float4x4 CModel::Get_OffsetMatrix(const char* pBoneName)
{
	_float4x4		OffsetMatrix;

	CHierarchyNode* pNode = Find_HierarchyNode(pBoneName);
	if (nullptr == pNode)
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());
	else
		XMStoreFloat4x4(&OffsetMatrix, pNode->Get_OffsetMatix());

	return OffsetMatrix;
}

HRESULT CModel::NativeConstruct_Prototype(TYPE eType, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix)
{
	char		szModelPath[MAX_PATH] = "";

	strcpy_s(szModelPath, pModelFilePath);
	strcat_s(szModelPath, pModelFileName);

	/* 1.파일로부터 읽은 여러 정볼르 assimp타입에 맞도록 정리하낟. */
	/* 2.읽어온 정보들을 내가 렌더링할 수 있는 구조로 정리보관한다. */
	/* 2-1. 정점버퍼, 인덱스버퍼, 셰이더(내가만든다) */


	/* 파일로부터 읽은 여러 정볼르 assimp타입에 맞도록 정리하낟. */
	_uint			iFlag = 0;
	m_eType = eType;

	if (TYPE_NONANIM == eType) // 애니메이션이 없는 경우) aiProcess_PreTransformVertices: 모든 정점에 로컬 transformation을 미리 곱해준다. 
		iFlag = aiProcess_PreTransformVertices | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_Triangulate;
	else 
		iFlag = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace | aiProcess_Triangulate;

	m_pScene = m_Importer.ReadFile(szModelPath, iFlag);
	/* m_pScene이라는 놈이 다 가지고 있다. */
	if (nullptr == m_pScene)
		return E_FAIL;	

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_MeshContainers.resize(m_pScene->mNumMaterials);


	if (FAILED(Create_MeshContainers()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffers()))
		return E_FAIL;

	if (FAILED(Create_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Compile_Shader(pShaderFilePath)))
		return E_FAIL;

	if (FAILED(Create_Animation()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	// 1. 애니메이션이 아니면, m_HierarchyNodes, m_Animations를 굳이 채울 필요가 없다. 
	if (TYPE_NONANIM == m_eType) 
		return S_OK;

	// 2. this 모델이 다른 모델의 애니메이션을 따라가고 싶다. 그걸 복사하자. ex) War Gauntlet같은 경우 애니메이션없고, 뼈는 있는데, War 애니메이션을 따라가고 싶다.
	if (TYPE_ANIM_USE_OTHER == m_eType) 
	{
		MODELDESC* pModelDesc = (MODELDESC*)pArg;
		m_HierarchyNodes.clear();
		//m_HierarchyNodes.assign(pModelDesc->pHierarchyNodes->begin(), pModelDesc->pHierarchyNodes->end());
		for (auto& pHierarchy : *pModelDesc->pHierarchyNodes)
		{
			Safe_AddRef(pHierarchy);
			m_HierarchyNodes.push_back(pHierarchy);
		}

		m_Animations.clear();
		for (auto& pAnimation : *pModelDesc->pAnimations)
		{
			Safe_AddRef(pAnimation);
			m_Animations.push_back(pAnimation);
		}
		m_iNumAnimation = (_uint)m_Animations.size();

		for (auto& MtrlMeshContainers : m_MeshContainers)
		{
			for (auto& pMeshContainer : MtrlMeshContainers)
			{
				pMeshContainer->Add_Bones(this);
			}
		}
	}
	else // if (TYPE_ANIM == m_eType) and if (TYPE_ANIM_need_continue == m_eType) Weapon같은 경우 끝에 플레이어의 뼈를 달자
	{
		// m_HierarchyNodes를 채운다.
		if (FAILED(Create_HierarchyNodes(m_pScene->mRootNode)))
			return E_FAIL;

		sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
			{
				return pSour->Get_Depth() < pDest->Get_Depth();
			});

		for (auto& MtrlMeshContainers : m_MeshContainers)
		{
			for (auto& pMeshContainer : MtrlMeshContainers)
			{
				// pArg가 있을 경우, 마지막에 Add_Bones한다. 
				if (pArg)
				{
					MODELDESC* pModelDesc = (MODELDESC*)pArg;
					if (pModelDesc->pHierarchyNode)
						pMeshContainer->Add_Bones(this, pModelDesc->pHierarchyNode);
				}
				else
					pMeshContainer->Add_Bones(this);// m_MeshContainers안에 있는 m_Bones(CHierarchyNode*) 를 채운다.

			}
		}

		// Depth가 작은 뼈부터 시작(루트노드부터시작). 하이카이 노드의 m_CombinedTransformationMatrix를 누적 계산한다.
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Update_CombinedTransformationMatrix();
		}


		for (_uint i = 0; i < m_iNumAnimation; ++i)
		{
			vector<class CChannel*>* pChannels = m_Animations[i]->Get_Channels();

			for (auto& pChannel : *pChannels)
			{
				CHierarchyNode* pHierarchyNode = Find_HierarchyNode(pChannel->Get_Name());
				if (nullptr == pHierarchyNode)
					return E_FAIL;

				pHierarchyNode->Add_Channel(i, pChannel);
			}
		}
	}

	return S_OK;
}

void CModel::SetUp_Animation(_uint iAnimIndex, _bool isLoop) {

	if (iAnimIndex >= m_iNumAnimation)
		return;

	// 다음 애니메이션에 이전 채널정보를 넘겨준다.
	m_Animations[iAnimIndex]->Set_Latest_Channels(m_Animations[m_iCurrentAnimIndex]->Get_Channels());
	m_Animations[iAnimIndex]->SetBeginFirst();
	m_iCurrentAnimIndex = iAnimIndex;
	m_isLoop = isLoop;
}

void CModel::SetUp_Animation(const char* pNameKey, _bool isLoop, _bool useLatestLerp) {

	auto findIter = m_AniNameKey_IdxValue_Map.find(pNameKey);
	if (findIter == m_AniNameKey_IdxValue_Map.end())
		return;

	_uint iAnimIndex = findIter->second;
	if (iAnimIndex >= m_iNumAnimation)
		return;

	if (useLatestLerp)
		m_Animations[iAnimIndex]->Set_Latest_Channels(m_Animations[m_iCurrentAnimIndex]->Get_Channels());
	else
		m_Animations[iAnimIndex]->Set_Latest_Channels(nullptr);

	m_Animations[iAnimIndex]->SetBeginFirst();
	m_iCurrentAnimIndex = iAnimIndex;
	m_isLoop = isLoop;
}

void CModel::SetUp_Animation(string pNameKey, _bool isLoop, _bool useLatestLerp) {

	SetUp_Animation(pNameKey.c_str(), isLoop, useLatestLerp);
}

_bool CModel::Get_Animation_isFinished(const char* pNameKey)
{
	auto findIter = m_AniNameKey_IdxValue_Map.find(pNameKey);
	assert(findIter != m_AniNameKey_IdxValue_Map.end());

	_uint iAnimIndex = findIter->second;
	assert(iAnimIndex < m_iNumAnimation);
	
	return m_Animations[iAnimIndex]->Get_isFinished();
}

_uint CModel::Get_Current_KeyFrame_Index(const char* pNameKey)
{
	auto findIter = m_AniNameKey_IdxValue_Map.find(pNameKey);
	assert(findIter != m_AniNameKey_IdxValue_Map.end());

	_uint iAnimIndex = findIter->second;
	assert(iAnimIndex < m_iNumAnimation);

	return m_Animations[iAnimIndex]->GetCurIdx();
}

HRESULT CModel::Update_Animation(_float fTimeDelta, OUT _float4x4* pMatW, const char* pRootNodeName, CNavigation* pNaviCom, IN OBJECT_DIR eDir, int iChannelIdx, _float OffsetMul)
{
	if (TYPE_ANIM != m_eType) // TYPE_NONANIM, TYPE_ANIM_USE_OTHER 인경우에는 Update안한다. TYPE_ANIM_USE_OTHER 경우 Update 하면 2번 업데이트된다. 
		return S_OK;

	if (m_iCurrentAnimIndex > m_iNumAnimation || m_iNumAnimation == 0)
		return E_FAIL;

	/* 현재 애니메이션 상태에 맞는 뼈의 행렬들을 모두 갱신한다. */
	m_Animations[m_iCurrentAnimIndex]->Update_TransformationMatrix(fTimeDelta, m_isLoop, iChannelIdx);

	/* 노드들을 순회하면서 노드에 컴바인트 트랜스포메이션 행려을 만든다. */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Update_CombinedTransformationMatrix(m_iCurrentAnimIndex, pMatW, pRootNodeName, pNaviCom, eDir, OffsetMul);
	}

	return S_OK;
}

HRESULT CModel::Bind_Shader(_uint iPassIndex)
{
	m_pDeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout.Get());
	m_PassesDesc[iPassIndex]->pPass->Apply(0, m_pDeviceContext);

	return S_OK;
}


HRESULT CModel::Render(_uint iMtrlIndex, _uint iPassIndex)
{
	if (iPassIndex >= m_PassesDesc.size())
		return E_FAIL;

	for (auto& pMeshContainer : m_MeshContainers[iMtrlIndex]) // 1. 정점들 룹을 돈다.
	{
		if (TYPE_NONANIM != m_eType) // 2. TYPE_ANIM_USE_OTHER, TYPE_ANIM 모델인경우 메쉬에 뼈들 최종 행렬 곱해줘야한다. 
		{
#define MAX_BONE_NUM 192
			_float4x4		BoneMatrices[MAX_BONE_NUM];
			ZeroMemory(BoneMatrices, sizeof(_float4x4) * MAX_BONE_NUM);

			// BoneMatrices 192개에 모두 다 채워서 
			pMeshContainer->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			// 셰이더에 던진다. 셰이더에서는 위에서 채워진 192개의 행렬들중 본인 정점에 영향받는 뼈들 (In.vBlendIndex)을 인덱싱한 후, In.vBlendWeight 가중치를 곱해 최종 행렬을 구한다. 
			// ex) g_BoneMatrices.Bones[7] * 0.1 + g_BoneMatrices.Bones[123] * 0.2 ... 
			if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * MAX_BONE_NUM)))
				return E_FAIL;
		}

		Bind_Shader(iPassIndex);

		pMeshContainer->Render();
	}

	return S_OK;
}

// 칼 같은 것
HRESULT CModel::Render(_uint iMtrlIndex, _fmatrix vMat, _uint iPassIndex)
{
	if (iPassIndex >= m_PassesDesc.size())
		return E_FAIL;

	for (auto& pMeshContainer : m_MeshContainers[iMtrlIndex]) // 1. 정점들 룹을 돈다.
	{
#define MAX_BONE_NUM 192
		_float4x4		BoneMatrices[MAX_BONE_NUM];
		ZeroMemory(BoneMatrices, sizeof(_float4x4) * MAX_BONE_NUM);
		XMStoreFloat4x4(&BoneMatrices[0], XMMatrixTranspose(vMat));

		if (FAILED(Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * MAX_BONE_NUM)))
			return E_FAIL;

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

	if (!m_Materials[iMaterialIndex].pTexture[eTextureType])
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pVariable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pVariable)
		return E_FAIL;

	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	return pVariable->SetResource(m_Materials[iMaterialIndex].pTexture[eTextureType]->Get_SRV());
}


HRESULT CModel::Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable* pValiable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pValiable)
		return E_FAIL;

	return pValiable->SetResource(pSRV);
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

	// For.Collider
	// Model 한테 Center, Extents를 채워주자. Collider용 Pivot과 Size를 계산하기 쉽게 해주자
	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

	// AABB, OBB 등 콜라이더 박스를 구하자
	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		aiMesh*		pMesh = m_pScene->mMeshes[i];
		if (nullptr == pMesh)
			return E_FAIL;

		/* 파일로 읽어온 정점과인덱스의 정보들을 저장한다.  */
		CMeshContainer* pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, this, pMesh, XMLoadFloat4x4(&m_PivotMatrix), &vMin, &vMax);
		if (nullptr == pMeshContainer) 
			return E_FAIL;

		m_MeshContainers[pMesh->mMaterialIndex].push_back(pMeshContainer);
	}

	// For.Collider
	Set_Center(0.5f * (vMin + vMax));
	Set_Extents((vMax - vMin));

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


	if (TYPE_NONANIM == m_eType)
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

	pHierarchyNode->Reserve_Channels(m_pScene->mNumAnimations);

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

		CAnimation* pAnimation = CAnimation::Create(pAnim->mName.data, pAnim->mDuration, pAnim->mTicksPerSecond * 1.1f);
		if (nullptr == pAnimation)
			return E_FAIL;

		/* 현재 애니메이션에 영향을 주는 뼈의 갯수. */
		for (_uint j = 0; j < pAnim->mNumChannels; ++j)
		{
			aiNodeAnim* pNodeAnim = pAnim->mChannels[j];

			CChannel* pChannel = CChannel::Create(pNodeAnim->mNodeName.data);
			if (nullptr == pChannel)
				return E_FAIL;

			/* 이 뼈는 몇개의 키프레임에서 사용되고 있는지?, 사실상 모든 키프레임 개수가 같기때문에 이부분은 생략가능하다. */
			_uint	iNumMaxKeyFrames = max(pNodeAnim->mNumScalingKeys, pNodeAnim->mNumRotationKeys);
			iNumMaxKeyFrames = max(iNumMaxKeyFrames, pNodeAnim->mNumPositionKeys);

			_float3		vScale = _float3(1.f, 1.f, 1.f);
			_float4		vRotation = _float4(0.f, 0.f, 0.f, 0.f);
			_float3		vPosition = _float3(0.f, 0.f, 0.f);

			for (_uint k = 0; k < iNumMaxKeyFrames; ++k)
			{
				KEYFRAME* pKeyFrame = new KEYFRAME;
				ZeroMemory(pKeyFrame, sizeof(KEYFRAME));

				if (pNodeAnim->mNumScalingKeys > k) // 사실상 모든 키프레임이 159개로 동일한 키프레임 개수를 가지고 있어, 이부분은 생략가능하다.
				{
					memcpy(&vScale, &pNodeAnim->mScalingKeys[k].mValue, sizeof(_float3));
					// pKeyFrame->Time = pNodeAnim->mScalingKeys[k].mTime; // 사실상 모든 time마다 키프레임이 존재하기때문에 pKeyFrame->Time는 한번만해도된다. 
				}

				if (pNodeAnim->mNumRotationKeys > k) // 사실상 모든 키프레임이 159개로 동일한 키프레임 개수를 가지고 있어, 이부분은 생략가능하다.
				{
					vRotation.x = pNodeAnim->mRotationKeys[k].mValue.x;
					vRotation.y = pNodeAnim->mRotationKeys[k].mValue.y;
					vRotation.z = pNodeAnim->mRotationKeys[k].mValue.z;
					vRotation.w = pNodeAnim->mRotationKeys[k].mValue.w;
					//pKeyFrame->Time = pNodeAnim->mRotationKeys[k].mTime;
				}

				if (pNodeAnim->mNumPositionKeys > k) // 사실상 모든 키프레임이 159개로 동일한 키프레임 개수를 가지고 있어, 이부분은 생략가능하다.
				{
					memcpy(&vPosition, &pNodeAnim->mPositionKeys[k].mValue, sizeof(_float3));
					//pKeyFrame->Time = pNodeAnim->mPositionKeys[k].mTime;
				}

				pKeyFrame->Time = pNodeAnim->mPositionKeys[k].mTime;
				pKeyFrame->vScale = vScale;
				pKeyFrame->vRotation = vRotation;
				pKeyFrame->vPosition = vPosition;

				pChannel->Add_KeyFrame(pKeyFrame); // 채널(뼈)가 키프레임들의 정보를 가지게된다. 
			}

			pAnimation->Add_Channels(pChannel); // 159개의 키프레임 정보가 담긴 채널을 애니메이션 객체에 push_back하여 넣어주자
		}

		m_AniNameKey_IdxValue_Map.emplace(pAnim->mName.data, (_int)m_Animations.size());
		m_Animations.push_back(pAnimation); // Walk.ao 정보를 모델이 가지고 있게하자

	}

	return S_OK;
}



CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix)
{
	CModel* pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(eType, pShaderFilePath, pModelFilePath, pModelFileName, PivotMatrix)))
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
	// m_Animations 
	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();
	m_AniNameKey_IdxValue_Map.clear();

	// m_HierarchyNodes
	for (auto& hNode : m_HierarchyNodes)
		Safe_Release(hNode);
	m_HierarchyNodes.clear();

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


	if (false == m_isCloned)
	{
		for (auto& pPassDesc : m_PassesDesc)
			Safe_Delete(pPassDesc);
	}
	m_PassesDesc.clear();


	// m_pEffect
	Safe_Release(m_pEffect);

	m_Importer.FreeScene();
}
