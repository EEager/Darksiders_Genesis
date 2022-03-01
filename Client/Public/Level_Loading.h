#pragma once
#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	explicit CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Loading() = default;
public:
	virtual HRESULT NativeConstruct(LEVEL eNextLevel);
	virtual _int Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont) final;

private:
	const _tchar* GetDots(bool loadFinished);
private:
	const float MAX_DOT_NUM_FLOAT = 7.f;
	_float m_fNumDot = 0;

private:
	LEVEL				m_eNextLevel = LEVEL_END;

	class CLoader*		m_pLoader = nullptr;

public:
	HRESULT Open_Level();
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);
	HRESULT Ready_Layer_LoadingUI(const _tchar* pLayerTag); /* 사본객체를 생성 */

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END