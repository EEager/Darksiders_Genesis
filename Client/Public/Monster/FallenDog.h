#pragma once

#include "Monster\Monster.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CNavigation;
class CModel;
END

BEGIN(Client)

class CFallenDog final : public CMonster
{
private:
	explicit CFallenDog(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFallenDog(const CFallenDog& rhs);
	virtual ~CFallenDog() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

	//
	// FSM
	// 
protected:
	virtual void UpdateState(); 
	virtual void DoState(float fTimeDelta); 

private:
	void ChangeToAtkStateRandom(); // 6가지 공격 패턴을 랜덤하게 수행하자.

private:
	const _float WAKEUP_RANGE = 30.f;
	const _float CHASE_RANGE = 60.f;
	const _float ATK_RANGE = 6.5f;
	const _float IDLE_TIME_TO_ATK_DELAY = 1.f; 


	_float m_fTimeIdle = 0.f; // 아이들상태에서 어느정도 대기를 해주자. 바로 공격하지는말자.
	_float m_stiffness = 10.f;
	OBJECT_DIR m_eDir = OBJECT_DIR::DIR_F;
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

public:	
	static CFallenDog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END