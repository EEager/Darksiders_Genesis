#pragma once

#include "Client_Defines.h"
#include "GameInstance.h"
#include "Base.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT NativeConstruct();
	_int Tick(_float fTimeDelta);
	HRESULT Clear();
	HRESULT Render();
	HRESULT PostRender();
	HRESULT Present();

public:
	// Wrapping wndProc
	LRESULT messageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	CGameInstance*					m_pGameInstance = nullptr;
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pDeviceContext = nullptr;
	CRenderer*						m_pRenderer = nullptr;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont;
	std::unique_ptr<DirectX::SpriteFont> m_spriteFont2;

	_float finalFloatingTextPosY = 900.f;
	bool finalFloatingStart = false;

private:
	HRESULT Open_Level(LEVEL eStartID);

	HRESULT Ready_Component_ForStatic();
	HRESULT Ready_GameObject_Prototype();
	HRESULT Ready_Gara();


	// ----------------------
	// For.Test_ShadowMap
private:
	

	// -----------------------


public:
	static CMainApp* Create();
	virtual void Free() override;
};

END

