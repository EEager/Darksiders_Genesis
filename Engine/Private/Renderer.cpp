#include "..\public\Renderer.h"
#include "GameObject.h"

#if 1 // JJLEE_TEST_RenderStates
class RenderStates
{
public:
	static ID3D11RasterizerState* WireframeRS;
	static ID3D11RasterizerState* NoCullRS;

	static ID3D11DepthStencilState* EqualsDSS;

	static ID3D11BlendState* AlphaToCoverageBS;
	static ID3D11BlendState* TransparentBS;
};


ID3D11RasterizerState* RenderStates::WireframeRS = 0;
ID3D11RasterizerState* RenderStates::NoCullRS = 0;

ID3D11DepthStencilState* RenderStates::EqualsDSS = 0;

ID3D11BlendState* RenderStates::AlphaToCoverageBS = 0;
ID3D11BlendState* RenderStates::TransparentBS = 0;

#endif

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{

}

HRESULT CRenderer::NativeConstruct_Prototype()
{

#if 1 // JJLEE_TEST_RenderStates
	//
	// TransparentBS
	//

	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pDevice->CreateBlendState(&transparentDesc, &RenderStates::TransparentBS);
#endif

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
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDeviceContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

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

	// Restore default blend state
	m_pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);



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
#if 1 // JJLEE_TEST_RenderStates
	//ID3D11RasterizerState* RenderStates::WireframeRS = 0;
	//ID3D11RasterizerState* RenderStates::NoCullRS = 0;

	//ID3D11DepthStencilState* RenderStates::EqualsDSS = 0;

	//ID3D11BlendState* RenderStates::AlphaToCoverageBS = 0;
	Safe_Release(RenderStates::TransparentBS);
#endif
	__super::Free();

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);

		ObjectList.clear();		
	}
}

