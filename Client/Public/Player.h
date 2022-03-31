#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CCollider;
class CTransform;
class CNavigation;
class CModel;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex=0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CNavigation*				m_pNaviCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();	

public:	
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END