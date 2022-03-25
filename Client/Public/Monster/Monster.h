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
	// Ÿ������ tick���� �ѹ�������
	_bool m_bTargetingOnce = false;
	_bool m_bHitted = false; // ���� �ǰݽ�. Render������ ���̴��� �����, Tick������ ü�°�������
	_bool m_bOnceHitted = false; // �ѹ��̶� �ǰݵǾ������Ǵ��ϱ� ����. HpBar�� �ǰ� ���Ŀ� ��µǱ⶧���̴�. 
	_bool m_bWillDead = false;

	_float m_fHitPower = 0.f; // ���� �ǰݽ�. Render������ ���̴��� �����, Tick������ ü�°�������
	const _float HIT_DELAY = 5.f; // �ǰ� ���ӽð�
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