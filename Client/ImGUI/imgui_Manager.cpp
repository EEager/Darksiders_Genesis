#include "stdafx.h"
#include "Base.h" 

#ifdef USE_IMGUI
#include "imgui_Manager.h"

// -------------------
// client headers
#include "GameInstance.h"
#include "Level_Loading.h"

bool CImguiManager::show_demo_window = true;
ImVec4 CImguiManager::clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void CImguiManager::Tick(_float fTimeDelta)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// Main Window 
	ShowMainControlWindow(fTimeDelta);

	// Demo Window
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
}

void CImguiManager::Render()
{
	// Rendering
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImguiManager::Initialize(ID3D11Device* pGraphic_Device, ID3D11DeviceContext* pDeviceContext)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pGraphic_Device, pDeviceContext);

	m_pGraphic_Device = pGraphic_Device;
	Safe_AddRef(pGraphic_Device);
	m_pDevice_Context = pDeviceContext;
	Safe_AddRef(pDeviceContext);


	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

}

void CImguiManager::Release()
{
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pDevice_Context);
	Safe_Release(m_pGameInstance);
}

void CImguiManager::ShowMainControlWindow(_float fDeltaTime)
{
	// =========================================
	// Default Stuffs
	static float f = 0.0f;
	ImGui::Begin("Main Control");
	ImGui::Text("Debugging Stuff: (hover over me~)");
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("F1:On/Off, ESC:CursorOn/CursorOff");

	ImGui::BulletText("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsMousePosValid())
		ImGui::BulletText("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
	else
		ImGui::BulletText("Mouse pos: <INVALID>");
	int count = IM_ARRAYSIZE(io.MouseDown);
	ImGui::BulletText("Mouse down:");
	for (int i = 0; i < count; i++) if (ImGui::IsMouseDown(i)) { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }

	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	// =========================================
	// Level Control 
	ImGui::Separator();
	ImGui::Text("Level Stuff:");
	int curLevel = m_pGameInstance->Get_CurrentLevel();
	ImGui::BulletText("Current Level : %s\n", curLevel < 0 ? "Error" : g_level_items[curLevel]);
	{
		static int m_iNextLevel = curLevel;
		if (ImGui::Combo("Level Select", &m_iNextLevel, g_level_items, IM_ARRAYSIZE(g_level_items)))
		{
			if (m_iNextLevel != LEVEL_STATIC && m_iNextLevel != LEVEL_LOADING && m_iNextLevel != LEVEL_END)
				if (FAILED(Open_Level(LEVEL(m_iNextLevel))))
					return;
		}
	}

	//// =========================================
	//// 카메라 빼고 다 멈추자
	//if (ImGui::Button("Play"))
	//	m_pGameInstance->StopAllTickWithOutCamera(false);
	//ImGui::SameLine();
	//if (ImGui::Button("Stop"))
	//	m_pGameInstance->StopAllTickWithOutCamera(true);


	//// ====================================
	//// GUI 컨트롤 
	//ImGui::Separator();
	//ImGui::Text("Show GUI Stuff:");
	//ImGui::Checkbox("Demo Control Window", &m_bShowDemoWindow);      // Edit  bools storing our window open/close state
	//ImGui::Checkbox("Camera Control Window", &m_bshow_camera_window);
	//ImGui::Checkbox("GameObject Manager Window", &m_bshow_gameobject_control_window);
	//ImGui::Checkbox("Editor Window", &m_bshow_editor_window);
	//ImGui::Checkbox("Floor Texture Window", &m_bshow_floor_window);

	ImGui::End();

}

HRESULT CImguiManager::Open_Level(LEVEL eStartID)
{
	CLevel* pNextLevel = CLevel_Loading::Create(m_pGraphic_Device, m_pDevice_Context, eStartID);
	if (nullptr == pNextLevel)
		return E_FAIL;

	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, pNextLevel)))
		return E_FAIL;

	return S_OK;
}

#endif // USE_IMGUI
