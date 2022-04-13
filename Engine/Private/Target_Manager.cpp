#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"
#include "VIBuffer_Rect.h"
#include "Graphic_Device.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{

}

ID3D11ShaderResourceView* CTarget_Manager::Get_SRV(const _tchar* pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_SRV();
}

ID3D11DepthStencilView* CTarget_Manager::Get_DSV(const _tchar* pTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_DSV();
}

HRESULT CTarget_Manager::Add_RenderTarget(const _tchar * pRenderTargetTag, ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _uint iWidth, _uint iHeight, DXGI_FORMAT eFormat, _float4 vClearColor, CRenderTarget::RT_TYPE eType)
{
	if (nullptr != Find_RenderTarget(pRenderTargetTag))
	{
		MSG_BOX("Add_RenderTarget Already exists!");
		return S_OK; 
	}

	CRenderTarget*		pRenderTarget = CRenderTarget::Create(pDevice, pDeviceContext, iWidth, iHeight, eFormat, vClearColor, eType);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	// RenderTarget �̸��鵵 ����ϰԲ�����.
	pRenderTarget->Set_RenderTargetTag(pRenderTargetTag);

	m_RenderTargets.emplace(pRenderTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _tchar* pMRTTag, const _tchar* pRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CRenderTarget*>		MRTList;
		MRTList.push_back(pRenderTarget);
		m_MRTs.emplace(pMRTTag, MRTList);
	}
	else
		pMRTList->push_back(pRenderTarget);


	Safe_AddRef(pRenderTarget);


	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag)
{
	/* ������ mrt�� �ִ� ����Ÿ�ٵ��� ��ġ�� ���ε��Ѵ�. */
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	/* ��ġ�� �����Ǿ��ִ� �����, ���̽��ٽǹ��۸� ���ͼ� ��������� �����Ѵ�.  */
	/* ��? ���ο� Ÿ�ٵ��� ��ġ�� ���ε��ϸ� ����۸� �Ҿ�����ϱ�. */
	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iIndex = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iIndex++] = pRenderTarget->Get_RTV();
	}

	// The shadow might might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	pDeviceContext->PSSetShaderResources(0, 16, nullSRV);

	// ��ġ�� �������� ����Ÿ�ٵ��� ���ε��Ѵ�
	pDeviceContext->OMSetRenderTargets((_uint)pMRTList->size(), pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* pDeviceContext)
{
	if (nullptr == m_pBackBufferView ||
		nullptr == m_pDepthStencilView)
		return E_FAIL;

	pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView); // ��ġ�� ����۸� ���ε��Ͽ� �����Ѵ�

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);


	return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT_Alpha(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag)
{
	/* ������ mrt�� �ִ� ����Ÿ�ٵ��� ��ġ�� ���ε��Ѵ�. */
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	/* ��ġ�� �����Ǿ��ִ� �����, ���̽��ٽǹ��۸� ���ͼ� ��������� �����Ѵ�.  */
	/* ��? ���ο� Ÿ�ٵ��� ��ġ�� ���ε��ϸ� ����۸� �Ҿ�����ϱ�. */
	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iIndex = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		if (iIndex!=0) // ù��°�� PreBB�ε� Clear�ϸ�ȵȴ�.
			pRenderTarget->Clear();
		pRenderTargets[iIndex++] = pRenderTarget->Get_RTV();
	}

	// The shadow might might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	pDeviceContext->PSSetShaderResources(0, 16, nullSRV);

	// ��ġ�� �������� ����Ÿ�ٵ��� ���ε��Ѵ�
	pDeviceContext->OMSetRenderTargets((_uint)pMRTList->size(), pRenderTargets, m_pDepthStencilView);

	return S_OK;
}



