#pragma once

#include "Base.h"

BEGIN(Engine)

// State Machine에 들어가는 State 추상 클래스
class ENGINE_DLL CState : public CBase
{
public:
	virtual ~CState() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float dt = 0.f) = 0;
	virtual void Execute(class CGameObject* pOwner = nullptr, _float dt = 0.f) = 0;
	virtual void Exit(class CGameObject* pOwner = nullptr, _float dt = 0.f) = 0;

protected:
	const char* m_pStateName = nullptr;
	_float m_fExecuteTime = 0.f;

public:
	// CBase을(를) 통해 상속됨
	virtual void Free() override;
};

END