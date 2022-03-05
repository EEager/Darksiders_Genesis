#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

#define DECLATRE_STATIC_SINGLETON(ClassName)	\
public:											\
	static ClassName* GetInstance()				\
	{											\
		static ClassName m_pInstance;			\
		return &m_pInstance;					\
	}											\

// -------------------------------------------------
// #1
// [State] CState_War_Idle
// [Infom] 무기 안들고 가만히 서있는 상태
// -------------------------------------------------
class CState_War_Idle final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Idle)
public:
	CState_War_Idle();
	virtual ~CState_War_Idle() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #2
// [State] CState_War_Run
// [Infom] 무기 안들꼬 뛰는 상태 
// -------------------------------------------------
class CState_War_Run final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Run)
public:
	CState_War_Run();
	virtual ~CState_War_Run() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #3
// [State] CState_War_Idle_to_Idle_Combat
// [Infom] Idle 에서 Idle_Combat 까지 중간 단계 
// -------------------------------------------------
class CState_War_Idle_to_Idle_Combat final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Idle_to_Idle_Combat)

public:
	CState_War_Idle_to_Idle_Combat();
	virtual ~CState_War_Idle_to_Idle_Combat() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #4
// [State] CState_War_Idle_Combat
// [Infom] 검들고 가만히 서있는 상태 
// -------------------------------------------------
class CState_War_Idle_Combat final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Idle_Combat)
public:
	CState_War_Idle_Combat();
	virtual ~CState_War_Idle_Combat() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

private:
	_float m_fIdle_Combat_time = 0.f;

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #5
// [State] CState_War_Idle_Combat_to_Idle
// [Infom] combat idle -> idle 로 가는 상태 
// -------------------------------------------------
class CState_War_Idle_Combat_to_Idle final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Idle_Combat_to_Idle)

public:
	CState_War_Idle_Combat_to_Idle();
	virtual ~CState_War_Idle_Combat_to_Idle() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};