#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_War_Skills final : public CGameObject
{
private:
	explicit CUI_War_Skills(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUI_War_Skills(const CUI_War_Skills& rhs);
	virtual ~CUI_War_Skills() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex=0);
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	ComPtr<CTexture>			m_pTextureCom_SkillBase = nullptr;
	ComPtr<CTexture>			m_pTextureCom_WrathPower = nullptr;
	ComPtr<CTexture>			m_pTextureCom_Fire = nullptr;
	ComPtr<CTexture>			m_pTextureCom_Leaf = nullptr;
	ComPtr<CTexture>			m_pTextureCom_Button_1 = nullptr;
	ComPtr<CTexture>			m_pTextureCom_Button_G = nullptr;

	CRenderer*					m_pRendererCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;

private:
	class CWar* m_pWar = nullptr;

private:
	_float4x4	m_ViewMatrix;
	_float4x4	m_ProjMatrix;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable_Base();
	HRESULT SetUp_ConstantTable_WrathPower();
	HRESULT SetUp_ConstantTable_Enhancement(CTexture* pEnhance_Texture);

	HRESULT SetUp_ConstantTable_Bnt1();
	HRESULT SetUp_ConstantTable_BntG();

public:	
	static CUI_War_Skills* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END