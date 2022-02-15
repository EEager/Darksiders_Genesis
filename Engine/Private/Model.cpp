#include "..\Public\Model.h"
#include "MeshContainer.h"

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pScene(rhs.m_pScene)	
{

}

HRESULT CModel::NativeConstruct_Prototype(const _tchar * pShaderFilePath, const char * pModelFilePath, const char * pModelFileName)
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

 	m_isAnimMesh = m_pScene->HasAnimations();

	if (FAILED(Create_MeshContainers()))
		return E_FAIL;

	if (FAILED(Create_VertexIndexBuffers()))
		return E_FAIL;

	//if (FAILED(Compile_Shader(pShaderFilePath)))
	//	return E_FAIL;





	return S_OK;
}

HRESULT CModel::NativeConstruct(void * pArg)
{
	return S_OK;
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
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pDeviceContext, m_isAnimMesh, pMesh);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_MeshContainers.push_back(pMeshContainer);
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
	for (auto& pMeshContainer : m_MeshContainers)
		pMeshContainer->Create_VertexIndexBuffer();

	return S_OK;
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pShaderFilePath, const char * pModelFilePath, const char * pModelFileName)
{
	CModel*	pInstance = new CModel(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pModelFilePath, pModelFileName)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Rect");
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
	for (auto& pMeshContainer : m_MeshContainers)
		Safe_Release(pMeshContainer);
	m_MeshContainers.clear();

	m_Importer.FreeScene();
}
