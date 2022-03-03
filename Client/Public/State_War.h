#pragma once

#include "Client_Defines.h"
#include "State.h"

BEGIN(Engine)
class CModel;
class CTransform;
END

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
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);

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
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fDeltaTime = 0.f);

private:
	class CModel* m_pModel = nullptr;
	class CTransform* m_pTransform = nullptr;

private:
	_float GetDegree(_ubyte downedKey);
	_bool KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit);

public:
	virtual void Free() final;
};




