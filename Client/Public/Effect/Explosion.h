#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CExplosion final : public CGameObject
{
private:
	explicit CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CExplosion(const CExplosion& rhs);
	virtual ~CExplosion() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect*				m_pModelCom = nullptr;

	_float m_fAnimIdx = 0.f;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();	

public:	
	static CExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END