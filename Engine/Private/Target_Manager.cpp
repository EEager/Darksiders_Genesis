#include "..\Public\Target_Manager.h"
#include "RenderTarget.h"

IMPLEMENT_SINGLETON(CTarget_Manager)

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
	/* 지정한 mrt에 있는 렌더타겟들을 장치에 바인드한다. */
	list<CRenderTarget*>* pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	/* 장치에 보관되어있던 백버퍼, 깊이스텐실버퍼를 얻어와서 멤버변수에 보관한다.  */
	/* 왜? 새로운 타겟들을 장치에 바인드하면 백버퍼를 잃어버리니까. */
	pDeviceContext->OMGetRenderTargets(1, &m_pBackBufferView, &m_pDepthStencilView);

	ID3D11RenderTargetView* pRenderTargets[8] = { nullptr };

	_uint		iIndex = 0;

	for (auto& pRenderTarget : *pMRTList)
	{
		pRenderTarget->Clear();
		pRenderTargets[iIndex++] = pRenderTarget->Get_RTV();
	}

	// 장치에 여러개의 렌터타겟들을 바인드한다
	pDeviceContext->OMSetRenderTargets(pMRTList->size(), pRenderTargets, m_pDepthStencilView);

	return S_OK;
}

HRESULT CTarget_Manager::End_MRT(ID3D11DeviceContext* pDeviceContext)
{
	if (nullptr == m_pBackBufferView ||
		nullptr == m_pDepthStencilView)
		return E_FAIL;

	pDeviceContext->OMSetRenderTargets(1, &m_pBackBufferView, m_pDepthStencilView); // 장치에 백버퍼를 바인드하여 원복한다

	Safe_Release(m_pBackBufferView);
	Safe_Release(m_pDepthStencilView);


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
}
