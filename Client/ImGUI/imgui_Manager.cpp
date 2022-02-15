#include "stdafx.h"
#include "Base.h" 

#ifdef USE_IMGUI
#include "imgui_Manager.h"

// -------------------
// client headers
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Fly.h"

bool CImguiManager::m_bShow_demo_window = true;
bool CImguiManager::m_bshow_camera_window = false;
bool CImguiManager::m_bshow_gameobject_control_window = false;
bool CImguiManager::m_bshow_editor_window = false;

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
	if (m_bShow_demo_window)
		ImGui::ShowDemoWindow(&m_bShow_demo_window);

	// Camera Window
	ShowCameraControlWindow();
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
	m_iNextLevel = curLevel;
	ImGui::BulletText("Current Level : %s\n", curLevel < 0 ? "Error" : g_level_items[curLevel]);
	if (ImGui::Combo("Level Select", &m_iNextLevel, g_level_items, IM_ARRAYSIZE(g_level_items)))
	{
		if (m_iNextLevel != LEVEL_STATIC && m_iNextLevel != LEVEL_LOADING && m_iNextLevel != LEVEL_END)
		{
			if (FAILED(Open_Level(LEVEL(m_iNextLevel))))
				return;
		}
		else
			m_iNextLevel = curLevel;
	}

	// =========================================
	// 카메라 빼고 다 멈추자
	if (ImGui::Button("Play"))
		;// m_pGameInstance->StopAllTickWithOutCamera(false);
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
		; //m_pGameInstance->StopAllTickWithOutCamera(true);


	// ====================================
	// GUI 컨트롤 
	ImGui::Separator();
	ImGui::Text("Show GUI Stuff:");
	ImGui::Checkbox("Demo Control Window", &m_bShow_demo_window);      // Edit  bools storing our window open/close state
	ImGui::Checkbox("Camera Control Window", &m_bshow_camera_window);
	ImGui::Checkbox("GameObject Manager Window", &m_bshow_gameobject_control_window);
	ImGui::Checkbox("Editor Window", &m_bshow_editor_window);

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

