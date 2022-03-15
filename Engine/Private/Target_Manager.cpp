#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"

CTarget_Manager::CTarget_Manager()
{

}

HRESULT CTarget_Manager::Add_RenderTarget(const _tchar * pRenderTargetTag, ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor)
{
	if (nullptr != Find_RenderTarget(pRenderTargetTag))
		return E_FAIL;

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iWidth, iHeight, eFormat, vClearColor);
	if (nullptr == pRenderTarget)
		return E_FAIL;


	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pRenderTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pRenderTargetTag));

	if(iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

void CTarget_Manager::Free()
{
}
