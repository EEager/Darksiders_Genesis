#include "..\public\Renderer.h"
#include "RendererStates.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Light_Manager.h"
#include "VIBuffer_Rect.h"
#include "PipeLine.h"
#include "Texture.h"




CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::NativeConstruct_Prototype()
{
	if (m_pTarget_Manager == nullptr)
		return E_FAIL;

#ifdef SHADOW_MAP_TEST
	mSmap = new ShadowMap(m_pDevice, (_uint)1600, (_uint)900);
#endif

	_uint iNumViewports = 1;
	D3D11_VIEWPORT ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof(D3D11_VIEWPORT));

	m_pDeviceContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	/* RT */
	// Just PreBackBuffer
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_PreBB"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// NonAlpha
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Diffuse"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Normal"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Depth_Cur"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Emissive"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_HitPower"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Depth_Prev"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// LightAcc
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shade"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Specular"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Shadown Map
	// ���� �ü����� �ٶ� ���̰����� ������ ����Ÿ��
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shadow_Env"), m_pDevice, m_pDeviceContext, SHADOWMAP_SIZE_X, SHADOWMAP_SIZE_Y, DXGI_FORMAT_R24G8_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f), CRenderTarget::RT_DEPTH_STENCIL)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Shadow_Obj"), m_pDevice, m_pDeviceContext, SHADOWMAP_SIZE_X, SHADOWMAP_SIZE_Y, DXGI_FORMAT_R24G8_TYPELESS, _float4(1.f, 1.f, 1.f, 1.f), CRenderTarget::RT_DEPTH_STENCIL)))
		return E_FAIL;

	// Distortion Map
	if (FAILED(m_pTarget_Manager->Add_RenderTarget(TEXT("Target_Distortion"), m_pDevice, m_pDeviceContext, ViewportDesc.Width, ViewportDesc.Height, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#ifdef _DEBUG
#define WIDTH 150
	/* Debug */
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_PreBB"), WIDTH * 0, WIDTH * 5, WIDTH, WIDTH)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Diffuse"), WIDTH * 0, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Normal"), WIDTH * 1, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Depth_Cur"), WIDTH * 2, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Depth_Prev"), WIDTH * 3, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Emissive"), WIDTH * 4, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_HitPower"), WIDTH * 5, WIDTH * 0, WIDTH, WIDTH)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Shade"), WIDTH * 0, WIDTH * 1, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Specular"), WIDTH * 1, WIDTH * 1, WIDTH, WIDTH)))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Shadow_Env"), WIDTH * 0, WIDTH * 2, WIDTH, WIDTH)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Shadow_Obj"), WIDTH * 1, WIDTH * 2, WIDTH, WIDTH)))
		return E_FAIL;

	// Distortion
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(m_pDevice, m_pDeviceContext, TEXT("Target_Distortion"), WIDTH * 0, WIDTH * 3, WIDTH, WIDTH)))
		return E_FAIL;
#endif // _DEBUG

	/* MRT */
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_PreBB"), TEXT("Target_PreBB"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Normal"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth_Cur"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_HitPower"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Deferred"), TEXT("Target_Depth_Prev"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
		return E_FAIL;

	// Shadow
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Shadows"), TEXT("Target_Shadow_Env"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Shadows"), TEXT("Target_Shadow_Obj"))))
		return E_FAIL;

	// Distortion
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Alpha"), TEXT("Target_PreBB"))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(TEXT("MRT_Alpha"), TEXT("Target_Distortion"))))
		return E_FAIL;

	/* Lighting */
	if (FAILED(m_pLight_Manager->NativeConstruct(m_pDevice, m_pDeviceContext)))
		return E_FAIL;

	/* Final Render(Render Blending) */
	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"));
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_TransformMatrix, XMMatrixIdentity());
	m_TransformMatrix._11 = ViewportDesc.Width;
	m_TransformMatrix._22 = ViewportDesc.Height;
	m_TransformMatrix._41 = 0.0f;
	m_TransformMatrix._42 = 0.0f;

	XMStoreFloat4x4(&m_OrthoMatrix, XMMatrixTranspose(XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f)));

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

#ifdef _DEBUG
bool bDraw_Debug = true;
#endif

