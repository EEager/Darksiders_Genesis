#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() {
		return m_pRTV;
	}

public:
	HRESULT NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);
	HRESULT Clear();

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	ID3D11Texture2D*			m_pTexture = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;
	_float4						m_vClearColor;

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);
	virtual void Free() override;
};

END