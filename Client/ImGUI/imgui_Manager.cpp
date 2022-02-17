#include "stdafx.h"
#include "Base.h" 

#ifdef USE_IMGUI
#include "imgui_Manager.h"

// -------------------
// client headers
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Fly.h"

#include <shobjidl.h>  // Ifileopendialog 관련 cominterface 들어있는거
#include <Shlwapi.h> // 초기 경로 얻어올때 사용하자

bool CImguiManager::m_bShow_demo_window = false;
bool CImguiManager::m_bshow_camera_window = true;
bool CImguiManager::m_bShow_Simulation_Speed = false;
bool CImguiManager::m_bshow_gameobject_control_window = false;
bool CImguiManager::m_bshow_editor_window = false;

ImVec4 CImguiManager::clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

void EnableImGuiMouse()
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void DisableImGuiMouse()
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void HideCursor()
{
	while (::ShowCursor(false) >= 0);
}

void ShowCursor()
{
	while (::ShowCursor(true) < 0);
}

void ConfineCursor() // 마우스 화면안에 가두자 
{
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	MapWindowPoints(g_hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void FreeCursor() // 마우스 화면안에 가두지 말자
{
	ClipCursor(nullptr);
}

void CImguiManager::EnableCursor()
{
	m_bCursorEnable = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}

void CImguiManager::DisableCursor()
{
	m_bCursorEnable = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}

wstring CImguiManager::SaveFilePath()
{
	//string path = "";
	wstring retPath = L"";
	HRESULT hr = CoInitializeEx(NULL, COINITBASE_MULTITHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileSaveDialog* pFileSave;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL,
			IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));
		if (SUCCEEDED(hr))
		{
			// Set default extension
			hr = pFileSave->SetDefaultExtension(L"txt");
			if (SUCCEEDED(hr))
			{
				// Show the Open dialog box.
				hr = pFileSave->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr))
				{
					IShellItem* pItem;
					hr = pFileSave->GetResult(&pItem);
					if (SUCCEEDED(hr))
					{
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						// Display the file name to the user.
						if (SUCCEEDED(hr))
						{
							//MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
					/*		char buffer[500];
							wcstombs(buffer, pszFilePath, 500);
							path = buffer;*/
							retPath = pszFilePath; // 저장할 파일 경로
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				pFileSave->Release();
			}
		}
		CoUninitialize();
	}

	return retPath;
}

wstring CImguiManager::LoadFilePath()
{
	wstring retPath = L"";
	HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
		COINIT_DISABLE_OLE1DDE);
	if (SUCCEEDED(hr))
	{
		IFileOpenDialog* pFileOpen;
		//IFileSaveDialog *pFileOpen;

		// Create the FileOpenDialog object.
		hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
			IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

		if (SUCCEEDED(hr))
		{
			// Show the Open dialog box.
			hr = pFileOpen->Show(NULL);

			// Get the file name from the dialog box.
			if (SUCCEEDED(hr))
			{
				IShellItem* pItem;
				hr = pFileOpen->GetResult(&pItem);
				if (SUCCEEDED(hr))
				{
					PWSTR pszFilePath;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

					// Display the file name to the user.
					if (SUCCEEDED(hr))
					{
						//MessageBoxW(NULL, pszFilePath, L"File Path", MB_OK);
						retPath = pszFilePath;
						CoTaskMemFree(pszFilePath);
					}
					pItem->Release();
				}
			}
			pFileOpen->Release();
		}
		CoUninitialize();
	}

	return retPath;
}

void CImguiManager::Tick(_float fTimeDelta)
{
	// Check Key First
	ImGUI_Key(fTimeDelta);

	if (!m_bImGUIEnable)
		return;

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

	// Speed Factor
	ShowSpeedFactorControlWindow();
}

