#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CSoulBarrier final : public CGameObject
{
private:
	explicit CSoulBarrier(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSoulBarrier(const CSoulBarrier& rhs);
	virtual ~CSoulBarrier() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIdx);

private:
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;

private:
	// FSM
	_bool m_bHitted = false;
	_float m_fHitPower = 0.f;
	_bool m_bWillDead = false;

private:
	const char* m_pCurState = "Ballista_A.ao|Balliista_A_Idle";
	const char* m_pNextState = "Ballista_A.ao|Balliista_A_Idle";

private:
	_bool	m_bInitAnimation = false;

public:	
	static CSoulBarrier* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END