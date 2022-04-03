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
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

protected:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	_float						m_fSpeed = 10.f;

	CNavigation*				m_pNaviCom = nullptr;
	CModel*						m_pModelCom = nullptr;

	// ---------------------------------------------------- 
	// FSM
protected:
	virtual void DoGlobalState(float fTimeDelta);

protected:
	_bool m_bHeight = true; // 모든 몬스터는 Hp가 0이 되어 날아갈때 높이를 태우지말자 
	_bool m_bTargetingOnce = false; // 타겟팅은 tick에서 한번만하자
	CGameObject* m_pTarget = nullptr;
	CTransform* m_pTargetTransform = nullptr;
	_bool m_bHitted = false; // 몬스터 피격시. Render에서는 쉐이더로 노랗게, Tick에서는 체력감소하자
	_bool m_bWillDead = false;

	_float m_fHitPower = 0.f; // 몬스터 피격시. Render에서는 쉐이더로 노랗게, Tick에서는 체력감소하자
	const _float HIT_DELAY = 5.f; // 피격 지속시간
	_float m_fHitTimeAcc = 0.f;

	const char* m_pCurState = nullptr;
	const char* m_pNextState = nullptr;
	const char* m_pPreState = nullptr; // ex) 이전상태를 저장하여, 피격후 전상태로 돌아갈 수 있도록하자
	const char* m_pImpactState_F = nullptr; // 피격 애니메이션은 각 몬스터들이 NativeContruct에서 할당해주자
	const char* m_pImpactState_B = nullptr; // 피격 애니메이션은 각 몬스터들이 NativeContruct에서 할당해주자

public:
	_bool m_bSuperArmor = false;
	// ---------------------------------------------------- 

protected:
	bool isTargetFront(class CTransform* pObj);

public:
	// Collider
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

protected:
	virtual HRESULT SetUp_Component();
	virtual HRESULT SetUp_ConstantTable(_uint iPassIndex);

public:	
	static CMonster* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END