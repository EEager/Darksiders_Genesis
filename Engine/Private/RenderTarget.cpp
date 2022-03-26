#include "..\Public\RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "DXString.h"

CRenderTarget::CRenderTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
}

HRESULT CRenderTarget::NativeConstruct(_uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	D3D11_TEXTURE2D_DESC		TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));	

	TextureDesc.Width = iWidth;
	TextureDesc.Height = iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = eFormat;

	TextureDesc.SampleDesc.Count = 1;
	TextureDesc.SampleDesc.Quality = 0;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC			SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	SRVDesc.Format = eFormat;
	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	D3D11_RENDER_TARGET_VIEW_DESC			RTVDesc;
	ZeroMemory(&RTVDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

	RTVDesc.Format = eFormat;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;		
	RTVDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, &RTVDesc, &m_pRTV)))
		return E_FAIL;

	m_vClearColor = vClearColor;

	return S_OK;
}

HRESULT CRenderTarget::Clear()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearRenderTargetView(m_pRTV, (_float*)&m_vClearColor);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_DebugBuffer(_uint iLTX, _uint iLTY, _uint iSizeX, _uint iSizeY)
{
	// 랜더타겟 Text 위치
	m_vDebugTextPos = _float2((_float)iLTX, (_float)iLTY);

	_float			fCenterX, fCenterY;

	// _uint iLTX, _uint iLTY, _uint iSizeX, _uint iSizeY)
	// 0,0, 200, 200 이 들어온다했을때
	fCenterX = iLTX + iSizeX * 0.5f; // 100
	fCenterY = iLTY + iSizeY * 0.5f; // 100

	D3D11_VIEWPORT		ViewportDesc;

	_uint			iNumViewport = 1;

	m_pDeviceContext->RSGetViewports(&iNumViewport, &ViewportDesc);

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	m_TransformMatrix._11 = iSizeX; // x크기
	m_TransformMatrix._22 = iSizeY; // y크기

	m_TransformMatrix._41 = fCenterX - (ViewportDesc.Width * 0.5f); // -800 왼쪽 벽 기준 + 100 오른쪽으로 간 위치
	m_TransformMatrix._42 = -fCenterY + (ViewportDesc.Height * 0.5f); // 450 윈쪽 벽 기준 - 100 만큼 내려온 위치

	XMStoreFloat4x4(&m_OrthoMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

	return S_OK;
}
HRESULT CRenderTarget::Render_DebugBuffer(CVIBuffer_Rect* pVIBuffer, _uint iPassIndex)
{
	pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
	pVIBuffer->Set_ShaderResourceView("g_TargetTexture", m_pSRV);

	pVIBuffer->Render(iPassIndex);

	return S_OK;
}
HRESULT CRenderTarget::PostRender_DebugBuffer(CVIBuffer_Rect* pVIBuffer, _uint iPassIndex, unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	// Render Target 종류를 TEXT로 출력하자
	wstring str = DXString::Format(m_pRenderTargetTag);

	const wchar_t* output = str.c_str();

	auto origin = DirectX::g_XMZero;

	_float2 tmpPos;
	// 왼위 0,0 오아 g_iWinCX, g_iWinCY이다.
	tmpPos = m_vDebugTextPos; // 처음 위치
	XMVECTOR m_fontPos = XMLoadFloat2(&tmpPos);

	// Outline Effect
	tmpPos = _float2(1.f, 1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin, 0.5f);
	tmpPos = _float2(-1.f, 1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin, 0.5f);
	tmpPos = _float2(-1.f, -1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin, 0.5f);
	tmpPos = _float2(1.f, -1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin, 0.5f);

	// Origin Text
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos, Colors::White, 0.f, origin, 0.5f);

	return S_OK;
}
#endif // _DEBUG

CRenderTarget * CRenderTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	CRenderTarget*	pInstance = new CRenderTarget(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(iWidth, iHeight, eFormat, vClearColor)))
	{
		MSG_BOX("Failed To Creating CRenderTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRenderTarget::Free()
{
	Safe_Release(m_pRTV);
	Safe_Release(m_pSRV);
	Safe_Release(m_pTexture);
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);

}