HRESULT CTarget_Manager::Begin_MRT_PreBB(ID3D11DeviceContext* pDeviceContext, const _tchar* pMRTTag)
{
	/* ������ mrt�� �ִ� ����Ÿ�ٵ��� ��ġ�� ���ε��Ѵ�. */
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	/* ��ġ�� �����Ǿ��ִ� �����, ���̽��ٽǹ��۸� ���ͼ� ��������� �����Ѵ�.  */
	/* ��? ���ο� Ÿ�ٵ��� ��ġ�� ���ε��ϸ� ����۸� �Ҿ�����ϱ�. */
	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferView_Real, &m_pDepthStencilView_Real);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iIndex = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iIndex++] = pRenderTarget->Get_RTV();
	}

	// The shadow might might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	pDeviceContext->PSSetShaderResources(0, 16, nullSRV);

	// ��ġ�� �������� ����Ÿ�ٵ��� ���ε��Ѵ�
	pDeviceContext->OMSetRenderTargets((_uint)pMRTList->size(), pRenderTargets, m_pDepthStencilView_Real);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT_PreBB(ID3D11DeviceContext* pDeviceContext)
{
	if (nullptr == m_pBackBufferView_Real ||
		nullptr == m_pDepthStencilView_Real)
		return E_FAIL;

	pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferView_Real , m_pDepthStencilView_Real); // ��ġ�� ����۸� ���ε��Ͽ� �����Ѵ�

	Safe_Release(m_pBackBufferView_Real);
	Safe_Release(m_pDepthStencilView_Real);


	return S_OK;
}

// For.TEXT("MRT_Shadows")
HRESULT CTarget_Manager::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* pDeviceContext, const _tchar* pRenderTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(pRenderTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	D3D11_VIEWPORT mViewport;
	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(SHADOWMAP_SIZE_X);
	mViewport.Height = static_cast<float>(SHADOWMAP_SIZE_Y);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	pDeviceContext->RSSetViewports(1, &mViewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	pDeviceContext->OMSetRenderTargets(1, renderTargets, pRenderTarget->Get_DSV());
	pDeviceContext->ClearDepthStencilView(pRenderTarget->Get_DSV(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	// The shadow might might be at any slot, so clear all slots.
	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	pDeviceContext->PSSetShaderResources(0, 16, nullSRV);

	return S_OK;
}


#ifdef _DEBUG

HRESULT CTarget_Manager::Ready_DebugBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pTargetTag, _uint iLTX, _uint iLTY, _uint iSizeX, _uint iSizeY)
{
	if (nullptr == m_pVIBuffer)
	{
		m_pVIBuffer = CVIBuffer_Rect::Create(pDevice, pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"));
		if (nullptr == m_pVIBuffer)
			return E_FAIL;
	}

	CRenderTarget* pRenderTarget = Find_RenderTarget(pTargetTag);
	if (nullptr == pRenderTarget)
		return E_FAIL;

	return pRenderTarget->Ready_DebugBuffer(iLTX, iLTY, iSizeX, iSizeY);
}
#endif // _DEBUG

HRESULT CTarget_Manager::Render_DebugBuffer(const _tchar* pMRTTag, _uint iPassIndex)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->Render_DebugBuffer(m_pVIBuffer, iPassIndex);

	return S_OK;
}

HRESULT CTarget_Manager::PostRender_DebugBuffer(const _tchar* pMRTTag, _uint iPassIndex, unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);
	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pRenderTarget : *pMRTList)
		pRenderTarget->PostRender_DebugBuffer(m_pVIBuffer, iPassIndex, m_spriteBatch, m_spriteFont);

	return S_OK;
}


CRenderTarget * CTarget_Manager::Find_RenderTarget(const _tchar * pRenderTargetTag)
{
	auto	iter = find_if(m_RenderTargets.begin(), m_RenderTargets.end(), CTagFinder(pRenderTargetTag));

	if(iter == m_RenderTargets.end())
		return nullptr;

	return iter->second;
}

list<CRenderTarget*>* CTarget_Manager::Find_MRT(const _tchar* pMRTTag)
{
	auto	iter = find_if(m_MRTs.begin(), m_MRTs.end(), CTagFinder(pMRTTag));

	if (iter == m_MRTs.end())
		return nullptr;

	return &iter->second;
}


void CTarget_Manager::Free()
{
	for (auto& Pair : m_MRTs)
	{
		for (auto& pRenderTarget : Pair.second)
		{
			Safe_Release(pRenderTarget);
		}
		Pair.second.clear();
	}

	m_MRTs.clear();



	for (auto& Pair : m_RenderTargets)
	{
		Safe_Release(Pair.second);
	}
	m_RenderTargets.clear();
	Safe_Release(m_pVIBuffer);
}
