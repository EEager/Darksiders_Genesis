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
	_bool m_bHeight = true; // ��� ���ʹ� Hp�� 0�� �Ǿ� ���ư��� ���̸� �¿������� 
	_bool m_bTargetingOnce = false; // Ÿ������ tick���� �ѹ�������
	CGameObject* m_pTarget = nullptr;
	CTransform* m_pTargetTransform = nullptr;
	_bool m_bHitted = false; // ���� �ǰݽ�. Render������ ���̴��� �����, Tick������ ü�°�������
	_bool m_bWillDead = false;

	_float m_fHitPower = 0.f; // ���� �ǰݽ�. Render������ ���̴��� �����, Tick������ ü�°�������
	const _float HIT_DELAY = 5.f; // �ǰ� ���ӽð�
	_float m_fHitTimeAcc = 0.f;

	const char* m_pCurState = nullptr;
	const char* m_pNextState = nullptr;
	const char* m_pPreState = nullptr; // ex) �������¸� �����Ͽ�, �ǰ��� �����·� ���ư� �� �ֵ�������
	const char* m_pImpactState_F = nullptr; // �ǰ� �ִϸ��̼��� �� ���͵��� NativeContruct���� �Ҵ�������
	const char* m_pImpactState_B = nullptr; // �ǰ� �ִϸ��̼��� �� ���͵��� NativeContruct���� �Ҵ�������

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