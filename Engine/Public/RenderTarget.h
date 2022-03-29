#pragma once

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
public:
	enum RT_TYPE { RT_RENDER_TARGET, RT_DEPTH_STENCIL, RT_END};

private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RTV() {
		return m_pRTV;
	}

	ID3D11ShaderResourceView* Get_SRV() {
		return m_pSRV;
	}

	ID3D11DepthStencilView* Get_DSV() {
		return m_pDSV;
	}

public:
	HRESULT NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, RT_TYPE eType = RT_TYPE::RT_RENDER_TARGET);
	HRESULT Clear();

#ifdef _DEBUG // 렌더타겟 화면에 출력할 때, 
public:
	HRESULT Ready_DebugBuffer(_uint iLTX, _uint iLTY, _uint iSizeX, _uint iSizeY);
	HRESULT Render_DebugBuffer(class CVIBuffer_Rect* pVIBuffer, _uint iPassIndex);
	HRESULT PostRender_DebugBuffer(class CVIBuffer_Rect* pVIBuffer, _uint iPassIndex, unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);
#endif // _DEBUG

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	ID3D11Texture2D*			m_pTexture = nullptr;
	ID3D11ShaderResourceView*	m_pSRV = nullptr;
	ID3D11DepthStencilView*		m_pDSV = nullptr;
	ID3D11RenderTargetView*		m_pRTV = nullptr;
	_float4						m_vClearColor;




public:
	void Set_RenderTargetTag(const _tchar* pTag) { m_pRenderTargetTag = pTag; }
	const _tchar* m_pRenderTargetTag = nullptr;

	

#ifdef _DEBUG // 렌더타겟 화면에 출력하는 용도
private:
	_float4x4					m_TransformMatrix;
	_float4x4					m_OrthoMatrix;
	_float2						m_vDebugTextPos;
#endif // _DEBUG

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, RT_TYPE eType = RT_TYPE::RT_RENDER_TARGET);
	virtual void Free() override;
};

END