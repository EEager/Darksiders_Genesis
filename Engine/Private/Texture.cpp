#include "..\Public\Texture.h"
#include "VIBuffer.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)	
{
		
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
{
	for (auto& pSRV : m_Textures)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::NativeConstruct_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;

	m_Textures.reserve(iNumTextures);

	for (_uint i = 0; i < iNumTextures; ++i)
	{		
		_tchar		szFullPath[MAX_PATH] = TEXT("");

		wsprintf(szFullPath, pTextureFilePath, i);

		DirectX::ScratchImage* Image = new DirectX::ScratchImage();

		_tchar	szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		if (!lstrcmp(szExt, TEXT(".dds")))		
			DirectX::LoadFromDDSFile(szFullPath, DDS_FLAGS_NONE, nullptr, *Image);

		else if (!lstrcmp(szExt, TEXT(".tga")))
			DirectX::LoadFromTGAFile(szFullPath, nullptr, *Image);
		else
			DirectX::LoadFromWICFile(szFullPath, WIC_FLAGS_NONE, nullptr, *Image);

		ComPtr<ID3D11Resource>			pTextureResource;

		if (FAILED(DirectX::CreateTexture(m_pDevice, Image->GetImages(), Image->GetImageCount(), Image->GetMetadata(), pTextureResource.GetAddressOf())))
			return E_FAIL;

		ID3D11ShaderResourceView*	pSRV = nullptr;
		if (FAILED(m_pDevice->CreateShaderResourceView(pTextureResource.Get(), nullptr, &pSRV)))
			return E_FAIL;

		m_Textures.push_back(pSRV);
		m_ScratchImages.push_back(Image);

	}

	return S_OK;
}

HRESULT CTexture::NativeConstruct(void * pArg)
{
	return S_OK;
}

HRESULT CTexture::SetUp_OnShader(CVIBuffer * pVIBuffer, const char * pConstantName, _uint iTextureIndex)
{
	if (nullptr == pVIBuffer || 
		iTextureIndex >= m_Textures.size())
		return E_FAIL;

	return pVIBuffer->Set_ShaderResourceView(pConstantName, m_Textures[iTextureIndex]);
	
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pTextureFilePath, _uint iNumTexture)
{
	CTexture*		pInstance = new CTexture(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pTextureFilePath, iNumTexture)))
	{
		MSG_BOX("Failed to Created CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
	
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Created CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_Textures)
		Safe_Release(pSRV);

	for (auto& pSIMG : m_ScratchImages)
		Safe_Delete(pSIMG);

	m_Textures.clear();

}
