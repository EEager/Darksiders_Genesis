#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)
public:
	enum MOUSEMOVESTATE { DIMM_X, DIMM_Y, DIMM_WHEEL, DIMM_END };
	enum MOUSEBUTTONSTATE { DIMB_LBUTTON, DIMB_RBUTTON, DIMB_WHEEL, DIMB_END };
private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	// ==================================
	// 키보드
	_ubyte Get_DIKeyState(_ubyte eKeyID) {
		return m_byKeyState[eKeyID];
	}
	bool Key_Pressing(_ubyte eKeyID);
	bool Key_Down(_ubyte eKeyID);
	bool Key_Up(_ubyte eKeyID);

	// ==================================
	// 마우스
	_long Get_DIMouseMoveState(MOUSEMOVESTATE eMouseMoveState) {

		return ((_long*)&m_MouseState)[eMouseMoveState];
	}
	_byte Get_DIMouseButtonState(MOUSEBUTTONSTATE eMouseButtonState) {
		return m_MouseState.rgbButtons[eMouseButtonState];
	}

	bool Mouse_Down(MOUSEBUTTONSTATE eState);
public:
	HRESULT Ready(HINSTANCE hInst, HWND hWnd);
	HRESULT Update();

private:
	LPDIRECTINPUT8				m_pInput = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse= nullptr;

private:
	_byte			m_byKeyState[256];
	DIMOUSESTATE	m_MouseState;

private:	
#define VK_MAX 0xff
	bool	m_bState[VK_MAX];
	bool	m_bMouseState[DIMB_END];

public:
	virtual void Free() override;
};

END