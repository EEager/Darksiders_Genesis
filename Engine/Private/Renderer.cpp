#include "..\public\Renderer.h"
#include "GameObject.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CRenderer::NativeConstruct(void * pArg)
{
	return S_OK;
}

/* 객체생성시에 추가한다(x) */
/* 매 프레임마다 그려져야할 객체를 판명하여 추가하낟. */
HRESULT CRenderer::Add_RenderGroup(RENDER eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;	
}

HRESULT CRenderer::Draw()
{
	if (FAILED(Render_Priority()))
		return E_FAIL;
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;
	if (FAILED(Render_Alpha()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;		
	

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{

	/* 리스트 순회 */
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

HRESULT CRenderer::Render_NonAlpha()
{




	/* 리스트 순회 */
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

HRESULT CRenderer::Render_Alpha()
{



	/* 카메라로부터 멀리 있는 객체부터 그린다. */
	m_RenderObjects[RENDER_ALPHA].sort([&](CGameObject* pSour, CGameObject* pDest) 
	{
		return pSour->Get_CamDistance() > pDest->Get_CamDistance();
	});


	/* 리스트 순회 */
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



	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	/* 리스트 순회 */
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

