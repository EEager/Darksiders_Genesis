#include "..\public\Renderer.h"
#include "RendererStates.h"
#include "GameObject.h"
#include "Target_Manager.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
{
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (m_pTarget_Manager == nullptr)
		return E_FAIL;

	_uint iNumViewports = 1;
	D3D11_VIEWPORT ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Diffuse"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Normal"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shade"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;


	if (FAILED(RenderStates::CreateRenderStates(m_pDevice)))
		return E_FAIL;

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
	if (FAILED(Render_Mouse()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CRenderer::PostDraw(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	/* 리스트 순회 */
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
	// Disable z-Buffer
	m_pDeviceContext->OMSetDepthStencilState(RenderStates::noDepthState.Get(), 0);

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
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

	// Restore default blend state
	ClearRenderStates();


	return S_OK;
}

HRESULT CRenderer::Render_Priority_Terrain()
{

	/* 리스트 순회 */
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

	// Restore default blend state
	ClearRenderStates();


	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

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

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	// Restore default blend state
	ClearRenderStates();

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha_War()
{

	/* 리스트 순회 */
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

	// Restore default blend state
	ClearRenderStates();

	return S_OK;
}

HRESULT CRenderer::Render_Alpha()
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDeviceContext->OMSetBlendState(RenderStates::TransparentBS.Get(), blendFactor, 0xffffffff);

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
	ClearRenderStates();



	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	// Disable z-Buffer
	m_pDeviceContext->OMSetDepthStencilState(RenderStates::noDepthState.Get(), 0);
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	// Blending
	m_pDeviceContext->OMSetBlendState(RenderStates::TransparentBS.Get(), blendFactor, 0xffffffff);

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

	// Restore default states
	ClearRenderStates();



	return S_OK;
}

HRESULT CRenderer::Render_Mouse()
{
	// Disable z-Buffer
	m_pDeviceContext->OMSetDepthStencilState(RenderStates::noDepthState.Get(), 0);
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_pDeviceContext->OMSetBlendState(RenderStates::TransparentBS.Get(), blendFactor, 0xffffffff);

	/* 리스트 순회 */
	for (auto& pGameObject : m_RenderObjects[RENDER_MOUSE])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_MOUSE].clear();

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

	Safe_Release(m_pTarget_Manager);
}