HRESULT CRenderer::Draw()
{
	// ----------------------------------
	// �� Deferred Process Start
	if (FAILED(m_pTarget_Manager->Begin_MRT_PreBB(m_pDeviceContext, TEXT("MRT_PreBB"))))
		return E_FAIL;

	// SkyBox
	if (FAILED(Render_Priority()))
		return E_FAIL;
	
	//ShadowMap 
	if (FAILED(Render_Shadow()))
		return E_FAIL;

	if (FAILED(Render_NonAlpha()))
		return E_FAIL;
	// �������� ���⼭���� : shade�� spec�� ����.
	if (FAILED(Render_LightAcc()))
		return E_FAIL;
	// [Deffered End] ���� ����ۿ� ��ġ�� = NonAlpha + LightAcc
	if (FAILED(Render_Blend()))
		return E_FAIL;

	// ��ó�� �ʿ���� �ֵ��� ��� �ٷ� ����ۿ� �׸���
	if (FAILED(Render_NonLight()))
		return E_FAIL;

	// Render_Alpha // Distortion Test
	{
		// Begin_MRT_Post_Alpha : 0��°�� �����, 1��°�� distortionMap �̴�.
		if (FAILED(m_pTarget_Manager->Begin_MRT_Alpha(m_pDeviceContext, TEXT("MRT_Alpha"))))
			return E_FAIL;

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

		if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
			return E_FAIL;

		// Restore default states
		ClearRenderStates();
	}

	// PreBB�� ��Ƴ��� �͵��� ��¥ ����ۿ� �������
	if (FAILED(m_pTarget_Manager->End_MRT_PreBB(m_pDeviceContext)))
		return E_FAIL;

	// Render_Blend_Final
	{
		// Bind matrix
		m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
		m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));
		// Bind Texture
		m_pVIBuffer->Set_ShaderResourceView("g_PreBBTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_PreBB")));
		m_pVIBuffer->Set_ShaderResourceView("g_DistortionTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Distortion")));

		// Bind Delta Time
		//m_fDistortionTimeAcc = MathHelper::RandF(0.f, 0.0029f);

		_bool tmpTrue = true;
		_bool tmpFlase = false;

		m_pVIBuffer->Set_RawValue("g_swapDistortion", &tmpFlase, sizeof(_bool));

		if (tmpFlase)
		{
			_float heatHazeSpeed = .1f; 
			m_pVIBuffer->Set_RawValue("g_HeatHazeSpeed", &heatHazeSpeed, sizeof(_float));
			_float heatHazeStrength = 0.f;
			m_pVIBuffer->Set_RawValue("g_HeatHazeStrength", &heatHazeStrength, sizeof(_float));
		}
		else
		{
			//m_fDistortionTimeAcc = MathHelper::RandF(0.f, 0.0029f);
			m_fDistortionTimeAcc += 0.016f;
			if (m_fDistortionTimeAcc >= 1000.f) 
			{
				m_fDistortionTimeAcc = 0.f;
			}
			m_pVIBuffer->Set_RawValue("g_fTimeDelta", &m_fDistortionTimeAcc, sizeof(_float));

			_float heatHazeSpeed = 5.f;
			m_pVIBuffer->Set_RawValue("g_HeatHazeSpeed", &heatHazeSpeed, sizeof(_float));
			_float heatHazeStrength = 0.0026f; 
			m_pVIBuffer->Set_RawValue("g_HeatHazeStrength", &heatHazeStrength, sizeof(_float));
		}

		// �縷�� �������̸� ǥ������
		m_pVIBuffer->Set_RawValue("g_DesertDistortion", &tmpFlase, sizeof(_bool));
		if (m_pTextureNoise)
			m_pTextureNoise->SetUp_OnShader(m_pVIBuffer, "g_NoiseTexture_HeatHaze", 2);
		m_pVIBuffer->Render(5); // RenderPreBBToBB_Pass
	}

	// �� Deferred Process End
	// ----------------------------------

	// ----------------------------------
	// �� Foward Process

	// UI�� �ٷ� ����ۿ� �׸���
	if (FAILED(Render_UI())) 
		return E_FAIL;		
	if (FAILED(Render_Mouse()))
		return E_FAIL;
	
	return S_OK;
}

// ������Ʈ���� ���� ����� Text�� Debugging�� Line�� ���⼭ �������
HRESULT CRenderer::PostDraw(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	//ClearRenderStates();

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

#ifdef _DEBUG
	if (CInput_Device::GetInstance()->Key_Down(DIK_F3))
	{
		bDraw_Debug = !bDraw_Debug;
	}

	if (bDraw_Debug)
	{
		// ����Ÿ�ٵ� ������
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_PreBB"), 0);
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Deferred"), 0);
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_LightAcc"), 0);
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Shadows"), 4); // ShadowMapDebug_Pass
		m_pTarget_Manager->Render_DebugBuffer(TEXT("MRT_Alpha"), 0); 


		// �ؽ�Ʈ ������
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_PreBB"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_Deferred"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_LightAcc"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_Shadows"), 0, m_spriteBatch, m_spriteFont);
		m_pTarget_Manager->PostRender_DebugBuffer(TEXT("MRT_Alpha"), 0, m_spriteBatch, m_spriteFont);
	}
#endif // _DEBUG

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

// SkyBox
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

	// Restore default states
	ClearRenderStates();

	return S_OK;
}

