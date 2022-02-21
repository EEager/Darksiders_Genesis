#pragma once
#if defined(USE_IMGUI) 
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Client_Defines.h"

#include "Light.h"

BEGIN(Engine)
class CGameInstance;
END

class CImguiManager
{
	enum RADIOOPERATION { RADIO_TR, RADIO_RT, RADIO_SC };

public:
	static CImguiManager* GetInstance()
	{
		static CImguiManager instance;
		return &instance;
	}

public:
	void Tick(_float fTimeDelta);
	void Render();

	void Initialize(ID3D11Device* pGraphic_Device, ID3D11DeviceContext* pDeviceContext);
	void Release();

private:
	class CGameInstance* m_pGameInstance = nullptr;
	ID3D11Device* m_pGraphic_Device = nullptr;
	ID3D11DeviceContext* m_pDevice_Context = nullptr;
//
	// =========================================
	// 마우스 en/disable
private:
	bool m_bImGUIEnable = true;
	bool m_bCursorEnable = true;
public:
	void EnableCursor();
	void DisableCursor();
	bool GetCursorEnable() { return m_bCursorEnable; }

	// =========================================
	// device 초기화 색상
private:
	static ImVec4 clear_color;
public:
	_float4 GetClearColor() { return _float4(clear_color.x, clear_color.y, clear_color.z, clear_color.w); }

	// =========================================
	// Main Control Window
private:
	void ShowMainControlWindow(_float fDeltaTime);
	HRESULT Open_Level(LEVEL eStartID);
private:
	static bool m_bShow_demo_window;
	static bool m_bshow_camera_window;
	static bool m_bshow_light_window;
	static bool m_bshow_gameobject_control_window;
	static bool m_bshow_editor_window;
	static bool m_bShow_Simulation_Speed;
	int m_iNextLevel = 0;


	// ==========================================
	// Camera Control Window
private:
	void ShowCameraControlWindow();

	// ===========================================
	// Speed Factor
private:
	void ShowSpeedFactorControlWindow();
	_float m_fSpeedFactor = 1.f;
public:
	_float Get_SpeedFactor() { return m_fSpeedFactor; }


	// ==========================================
	// Light Control Window
private:
	void ShowLightControlWindow();
private:
	int m_active_light = 0;
	class CLight* m_pSelected_Light = nullptr;


	// ===========================================
	// Key
private:
	void ImGUI_Key(_float fTimeDelta);

	// ===========================================
	// Save and Load
private:
	wstring SaveFilePath();
	wstring LoadFilePath();
};

#endif