void CImguiManager::Render()
{
	if (!m_bImGUIEnable)
		return;
	
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
		ImGui::SetTooltip("F2:On/Off, ESC:CursorOn/CursorOff");

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


	// ====================================
	// GUI 컨트롤 
	ImGui::Separator();
	ImGui::Text("Show GUI Stuff:");
	ImGui::Checkbox("Demo Control Window", &m_bShow_demo_window);      // Edit  bools storing our window open/close state
	ImGui::Checkbox("Camera Control Window", &m_bshow_camera_window);
	ImGui::Checkbox("Simulation Speed Factor", &m_bShow_Simulation_Speed);
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

	dCheck(ImGui::InputFloat("Speed", &f0, 1.f), speedDirty);
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

	// --------------------------------------------
	// 1. fPitch : 상하 각도. 
	_vector vLook = pCamera_Transform->Get_State(CTransform::STATE_LOOK);
	vLook = XMVector3Normalize(vLook);
	_vector vLookXZ = XMVector3Normalize(XMVectorSet(XMVectorGetX(vLook), 0.f, XMVectorGetZ(vLook), 0.f));

	XMVECTOR angleVecPitch = XMVector3AngleBetweenVectors(vLookXZ, vLook) * (XMVectorGetY(vLook) >= 0 ? -1.f : 1.f); // vLook.y가 0보다 크면 -1 곱하자
	_float angleRadiansPitch = XMVectorGetX(angleVecPitch);
	_float angleDegreesPitch = XMConvertToDegrees(angleRadiansPitch);

	fPitch = fPitch_Prev = angleRadiansPitch;
	dCheck(ImGui::SliderAngle("Pitch", &fPitch/*Radians*/, 0.995f * -90.0f, 0.995f * 90.0f), rotDirty_pitch); // 위아래, 0.995f 없으면 에러뜸

	// --------------------------------------------
	// 2. fYaw : 좌우 각도.
	vLook = XMVector3Normalize(vLook);

	// z랑 zx에 투영된선 간의 각도
	XMVECTOR angleVecYaw = XMVector3AngleBetweenVectors(vLookXZ, XMVectorSet(0.f, 0.f, 1.f, 0.f)) * (XMVectorGetX(vLook) < 0 ? -1.f : 1.f);
	_float angleRadiansYaw = XMVectorGetX(angleVecYaw);
	_float angleDegreesYaw = XMConvertToDegrees(angleRadiansYaw);

	fYaw = fYaw_Prev = angleRadiansYaw;
	dCheck(ImGui::SliderAngle("Yaw", &fYaw, -180.0f, 180.0f), rotDirty_yaw); // 좌우

	if (ImGui::Button("Reset"))
	{
		vStatePos = _float4(0.f, 10.f, -15.0f, 0.f);
		fPitch = 0, fYaw = 0;
		posDirty = rotDirty_pitch = rotDirty_pitch = true;
	}

	if (rotDirty_pitch)
	{
		pCamera_Transform->Turn(pCamera_Transform->Get_State(CTransform::STATE_RIGHT), (fPitch - fPitch_Prev == 0) ? 0.001f : fPitch - fPitch_Prev);
		fPitch_Prev = fPitch;
	}

	if (rotDirty_yaw)
	{
		pCamera_Transform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fYaw - fYaw_Prev);
		fYaw_Prev = fYaw;

	}

	if (posDirty)
	{
		pCamera_Transform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vStatePos));
	}

	ImGui::Text("Camera Mode");
	{
	}


	ImGui::Text("Load And Save");
	if (ImGui::Button("Save Camera Data"))
	{
		wstring savePath = SaveFilePath();
		if (savePath != L"")
		{
			HANDLE hFile = ::CreateFile(savePath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (INVALID_HANDLE_VALUE == hFile)
			{
				MessageBox(NULL, savePath.c_str(), L"Save Camera Data 파일 오픈 실패", MB_OK);
			}

			DWORD dwBytes = 0;
			_tchar CameraSaveTest[MAX_PATH] = L"Camera Save Test";
			::WriteFile(hFile, &CameraSaveTest, sizeof(CameraSaveTest), &dwBytes, nullptr);
			::CloseHandle(hFile);
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Camera Data"))
	{
		wstring loadPath = LoadFilePath();
		if (loadPath != L"")
		{
			HANDLE hFile = ::CreateFile(loadPath.c_str(), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (INVALID_HANDLE_VALUE == hFile) // 저장된거 없다.
			{
				MessageBox(NULL, loadPath.c_str(), L"Load Camera Data 할게 없다.(정상)", MB_OK);
			}
			// 저장된거 있으면 불러오자
			DWORD dwBytes = 0;
			_tchar CameraLoadTest[MAX_PATH] = L"";
			::ReadFile(hFile, &CameraLoadTest, sizeof(CameraLoadTest), &dwBytes, nullptr);
			MessageBox(NULL, CameraLoadTest, L"Load Camera 성공", MB_OK);
			::CloseHandle(hFile);
		}
	}


	ImGui::End(); // "Camera Control"
}

void CImguiManager::ShowSpeedFactorControlWindow()
{
	if (!m_bShow_Simulation_Speed)
		return;

	ImGui::Begin("Simulation Speed");
	//ImGui::SliderFloat("Simulation Speed Factor", &m_fSpeedFactor, 0.0f, 4.0f);
	ImGui::DragFloat("Simulation Speed Factor", &m_fSpeedFactor, 0.01f, 0.f, 10.f);
	
	// 카메라 빼고 다 멈추자
	if (ImGui::Button("Play"))
		;// m_pGameInstance->StopAllTickWithOutCamera(false);
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
		; //m_pGameInstance->StopAllTickWithOutCamera(true);
	ImGui::End();
}

void CImguiManager::ImGUI_Key(_float fTimeDelta)
{
	if (CInput_Device::GetInstance()->Key_Down(DIK_F2))
	{
		m_bImGUIEnable = !m_bImGUIEnable;
		if (m_bImGUIEnable)
			EnableCursor();
		else
			DisableCursor();
	}

	if (CInput_Device::GetInstance()->Key_Down(DIK_ESCAPE))
	{
		m_bCursorEnable = !m_bCursorEnable;
		if (m_bCursorEnable)
			EnableCursor();
		else
			DisableCursor();
	}
}

#endif // USE_IMGUI
