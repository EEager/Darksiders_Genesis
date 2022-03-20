#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_War_Hp_n_Wrath_Bar final : public CGameObject
{
private:
	explicit CUI_War_Hp_n_Wrath_Bar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_War_Hp_n_Wrath_Bar(const CUI_War_Hp_n_Wrath_Bar& rhs);
	virtual ~CUI_War_Hp_n_Wrath_Bar() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex=0);
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	CTexture*					m_pTextureCom = nullptr;
	ComPtr<CTexture>			m_pTextureCom_HpBar = nullptr;
	ComPtr<CTexture>			m_pTextureCom_WrathBar = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

private:
	class CWar* m_pWar = nullptr;

private:
	_float4x4	m_ProjMatrix;
	_float4x4	m_ViewMatrix;


private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable_Base();
	HRESULT SetUp_ConstantTable_HpBar();
	HRESULT SetUp_ConstantTable_WrathBar();

public:	
	static CUI_War_Hp_n_Wrath_Bar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END