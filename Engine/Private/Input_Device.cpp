#include "..\public\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
{
	ZeroMemory(m_bState, sizeof(m_bState));
	ZeroMemory(m_bMouseState, sizeof(m_bMouseState));

}

bool CInput_Device::Key_Pressing(_ubyte eKeyID)
{
	if (Get_DIKeyState(eKeyID) & 0x80)
		return true;
	return false;
}

bool CInput_Device::Mouse_Pressing(MOUSEBUTTONSTATE eState)
{
	if (m_MouseState.rgbButtons[eState])
		return true;
	return false;
}

bool CInput_Device::Key_Down(_ubyte eKeyID)
{
	if (!m_bState[eKeyID] && (Get_DIKeyState(eKeyID) & 0x80))
	{
		m_bState[eKeyID] = true;
		return true;
	}

	if (m_bState[eKeyID] && !(Get_DIKeyState(eKeyID) & 0x80))
		m_bState[eKeyID] = false;

	return false;
}
bool CInput_Device::Mouse_Down(MOUSEBUTTONSTATE eState)
{
	if (!m_bMouseState[eState] && (m_MouseState.rgbButtons[eState]))
	{
		m_bMouseState[eState] = true;
		return true;
	}

	if (m_bMouseState[eState] && !(m_MouseState.rgbButtons[eState]))
		m_bMouseState[eState] = false;

	return false;
}

bool CInput_Device::Mouse_Up(MOUSEBUTTONSTATE eState)
{
	if (m_bMouseState[eState] && !(m_MouseState.rgbButtons[eState]))
	{
		m_bMouseState[eState] = !m_bMouseState[eState];
		return true;
	}

	if (!m_bState[eState] && (m_MouseState.rgbButtons[eState]))
		m_bMouseState[eState] = !m_bMouseState[eState];

	return false;
}

bool CInput_Device::Key_Up(_ubyte eKeyID)
{
	if (m_bState[eKeyID] && !(Get_DIKeyState(eKeyID) & 0x80))
	{
		m_bState[eKeyID] = !m_bState[eKeyID];
		return true;
	}

	if (!m_bState[eKeyID] && (Get_DIKeyState(eKeyID) & 0x80))
		m_bState[eKeyID] = !m_bState[eKeyID];

	return false;
}

HRESULT CInput_Device::Ready(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pKeyboard->Acquire();

	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	return S_OK;
}

HRESULT CInput_Device::Update()
{
	if (nullptr == m_pKeyboard ||
		nullptr == m_pMouse)
		return E_FAIL;

	/* 키보드와 마우스의 상태를 얻어온다. */
	ZeroMemory(&m_MouseState, sizeof(m_MouseState));
	ZeroMemory(&m_byKeyState, sizeof(m_byKeyState));

	m_pKeyboard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);

	return S_OK;
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInput);
}
