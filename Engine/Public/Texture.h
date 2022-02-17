#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	ID3D11ShaderResourceView* Get_SRV(_uint iIndex = 0) {
		return m_Textures[iIndex];
	}

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	HRESULT SetUp_OnShader(class CVIBuffer* pVIBuffer, const char* pConstantName, _uint iTextureIndex = 0);


private:	
	_uint									m_iNumTextures = 0;
	vector<ID3D11ShaderResourceView*>			m_Textures;
	typedef vector<ID3D11ShaderResourceView*>	TEXTURES;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pTextureFilePath, _uint iNumTexture = 1);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END