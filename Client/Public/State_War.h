#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CModel;
class CTransform;
END


// -------------------------------------------------
// CState_War_Idle
// Desc : 무기 안들고 가만히 서있는 상태
// -------------------------------------------------
class CState_War_Idle final : public CState
{
	DECLARE_SINGLETON(CState_War_Idle)
public:
	CState_War_Idle();
	virtual ~CState_War_Idle() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// CState_War_Run
// Desc : 무기 안들꼬 뛰는 상태 
// -------------------------------------------------
class CState_War_Run final : public CState
{
	DECLARE_SINGLETON(CState_War_Run)
public:
	CState_War_Run();
	virtual ~CState_War_Run() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);

private:
	_float GetDegree(_ubyte downedKey);
	_bool KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// CState_War_Idle_to_Idle_Combat
// Desc : Idle 에서 Idle_Combat 까지 중간 단계 
// -------------------------------------------------
class CState_War_Idle_to_Idle_Combat final : public CState
{
	DECLARE_SINGLETON(CState_War_Idle_to_Idle_Combat)
public:
	CState_War_Idle_to_Idle_Combat();
	virtual ~CState_War_Idle_to_Idle_Combat() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);

public:
	virtual void Free() final;
};