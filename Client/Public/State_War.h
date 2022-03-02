#pragma once

#include "Client_Defines.h"
#include "State.h"

// ---------------------
// CState_War_Idle
// ---------------------
class CState_War_Idle final : public CState
{
	DECLARE_SINGLETON(CState_War_Idle)
public:
	CState_War_Idle();
	virtual ~CState_War_Idle() {}

public:
	virtual void Enter(class CGameObject* pOwner, _float fDeltaTime = 0.f);
	virtual void Execute(class CGameObject* pOwner, _float fDeltaTime = 0.f);
	virtual void Exit(class CGameObject* pOwner, _float fDeltaTime = 0.f);

public:
	virtual void Free() final;
};



// ---------------------
// CState_War_Run
// ---------------------
class CState_War_Run final : public CState
{
	DECLARE_SINGLETON(CState_War_Run)
public:
	CState_War_Run();
	virtual ~CState_War_Run() {}

public:
	virtual void Enter(class CGameObject* pOwner, _float fDeltaTime = 0.f);
	virtual void Execute(class CGameObject* pOwner, _float fDeltaTime = 0.f);
	virtual void Exit(class CGameObject* pOwner, _float fDeltaTime = 0.f);

public:
	virtual void Free() final;
};




