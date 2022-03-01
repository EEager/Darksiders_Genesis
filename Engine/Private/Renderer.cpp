#include "..\public\Renderer.h"
#include "RendererStates.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (FAILED(RenderStates::CreateRenderStates(m_pDevice)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void * pArg)
{
	return S_OK;
}

/* ��ü�����ÿ� �߰��Ѵ�(x) */
/* �� �����Ӹ��� �׷������� ��ü�� �Ǹ��Ͽ� �߰��ϳ�. */
HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;	
}

HRESULT CRenderer::Add_PostRenderGroup(CGameObject* pGameObject)
{
	m_PostRenderObjects.push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_Priority_Terrain()))
		return E_FAIL;
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;
	if (FAILED(Render_NonAlpha_War()))
		return E_FAIL;
	if (FAILED(Render_Alpha()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;		
	

	return S_OK;
}

HRESULT CRenderer::PostDraw(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	/* ����Ʈ ��ȸ */
	for (auto& pGameObject : m_PostRenderObjects)
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->PostRender(m_spriteBatch, m_spriteFont)))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_PostRenderObjects.clear();

	return S_OK;
}

HRESULT CRenderer::ClearRenderStates()
{
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);
	m_pDeviceContext->OMSetBlendState(0, 0, 0xffffffff);
	m_pDeviceContext->PSSetSamplers(0, 0, 0);

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{

	/* ����Ʈ ��ȸ */
	for (auto& pGameObject : m_RenderObjects[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_PRIORITY].clear();


	return S_OK;
}

HRESULT CRenderer::Render_Priority_Terrain()
{

	/* ����Ʈ ��ȸ */
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_TERRAIN])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONALPHA_TERRAIN].clear();


	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{

	/* ����Ʈ ��ȸ */
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONALPHA].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha_War()
{

	/* ����Ʈ ��ȸ */
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_WAR])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONALPHA_WAR].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Alpha()
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDeviceContext->OMSetBlendState(RenderStates::TransparentBS.Get(), blendFactor, 0xffffffff);

	/* ī�޶�κ��� �ָ� �ִ� ��ü���� �׸���. */
	m_RenderObjects[RENDER_ALPHA].sort([&](CGameObject* pSour, CGameObject* pDest) 
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});


	/* ����Ʈ ��ȸ */
	for (auto& pGameObject : m_RenderObjects[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_ALPHA].clear();

	// Restore default blend state
	ClearRenderStates();
	//m_pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);



	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	// Disable z-Buffer
	m_pDeviceContext->OMSetDepthStencilState(RenderStates::noDepthState.Get(), 0);
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDeviceContext->OMSetBlendState(RenderStates::TransparentBS.Get(), blendFactor, 0xffffffff);

	/* ����Ʈ ��ȸ */
	for (auto& pGameObject : m_RenderObjects[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_UI].clear();

	// Restore default states
	ClearRenderStates();



	return S_OK;
}

CRenderer * CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CRenderer*		pInstance = new CRenderer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Created CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent * CRenderer::Clone(void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);

		ObjectList.clear();		
	}
}

