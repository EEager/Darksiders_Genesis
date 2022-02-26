#include "stdafx.h"
#include "..\Public\MainApp.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "Camera_Fly.h"

// ----------------------
//	ImGUI
// ----------------------
#if defined(USE_IMGUI) 
#include "imgui_Manager.h"
#endif


// ----------------------
// Client_Extern.h Variables
bool g_bUseNormalMap = true;
bool g_bUseEmissiveMap = true;
// ----------------------


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::NativeConstruct()
{
	CGraphic_Device::GRAPHICDEVDESC		GraphicDevDesc;
	GraphicDevDesc.eWinMode = CGraphic_Device::MODE_WIN;
	GraphicDevDesc.iWinCX = (_uint)g_iWinCX;
	GraphicDevDesc.iWinCY = (_uint)g_iWinCY;
	GraphicDevDesc.hWnd = g_hWnd;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDevDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_pDeviceContext);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(m_pDevice, L"../Bin/Resources/Font/DotumChe_16.spritefont");

#if defined(USE_IMGUI)
	CImguiManager::GetInstance()->Initialize(m_pDevice, m_pDeviceContext);
#endif

	if (FAILED(Ready_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_GameObject_Prototype()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;	

	return S_OK;
}

_int CMainApp::Tick(_float fTimeDelta)
{
#if defined(USE_IMGUI)
	_float dt = CImguiManager::GetInstance()->Get_SpeedFactor() * fTimeDelta;
	CImguiManager::GetInstance()->Tick(dt);
	m_pGameInstance->Tick_Engine(dt);
#else
	m_pGameInstance->Tick_Engine(fTimeDelta);
#endif


	return _int();
}

HRESULT CMainApp::Clear()
{
	assert(m_pGameInstance);

#if defined(USE_IMGUI)
	m_pGameInstance->Clear_BackBuffer_View(CImguiManager::GetInstance()->GetClearColor());
#else
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.4f, 0.4f, 0.4f, 1.f));
#endif
	m_pGameInstance->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CMainApp::Render()
{
	m_pRenderer->Draw(); // Main Draw

	m_pGameInstance->Render_Engine(); // Level Dummy

#if defined(USE_IMGUI)
	CImguiManager::GetInstance()->Render();
#endif
	return S_OK;
}

HRESULT CMainApp::PostRender()
{
	assert(m_pGameInstance);

	RECT rect = { 0,0,300,300 };
	m_spriteBatch->Begin();
#ifdef USE_IMGUI
	wstring str = DXString::Format(L"FPS : %.0f", ImGui::GetIO().Framerate);
#else
	wstring str = DXString::Format(L"Font Test");
#endif
	const wchar_t* output = str.c_str();
	
	//auto origin = m_spriteFont->MeasureString(output) / 2.f;
	auto origin = DirectX::g_XMZero;

	_float2 tmpPos;
	// Font Position
	tmpPos = _float2(g_iWinCX/2.f, g_iWinCY/2.f); 
	XMVECTOR m_fontPos = XMLoadFloat2(&tmpPos);

	// Outline Effect
	tmpPos = _float2(1.f, 1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	tmpPos = _float2(-1.f, 1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	tmpPos = _float2(-1.f, -1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);
	tmpPos = _float2(1.f, -1.f);
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos + XMLoadFloat2(&tmpPos), Colors::Black, 0.f, origin);

	// Origin Text
	m_spriteFont->DrawString(m_spriteBatch.get(), output,
		m_fontPos, Colors::White, 0.f, origin);
	
	m_spriteBatch->End();


	/*
		m_spriteBatch changes some render states...
		https://shawnhargreaves.com/blog/spritebatch-and-renderstates.html
		So, we need to restore render state...

		GraphicsDevice.RenderState.DepthBufferEnable = true;
		GraphicsDevice.RenderState.AlphaBlendEnable = false;
		GraphicsDevice.RenderState.AlphaTestEnable = false;
		(Depending on your 3D content)
	    GraphicsDevice.SamplerStates[0].AddressU = TextureAddressMode.Wrap;
		GraphicsDevice.SamplerStates[0].AddressV = TextureAddressMode.Wrap;
	*/
	m_pDeviceContext->RSSetState(0);
	m_pDeviceContext->OMSetDepthStencilState(0, 0);
	m_pDeviceContext->OMSetBlendState(0, 0, 0xffffffff);
	m_pDeviceContext->PSSetSamplers(0, 0, 0);

	return S_OK;
}

HRESULT CMainApp::Present()
{
	assert(m_pGameInstance);
	m_pGameInstance->Present();
	return S_OK;
}

#if defined(USE_IMGUI)
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

LRESULT CMainApp::messageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
#if defined(USE_IMGUI)
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
		return true;
#endif
	switch (msg)
	{
	case WM_COMMAND:
	{
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		// TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hwnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

HRESULT CMainApp::Open_Level(LEVEL eStartID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	CLevel*		pNextLevel = CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eStartID);
	if (nullptr == pNextLevel)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, pNextLevel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Component_ForStatic()
{
	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer);

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), CTransform::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */ 
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Rect.hlsl")))))
		return E_FAIL;
	
	/* For.Prototype_Component_Texture_Default */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Default"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Default%d.jpg"), 2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_GameObject_Prototype()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Fly"), CCamera_Fly::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMainApp::Free()
{
#if defined(USE_IMGUI)
	CImguiManager::GetInstance()->Release();
#endif


	Safe_Release(m_pRenderer);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}

