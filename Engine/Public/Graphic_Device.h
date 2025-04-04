#pragma once

#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

public:
	enum WINMODE { MODE_FULL, MODE_WIN, MODE_END };

public:
	typedef struct tagGraphicDeviceDesc
	{
		HWND			hWnd;
		unsigned int	iWinCX, iWinCY;
		WINMODE			eWinMode;
	}GRAPHICDEVDESC;

public:
	CGraphic_Device();
	virtual ~CGraphic_Device() = default;
public:
	HRESULT Ready_Graphic_Device(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut);
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);
	HRESULT Clear_DepthStencil_View();
	HRESULT Present();

public:
	ID3D11Device* GetDevice() { return m_pDevice; }
	ID3D11DeviceContext* GetDC() { return m_pDeviceContext; }
	IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }

	// for shadow map
	ID3D11RenderTargetView* Get_BackBufferRTV() { return m_pBackBufferRTV; }
	ID3D11DepthStencilView* Get_DepthStencilView() { return m_pDepthStencilView; }
	D3D11_VIEWPORT* Get_ViewPortDesc_Ptr() { return &m_ViewPortDesc; }

private:	

	/* 메모리 할당. (정점버퍼, 인덱스버퍼, 텍스쳐로드) */
	ID3D11Device*			m_pDevice = nullptr;

	/* 바인딩작업. 기능실행 (정점버퍼를 SetStreamSource(), SetIndices(), SetTransform(), SetTexture() */
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	IDXGISwapChain*			m_pSwapChain = nullptr;	

	// ID3D11ShaderResourceView*
	ID3D11RenderTargetView*		m_pBackBufferRTV = nullptr;
	ID3D11DepthStencilView*		m_pDepthStencilView = nullptr;

	D3D11_VIEWPORT			m_ViewPortDesc;


private:
	HRESULT Ready_SwapChain(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY);
	HRESULT Ready_BackBufferRenderTargetView();
	HRESULT Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY);


public:
	virtual void Free() override;
};

END