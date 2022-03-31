#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CTexture;
class CModel;
class CNavigation;
END

BEGIN(Client)

class CFork final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };
private:
	explicit CFork(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFork(const CFork& rhs);
	virtual ~CFork() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	_float dissolvePower = 0.001f;

public:
	virtual void OnCollision_Enter(class CGameObject* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(class CGameObject* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(class CGameObject* pDst, float fTimeDelta);

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CCollider*					m_pAABBCom = nullptr;
	CCollider*					m_pOBBCom = nullptr;
	ComPtr<CNavigation>			m_pNaviCom = nullptr; // ComPtr Test
	ComPtr<CTexture>			m_pDissolveTextureCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIndex);	

public:	
	static CFork* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END