void CImguiManager::ShowCameraControlWindow()
{
	if (!m_bshow_camera_window)
		return;

	ImGui::Begin("Camera Control", &m_bshow_camera_window);
	auto pGameList = m_pGameInstance->Get_GameObject_CloneList(TEXT("Layer_Camera"));

	if (!pGameList || pGameList->empty())
	{
		ImGui::Text("No Camera");
		ImGui::End();
		return;
	}

	CCamera_Fly* pCamera = (CCamera_Fly*)pGameList->front();
	auto pCamera_Transform = (CTransform*)pCamera->Get_Camera_Transform();

	bool rotDirty_pitch = false;
	bool rotDirty_yaw = false;
	bool posDirty = false;

	const auto dCheck = [](bool d, bool& carry) { carry = carry || d; }; // 람다 한번 사용해보자

	float f0 = pCamera->Get_Camera_Desc().TransformDesc.fSpeedPerSec;
	bool speedDirty = false;

	//ImGui::InputFloat(const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
	dCheck(ImGui::InputFloat("Camera Move Speed", &f0, 1.f), speedDirty);
	if (speedDirty)
		pCamera->Set_Camera_Speed(f0);

	ImGui::Text("Position");
	auto vStatePos = pCamera_Transform->Get_State_Flt(CTransform::STATE_POSITION);
	dCheck(ImGui::SliderFloat("X", &vStatePos.x, -180.0f, 180.0f, "%.1f"), posDirty);
	dCheck(ImGui::SliderFloat("Y", &vStatePos.y, -180.0f, 180.0f, "%.1f"), posDirty);
	dCheck(ImGui::SliderFloat("Z", &vStatePos.z, -180.0f, 180.0f, "%.1f"), posDirty);

	ImGui::Text("Orientation");

	_float fPitch = 0;
	_float fPitch_Prev = fPitch;
	_float fYaw = 0;
	_float fYaw_Prev = fYaw;

	//// fPitch
	//_float3 tmp, tmp2;
	//_float3 vLook = pCamera_Transform->Get_State(CTransform::STATE_LOOK);
	//D3DXVec3Normalize(&tmp, &vLook);
	//D3DXVec3Normalize(&tmp2, &_float3(vLook.x, 0, vLook.z));
	//fPitch = fPitch_Prev = acosf(D3DXVec3Dot(&tmp, &tmp2)) * (tmp.y >= 0 ? -1 : 1); // look과 zx에 투영된 look 간의 각도
	//dCheck(ImGui::SliderAngle("Pitch", &fPitch, 0.995f * -90.0f, 0.995f * 90.0f), rotDirty_pitch); // 위아래, 0.995f 없으면 에러뜸

	//																							   // fYaw
	//D3DXVec3Normalize(&tmp, &vLook);
	//D3DXVec3Normalize(&tmp2, &_float3(vLook.x, 0, vLook.z));
	//fYaw = fYaw_Prev = acosf(D3DXVec3Dot(&_float3(0, 0, 1.f), &tmp2)) * (tmp.x < 0 ? -1 : 1); // z랑 zx에 투영된선 간의 각도
	//dCheck(ImGui::SliderAngle("Yaw", &fYaw, -180.0f, 180.0f), rotDirty_yaw); // 좌우

	//if (ImGui::Button("Reset"))
	//{
	//	vStatePos = _float3(0.f, 10.f, -15.0f);
	//	fPitch = 0, fYaw = 0;
	//	posDirty = rotDirty_pitch = rotDirty_pitch = true;
	//}

	//if (rotDirty_pitch)
	//{
	//	pCamera_Transform->Rotation_Axis(pCamera_Transform->Get_State(CTransform::STATE_RIGHT), (fPitch - fPitch_Prev == 0) ? 0.001f : fPitch - fPitch_Prev);
	//	fPitch_Prev = fPitch;
	//}

	//if (rotDirty_yaw)
	//{
	//	pCamera_Transform->Rotation_Axis(_float3(0.f, 1.f, 0.f), fYaw - fYaw_Prev);
	//	fYaw_Prev = fYaw;
	//}

	//if (posDirty)
	//{
	//	pCamera_Transform->Set_State(CTransform::STATE_POSITION, vStatePos);
	//}

	//ImGui::Text("Camera Mode");
	//{
	//	static int camera_item_current = 0;
	//	static const char* camera_items[] = { "Free", "TooDee View", "TopDee View" };
	//	if (ImGui::Combo("Mode", &camera_item_current, camera_items, IM_ARRAYSIZE(camera_items)))
	//	{
	//		if (camera_item_current == 0) // "Free" 
	//		{
	//			pCamera->Set_Pos_Lerp(false);
	//			pCamera->Set_TooDeeView(false);
	//		}
	//		else if (camera_item_current == 1) // "TooDee View"
	//		{
	//			pCamera->Set_TooDeeView(true);
	//			pCamera->Set_Pos_Lerp(true, { 14.4f, 15.2f, 7.6f }, m_fTimeDelta * 0.9f);
	//			pCamera->Set_Rotation_Lerp(true, { 14.4f, 0.f, 7.6f }, m_fTimeDelta * 0.9f);
	//		}
	//		else // "TopDee View" 
	//		{
	//			pCamera->Set_TooDeeView(false);
	//			pCamera->Set_Pos_Lerp(true, { 14.4f, 12.5f, -1.9f }, m_fTimeDelta * 0.9f);
	//			pCamera->Set_Rotation_Lerp(true, { 14.4f, 0.f, 7.6f }, m_fTimeDelta * 0.9f);
	//		}

	//	}
	//}


	//ImGui::Text("Load And Save");
	//if (ImGui::Button("Save Camera Data"))
	//{
	//	wstring savePath = SaveFilePath();
	//	if (savePath != L"")
	//	{
	//		HANDLE hFile = ::CreateFile(savePath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	//		if (INVALID_HANDLE_VALUE == hFile)
	//		{
	//			MessageBox(NULL, savePath.c_str(), L"Save Camera Data 파일 오픈 실패", MB_OK);
	//		}

	//		DWORD dwBytes = 0;
	//		_tchar CameraSaveTest[MAX_TAG_LEN] = L"Camera Save Test";
	//		::WriteFile(hFile, &CameraSaveTest, sizeof(CameraSaveTest), &dwBytes, nullptr);
	//		::CloseHandle(hFile);
	//	}
	//}
	//ImGui::SameLine();
	//if (ImGui::Button("Load Camera Data"))
	//{
	//	wstring loadPath = LoadFilePath();
	//	if (loadPath != L"")
	//	{
	//		HANDLE hFile = ::CreateFile(loadPath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	//		if (INVALID_HANDLE_VALUE == hFile) // 저장된거 없다.
	//		{
	//			MessageBox(NULL, loadPath.c_str(), L"Load Camera Data 할게 없다.(정상)", MB_OK);
	//		}
	//		// 저장된거 있으면 불러오자
	//		DWORD dwBytes = 0;
	//		_tchar CameraLoadTest[MAX_TAG_LEN] = L"";
	//		::ReadFile(hFile, &CameraLoadTest, sizeof(CameraLoadTest), &dwBytes, nullptr);
	//		MessageBox(NULL, CameraLoadTest, L"Load Camera 성공", MB_OK);
	//		::CloseHandle(hFile);
	//	}
	//}


	ImGui::End(); // "Camera Control"
}

#endif // USE_IMGUI
