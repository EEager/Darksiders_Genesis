#include "..\public\Graphic_Device.h"

IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
	: m_pDevice(nullptr)
	, m_pDeviceContext(nullptr)
{		
	


}

HRESULT CGraphic_Device::Ready_Graphic_Device(HWND hWnd, WINMODE WinMode, _uint iWinCX, _uint iWinCY, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppDeviceContextOut)
{
	// JJLEE
	// https://commen.tistory.com/106
	// gpu 정보 설정하는 함수들
	//{
	//	HRESULT hr;

	//	IDXGIFactory* factory;
	//	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	//	assert(SUCCEEDED(hr));

	//	// 해외에선 그래픽 어답터라고 함 그래픽 카드를

	//	IDXGIAdapter* adapter;
	//	hr = factory->EnumAdapters(0, &adapter);
	//	assert(SUCCEEDED(hr));

	//	// gpu 정보 가져오는거
	//	IDXGIOutput* adapterOutput;
	//	hr = adapter->EnumOutputs(0, &adapterOutput);
	//	assert(SUCCEEDED(hr));

	//	UINT modeCount;
	//	hr = adapterOutput->GetDisplayModeList
	//	(
	//		DXGI_FORMAT_R8G8B8A8_UNORM
	//		, DXGI_ENUM_MODES_INTERLACED
	//		, &modeCount
	//		, NULL
	//	);
	//	assert(SUCCEEDED(hr));

	//	// r8g8b8a8
	//	// 0 ~ 255(0xff) 앞으로 16진수 많이 다룰꺼

	//	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[modeCount];
	//	hr = adapterOutput->GetDisplayModeList
	//	(
	//		// 16진수 형의 unsinged 쓰겠다는거 (UNORM)
	//		DXGI_FORMAT_R8G8B8A8_UNORM
	//		, DXGI_ENUM_MODES_INTERLACED
	//		, &modeCount
	//		, displayModeList
	//	);
	//	assert(SUCCEEDED(hr));

	//	for (UINT i = 0; i < modeCount; i++)
	//	{
	//		bool isCheck = true;
	//		isCheck &= displayModeList[i].Width == g_iWinCX;
	//		isCheck &= displayModeList[i].Height == g_iWinCY;

	//		if (isCheck == true)
	//		{
	//			auto numerator = displayModeList[i].RefreshRate.Numerator;
	//			auto denominator = displayModeList[i].RefreshRate.Denominator;
	//		}
	//	}

	//	DXGI_ADAPTER_DESC adapterDesc;
	//	hr = adapter->GetDesc(&adapterDesc);
	//	assert(SUCCEEDED(hr));

	//	auto gpuMemorySize = adapterDesc.DedicatedVideoMemory / 1024 / 1024;
	//	auto gpuDescription = adapterDesc.Description;

	//	Safe_Delete_Array(displayModeList);


	//	Safe_Release(adapterOutput);
	//	Safe_Release(adapter);
	//	Safe_Release(factory);
	//}

	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = /*D3D11_CREATE_DEVICE_DEBUG | */D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif
	D3D_FEATURE_LEVEL			FreatureLV[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	/* 그래픽 장치를 초기화한다. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, FreatureLV, &m_pDeviceContext)))
		return E_FAIL;

	if (FAILED(Ready_SwapChain(hWnd, WinMode, iWinCX, iWinCY)))
		return E_FAIL;

	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilRenderTargetView(iWinCX, iWinCY)))
		return E_FAIL;

	/* 장치에 바인드해놓을 렌더타겟들과 뎁스스텐실뷰를 셋팅한다. */
	m_pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pDepthStencilView);		
	
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (float)iWinCX;
	ViewPortDesc.Height = (float)iWinCY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);

	*ppDeviceOut = m_pDevice;
	*ppDeviceContextOut = m_pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 백버퍼를 초기화하낟.  */
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&vClearColor);

	

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	return m_pSwapChain->Present(0/*d3dDesc.bVsync == true?1:0*/, 0);
}


HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, WINMODE eWinMode, _uint iWinCX, _uint iWinCY)
{
	IDXGIDevice*			pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter*			pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory*			pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));
			
	SwapChain.BufferDesc.Width = iWinCX;
	SwapChain.BufferDesc.Height = iWinCY;
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;
	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;
	SwapChain.OutputWindow = hWnd;	
	SwapChain.Windowed = eWinMode;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}


HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pBackBufferTexture = nullptr;

	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;	

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilRenderTargetView(_uint iWinCX, _uint iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;
	
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinCX;
	TextureDesc.Height = iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTarget */
	/* ShaderResource */
	/* DepthStencil */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;	

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