// Shadow
HRESULT CRenderer::Render_Shadow()
{
	// #1. ���� �׸��� ���̸� ����
	if (FAILED(m_pTarget_Manager->BindDsvAndSetNullRenderTarget(m_pDeviceContext, TEXT("Target_Shadow_Env"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_TERRAIN])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(3))) // BuildShadowMap_Pass
				return E_FAIL;
		}
	}

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	// #2. ������Ʈ���� �׸��ڸ� ��´�.
	if (FAILED(m_pTarget_Manager->BindDsvAndSetNullRenderTarget(m_pDeviceContext, TEXT("Target_Shadow_Obj"))))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_WAR])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(3))) // BuildShadowMap_Pass
				return E_FAIL;
		}
	}

	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(3))) // BuildShadowMap_Pass
				return E_FAIL;
		}
	}

	// Restore default states
	ClearRenderStates();
	m_pDeviceContext->RSSetViewports(1, CGraphic_Device::GetInstance()->Get_ViewPortDesc_Ptr());

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NonAlpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_Deferred"))))
		return E_FAIL;

	// War�� ��� RENDER_NONALPHA_WAR �׷쿡 ���� MRT_Deferred�� Depth_War�� �߰��� ����Ѵ�.
	// �̴� Render_Blend���� War�� ���������� �ܰ��� ���⿡ ���ȴ�.
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA_WAR])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(1))) // onlyWarPass
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}
	m_RenderObjects[RENDER_NONALPHA_WAR].clear();
	// Restore default states
	ClearRenderStates();

	// ������´�.
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
	// Restore default states
	ClearRenderStates();
	
	// ������ War�� ���� ��� �� ���� �ٸ� ��ü���� �ﵵ�� ����.
	for (auto& pGameObject : m_RenderObjects[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render(0))) // deferredPass
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONALPHA].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	// Restore default states
	ClearRenderStates();


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

	// Restore default states
	ClearRenderStates();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& pGameObject : m_RenderObjects[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render()))
				return E_FAIL;

			Safe_Release(pGameObject);
		}
	}

	m_RenderObjects[RENDER_NONLIGHT].clear();

	// Restore default states
	ClearRenderStates();

	return S_OK;
}


HRESULT CRenderer::Render_Alpha()
{
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
	
	// Restore default states
	ClearRenderStates();


	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
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

HRESULT CRenderer::Render_Mouse()
{
	/* ����Ʈ ��ȸ */
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

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTarget_Manager ||
		nullptr == m_pLight_Manager)
		return E_FAIL;

	/* ��ġ���� Target_Shade�� ���õȴ�. */
	if (FAILED(m_pTarget_Manager->Begin_MRT(m_pDeviceContext, TEXT("MRT_LightAcc"))))
		return E_FAIL;

	/* Target_Shade�� �׸���. */
	m_pLight_Manager->Render();

	if (FAILED(m_pTarget_Manager->End_MRT(m_pDeviceContext)))
		return E_FAIL;

	return S_OK;
}


HRESULT CRenderer::Render_Blend()
{
	m_pVIBuffer->Set_RawValue("g_TransformMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_TransformMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(XMLoadFloat4x4(&m_OrthoMatrix)), sizeof(_float4x4));

	m_pVIBuffer->Set_ShaderResourceView("g_DiffuseTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Diffuse")));

	// �Ʒ� 2���� Lighting���� ����ִ°�. 
	m_pVIBuffer->Set_ShaderResourceView("g_ShadeTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Shade")));
	m_pVIBuffer->Set_ShaderResourceView("g_SpecularTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Specular")));


	// �Ȱ� ȿ���� ���� �ȼ��� world ��ġ�� �˾ƿ���
	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);
	_matrix		ViewMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_VIEW));
	_matrix		ProjMatrixInverse = XMMatrixInverse(nullptr, pPipeLine->Get_Transform(CPipeLine::TS_PROJ));
	m_pVIBuffer->Set_RawValue("g_ViewMatrixInverse", &XMMatrixTranspose(ViewMatrixInverse), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrixInverse", &XMMatrixTranspose(ProjMatrixInverse), sizeof(_float4x4));
	RELEASE_INSTANCE(CPipeLine);

	// Target_Depth_Cur
	m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth_Cur")));

	// Target_Depth_Prev
	m_pVIBuffer->Set_ShaderResourceView("g_DepthTexture_War", m_pTarget_Manager->Get_SRV(TEXT("Target_Depth_Prev")));

	// EmissiveTexture
	m_pVIBuffer->Set_ShaderResourceView("g_EmissiveTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_Emissive")));

	// HitPowerTexture
	m_pVIBuffer->Set_ShaderResourceView("g_HitPowerTexture", m_pTarget_Manager->Get_SRV(TEXT("Target_HitPower")));

	m_pVIBuffer->Render(3); // final pass�� �������

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
	this->AddRef();

	return this;
}

void CRenderer::Free()
{
#ifdef SHADOW_MAP_TEST
	SafeDelete(mSmap);
#endif

	__super::Free();

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
			Safe_Release(pGameObject);

		ObjectList.clear();		
	}

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);
	
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pTextureNoise);
}

