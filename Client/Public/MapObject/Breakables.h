#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CBreakableBase final : public CGameObject
{
private:
	explicit CBreakableBase(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBreakableBase(const CBreakableBase& rhs);
	virtual ~CBreakableBase() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	// Collider
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

protected:
	ComPtr<CRenderer>					m_pRendererCom = nullptr;
	ComPtr<CTransform>					m_pTransformCom = nullptr;
	ComPtr<CModel>						m_pModelCom = nullptr;

protected:
	_bool m_bHitted = false;
	_float m_fHitPower = 0.f; 

private:
	HRESULT SetUp_Component(const _tchar* pModelTag);
	HRESULT SetUp_ConstantTable(_uint iPassIndex);	

public:	
	static CBreakableBase* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END