#pragma once

#include "Base.h"

BEGIN(Engine)

// State Machine�� ���� State �߻� Ŭ����
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
	// CBase��(��) ���� ��ӵ�
	virtual void Free() override;
};

END