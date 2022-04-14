#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Monster/HollowLord.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_HollowLord_HpBar final : public CGameObject
{
	friend class CHollowLord;

public:
	explicit CUI_HollowLord_HpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_HollowLord_HpBar(const CUI_HollowLord_HpBar& rhs);
	virtual ~CUI_HollowLord_HpBar() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex=0);
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	ComPtr<CTexture>			m_pTextureCom_HpBar = nullptr;
	ComPtr<CTexture>			m_pTextureCom_Portrait = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

private:
	class CHollowLord*			m_pHollowLord = nullptr;

private:
	_float4x4	m_ProjMatrix;
	_float4x4	m_ViewMatrix;

	_float	m_fHpBar_X = 857.f;
	_float	m_fHpBar_Y = 743.f;
	_float	m_fPortrait_X = 551.f;
	_float	m_fPortrait_Y = 732.f;

	_float	m_fAlpha = 0.f;


private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable_HpBar();
	HRESULT SetUp_ConstantTable_Portrait();

public:	
	static CUI_HollowLord_HpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END