#pragma once

#include "RenderTarget.h"

BEGIN(Engine)

class CTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CTarget_Manager)
private:
	CTarget_Manager();
	virtual ~CTarget_Manager() = default;

public:
	ID3D11ShaderResourceView* Get_SRV(const _tchar* pTargetTag);
	ID3D11DepthStencilView* Get_DSV(const _tchar* pTargetTag);

public:
	HRESULT Add_RenderTarget(const _tchar* pRenderTargetTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, CRenderTarget::RT_TYPE = CRenderTarget::RT_TYPE::RT_RENDER_TARGET);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pRenderTargetTag);

	HRESULT Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* pDeviceContext, const _tchar* pRenderTargetTag);
	HRESULT End_MRT(ID3D11DeviceContext* pDeviceContext);

#ifdef _DEBUG
public:
	HRESULT Ready_DebugBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pTargetTag, _uint iLTX, _uint iLTY, _uint iSizeX, _uint iSizeY);
	HRESULT Render_DebugBuffer(const _tchar* pMRTTag, _uint iPassIndex);
	HRESULT PostRender_DebugBuffer(const _tchar* pMRTTag, _uint iPassIndex, unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);
#endif // _DEBUG

private:
	map<const _tchar*, CRenderTarget*>				m_RenderTargets;
	typedef map<const _tchar*, CRenderTarget*>		RENDERTARGETS;

private:
	map<const _tchar*, list<CRenderTarget*>>				m_MRTs;
	typedef map<const _tchar*, list<CRenderTarget*>>		MRTs;

private:
	ID3D11RenderTargetView* m_pBackBufferView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

#ifdef _DEBUG
private:
	class CVIBuffer_Rect* m_pVIBuffer = nullptr;
#endif // _DEBUG

private:
	CRenderTarget* Find_RenderTarget(const _tchar* pRenderTargetTag);
	list<CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free();
};

END