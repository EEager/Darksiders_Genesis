#include "stdafx.h"
#include "..\Public\MainApp.h"


// ---------------------------
// GameObject Headers
// ---------------------------
#include "Level_Loading.h"
#include "Logo_BackGround.h"
#include "Loading_BackGround.h"
#include "Camera_Fly.h"
#include "Mouse.h"

// Effect
#include "SceneChangeEffect.h"

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
bool g_bUseRoughnessMap = false;
bool g_bUseMetalicMap = false;
// ----------------------


CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::NativeConstruct()
{
#ifndef _DEBUG // Release
	while (::ShowCursor(false) >= 0); // HIDE cursor
#endif

	CGraphic_Device::GRAPHICDEVDESC		GraphicDevDesc;
	GraphicDevDesc.eWinMode = CGraphic_Device::MODE_WIN;
	GraphicDevDesc.iWinCX = (_uint)g_iWinCX;
	GraphicDevDesc.iWinCY = (_uint)g_iWinCY;
	GraphicDevDesc.hWnd = g_hWnd;

	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, GraphicDevDesc, &m_pDevice, &m_pDeviceContext)))
		return E_FAIL;

	m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(m_pDeviceContext);
	m_spriteFont = std::make_unique<DirectX::SpriteFont>(m_pDevice, L"../Bin/Resources/Font/Requiem_18.spritefont");


#if defined(USE_IMGUI)
	CImguiManager::GetInstance()->Initialize(m_pDevice, m_pDeviceContext);
#endif

	if (FAILED(Ready_Component_ForStatic()))
		return E_FAIL;

	if (FAILED(Ready_GameObject_Prototype()))
		return E_FAIL;

	//if (FAILED(Ready_Gara()))
	//	return E_FAIL;

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
	m_pGameInstance->Clear_BackBuffer_View(_float4(0.f, 0.f, 0.f, 1.f));
#endif
	m_pGameInstance->Clear_DepthStencil_View();

	return S_OK;
}

HRESULT CMainApp::Render()
{
	m_pRenderer->Draw(); // Main Draw

	m_pGameInstance->Render_Engine(); // Level Dummy


	return S_OK;
}

HRESULT CMainApp::PostRender()
{
	assert(m_pGameInstance);

	RECT rect = { 0,0,300,300 };
	m_spriteBatch->Begin();

	m_pRenderer->PostDraw(m_spriteBatch, m_spriteFont); // Post Draw

	m_pGameInstance->PostRender_Engine(m_spriteBatch, m_spriteFont); // Level Post Dummy
	m_spriteBatch->End();


	// 암튼 다시 디폴트 설정해야한다는 뜻 ㅎ
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

#if defined(USE_IMGUI)
	CImguiManager::GetInstance()->Render();
#endif

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
		// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
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

	CLevel* pNextLevel = CLevel_Loading::Create(m_pDevice, m_pDeviceContext, eStartID);
	if (nullptr == pNextLevel)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, pNextLevel)))
		return E_FAIL;

	return S_OK;
}


// STATIC Component Prototype 
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

	/* For.Prototype_Component_StateMachine */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), CStateMachine::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	// ======================================================================
	// Textures

	/* For.Prototype_Component_Texture_Black */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Black"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Default/Black_%d.tga"), 2))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Logo */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Logo/DSG_Loading_Title.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Loading */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_BackGround"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Loading/DSG_Loading_Emblem.dds")))))
		return E_FAIL;
	/* For.Prototype_Component_Texture_Loading_Circle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading_Circle"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Loading/UI_RuneRingOuter.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Mouse */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mouse"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Mouse/UI_Map_FocusCursor.dds")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Dissolve */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/VFX/T_DissolveMask_A.tga")))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_MenuTransSlice1 */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MenuTransSlice1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/UI_MenuTransSlice1.tga")))))
		return E_FAIL;

	// ======================================================================
	// Collider
	/* For.Prototype_Component_Colllider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider"),
		CCollider::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;



	return S_OK;
}

// STATIC GameObject Prototype 
HRESULT CMainApp::Ready_GameObject_Prototype()
{
	// for.Prototype_GameObject_Logo_BackGround
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Logo_BackGround"), CLogo_BackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	// for.Prototype_GameObject_Loading_BackGround
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Loading_BackGround"), CLoading_BackGround::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	// for.Prototype_GameObject_Camera_Fly
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Fly"), CCamera_Fly::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	// for.Prototype_GameObject_Mouse
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mouse"), CMouse::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, L"Layer_Mouse", TEXT("Prototype_GameObject_Mouse"))))
		return E_FAIL;

	/* For.Prototype_GameObject_SceneChangeEffect1 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SceneChangeEffect1"),
		CSceneChangeEffect1::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SceneChangeEffect2 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SceneChangeEffect2"),
		CSceneChangeEffect2::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_SceneChangeEffect3 */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_SceneChangeEffect3"),
		CSceneChangeEffect3::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/NavigationData.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;


	_float3		vPoint[3];

	ZeroMemory(vPoint, sizeof(_float3) * 3);
	vPoint[0] = _float3(0.f, 0.f, 5.f);
	vPoint[1] = _float3(5.f, 0.f, 0.f);
	vPoint[2] = _float3(0.0f, 0.f, 0.f);
	WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoint, sizeof(_float3) * 3);
	vPoint[0] = _float3(0.f, 0.f, 5.f);
	vPoint[1] = _float3(5.f, 0.f, 5.0f);
	vPoint[2] = _float3(5.0f, 0.f, 0.f);
	WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoint, sizeof(_float3) * 3);
	vPoint[0] = _float3(0.f, 0.f, 10.f);
	vPoint[1] = _float3(5.f, 0.f, 5.0f);
	vPoint[2] = _float3(0.0f, 0.f, 5.0f);
	WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);

	ZeroMemory(vPoint, sizeof(_float3) * 3);
	vPoint[0] = _float3(5.0f, 0.f, 5.f);
	vPoint[1] = _float3(10.f, 0.f, 0.f);
	vPoint[2] = _float3(5.0f, 0.f, 0.f);
	WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);

	CloseHandle(hFile);


	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pInstance = new CMainApp();

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

