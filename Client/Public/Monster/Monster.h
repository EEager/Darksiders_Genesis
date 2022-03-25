#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CNavigation;
class CModel;
END

BEGIN(Client)

class CMonster : public CGameObject
{
protected:
	explicit CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

protected:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	_float						m_fSpeed = 10.f;

	CNavigation*				m_pNaviCom = nullptr;
	CModel*						m_pModelCom = nullptr;

	// ---------------------------------------------------- 
	// 타겟팅은 tick에서 한번만하자
	_bool m_bTargetingOnce = false;
	_bool m_bHitted = false; // 몬스터 피격시. Render에서는 쉐이더로 노랗게, Tick에서는 체력감소하자
	_bool m_bOnceHitted = false; // 한번이라도 피격되었는지판단하기 위함. HpBar는 피격 이후에 출력되기때문이다. 
	_bool m_bWillDead = false;

	_float m_fHitPower = 0.f; // 몬스터 피격시. Render에서는 쉐이더로 노랗게, Tick에서는 체력감소하자
	const _float HIT_DELAY = 5.f; // 피격 지속시간
	_float m_fHitTimeAcc = 0.f;

public:
	// Collider
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

protected:
	virtual void DoGlobalState(float fTimeDelta);

protected:
	virtual HRESULT SetUp_Component();
	virtual HRESULT SetUp_ConstantTable();	

public:	
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END