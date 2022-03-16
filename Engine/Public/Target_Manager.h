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
	HRESULT Add_RenderTarget(const _tchar* pRenderTargetTag, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pRenderTargetTag);

	HRESULT Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag);
	HRESULT End_MRT(ID3D11DeviceContext* pDeviceContext);

private:
	map<const _tchar*, CRenderTarget*>				m_RenderTargets;
	typedef map<const _tchar*, CRenderTarget*>		RENDERTARGETS;

private:
	map<const _tchar*, list<CRenderTarget*>>				m_MRTs;
	typedef map<const _tchar*, list<CRenderTarget*>>		MRTs;

private:
	ID3D11RenderTargetView* m_pBackBufferView = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;

private:
	CRenderTarget* Find_RenderTarget(const _tchar* pRenderTargetTag);
	list<CRenderTarget*>* Find_MRT(const _tchar* pMRTTag);

public:
	virtual void Free();
};

END