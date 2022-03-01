#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_VBDesc(rhs.m_VBDesc)
	, m_IBDesc(rhs.m_IBDesc)
	, m_VBSubresourceData(rhs.m_VBSubresourceData)
	, m_IBSubresourceData(rhs.m_IBSubresourceData)
	, m_iStride(rhs.m_iStride)
	, m_iNumVertices(rhs.m_iNumVertices)
	, m_iNumPrimitive(rhs.m_iNumPrimitive)
	, m_iIndicesSize(rhs.m_iIndicesSize)
	, m_pVertices(rhs.m_pVertices)
	, m_pPrimitiveIndices(rhs.m_pPrimitiveIndices)
	, m_iNumIndicesPerFigure(rhs.m_iNumIndicesPerFigure)
	, m_eIndexFormat(rhs.m_eIndexFormat)
	, m_ePrimitiveTopology(rhs.m_ePrimitiveTopology)
	, m_PassesDesc(rhs.m_PassesDesc)
	, m_iNumVertexBuffers(rhs.m_iNumVertexBuffers)
	, m_pEffect(rhs.m_pEffect)
{
	/*for (auto& pPassDesc : m_PassesDesc)
	{
		Safe_AddRef(pPassDesc->pInputlayout);
		Safe_AddRef(pPassDesc->pPass);
	}*/

	Safe_AddRef(m_pEffect);
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 장치에 동시에 여러개의 정점버퍼를 셋팅할 수 있기 때문. */
	ID3D11Buffer*		pBuffers[] = {
		m_pVB		
	};

	_uint		iStrides[] = {
		m_iStride
	};

	_uint		iOffset[] = {
		0
	};

	m_pDeviceContext->IASetVertexBuffers(m_iNumVertexBuffers - 1, m_iNumVertexBuffers, pBuffers, iStrides, iOffset);

	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);	
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	/* 정점셰이더에 입력되는 정점의 구성정보. */
	m_pDeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout.Get());

	if (FAILED(m_PassesDesc[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerFigure * m_iNumPrimitive, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Set_RawValue(const char* pConstantName, void* pData, _uint iSize)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectVariable*		pValiable = m_pEffect->GetVariableByName(pConstantName);
	if (nullptr == pValiable)
		return E_FAIL;

	return pValiable->SetRawValue(pData, 0, iSize);	
}

HRESULT CVIBuffer::Set_ShaderResourceView(const char * pConstantName, ID3D11ShaderResourceView * pSRV)
{
	if (nullptr == m_pEffect)
		return E_FAIL;

	ID3DX11EffectShaderResourceVariable*		pValiable = m_pEffect->GetVariableByName(pConstantName)->AsShaderResource();
	if (nullptr == pValiable)
		return E_FAIL;

	return pValiable->SetResource(pSRV);	
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBDesc, &m_VBSubresourceData, &m_pVB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	if (FAILED(m_pDevice->CreateBuffer(&m_IBDesc, &m_IBSubresourceData, &m_pIB)))
		return E_FAIL;

	return S_OK;	
}

HRESULT CVIBuffer::Compile_Shader(D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const _tchar* pShaderFilePath)
{
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

		if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pPassDesc->pInputlayout)))
			return E_FAIL;

		m_PassesDesc.push_back(pPassDesc);
	}

	return S_OK;
}

void CVIBuffer::Free()
{
	__super::Free();

	if (false == m_isCloned) // if, onriginal 
	{
		for (auto& pPassDesc : m_PassesDesc)
			Safe_Delete(pPassDesc);

		Safe_Delete_Array(m_pVertices);
		Safe_Delete_Array(m_pPrimitiveIndices);
	}	
	m_PassesDesc.clear();

	Safe_Release(m_pEffect);

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
