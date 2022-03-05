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


/* ------------------------------------------------------------------------------
*
*	Global FSM
* 
[#]	[State]							[Event]							[ToState]
1									�ǰݽ�							�˹� �Ǵ� �̰� �����ϰ�? 





* 
*	Finite State Machine
* 
[#]	[State]							[Event]							[ToState]
= = = = = = == = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// �⺻ ������
1	CState_War_Idle					��ó��,R,�뽬					CState_War_Idle_to_Idle_Combat
									����Ű �ϳ��� ����			CState_War_Run
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 ToDo

	
2	CState_War_Run					����Ű �ϳ��� �� ����			CState_War_Idle
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 ToDo

	
3	CState_War_Idle_to_Idle_Combat  �ִϸ��̼� ����					CState_War_Idle_Combat
									����Ű �ϳ��� ����			CState_War_Run_Combat
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 ToDo

	
4	CState_War_Idle_Combat			����Ű �ϳ��� ����			CState_War_Run_Combat
									4�ʰ� �̺�Ʈ����				CState_War_Idle_Combat_to_Idle
									R��ư							CState_War_Idle_Combat_to_Idle
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 ToDo

5	CState_War_Idle_Combat_to_Idle  �ִϸ��̼� ����					CState_War_Idle
									����Ű �ϳ��� ����			CState_War_Run_Combat
									���콺 ����(���)				CState_War_Atk_Light_01 
									���콺 ������(����)				CState_War_Atk_Heavy_01 ToDo

	
6	CState_War_Run_Combat			����Ű �ϳ��� �� ����			CState_War_Idle_Combat
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 ToDo


// ���
7	CState_War_Atk_Light_01			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ����(���)				CState_War_Atk_Light_02

8	CState_War_Atk_Light_02			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ����(���)				CState_War_Atk_Light_02

9	CState_War_Atk_Light_03			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ����(���)				CState_War_Atk_Light_04

10	CState_War_Atk_Light_04			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ������(����)				G��ų  ToDo


// ����
11	CState_War_Atk_Heavy_01			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ������(����)				CState_War_Atk_Heavy_02

12	CState_War_Atk_Heavy_02			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ������(����)				CState_War_Atk_Heavy_03

13	CState_War_Atk_Heavy_03			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ������(����)				G��ų

// �Ϲ� ����
14	CState_War_Jump					�ִϸ��̼� ����					CState_War_Jump_Fall
									�����̽�(����)					CState_War_Jump_Double

15	CState_War_Jump_Double			�ִϸ��̼� ����					CState_War_Jump_Fall

16	CState_War_Jump_Fall			���� ������1					CState_War_Jump_Land
									���� ������2					CState_War_Jump_Land_Heavy
									���� �����鼭 �޸���			CState_War_Jump_Land_Run

17	CState_War_Jump_Land			�ִϸ��̼� ����					CState_War_Idle
18	CState_War_Jump_Land_Heavy		�ִϸ��̼� ����					CState_War_Idle
19	CState_War_Jump_Land_Run		�ִϸ��̼� ����					CState_War_Idle





// Combat ����
20	CState_War_Jump_Combat			�ִϸ��̼� ����					CState_War_Jump_Fall_Combat
21	CState_War_Jump_Fall_Combat		���� ������						CState_War_Jump_Combat_Land
22	



// ���� 


// G��ų
??	CState_War_Atk_EarthSplitter_Level1			�ִϸ��̼� ����					CState_War_Idle_Combat
												���콺 ������(����)				CState_War_Atk_EarthSplitter_Level1
									

-----------------------------------------------------------------------------------*/



// -------------------------------------------------
// #1
// [State] CState_War_Idle
// [Infom] ���� �ȵ�� ������ ���ִ� ����
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
// [Infom] ���� �ȵ鲿 �ٴ� ���� 
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
// [Infom] Idle ���� Idle_Combat ���� �߰� �ܰ� 
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
// [Infom] �˵�� ������ ���ִ� ���� 
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
// [Infom] combat idle -> idle �� ���� ���� 
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

// -------------------------------------------------
// #6
// [State] CState_War_Run_Combat
// [Infom] Į��� �ٴ� ����
// -------------------------------------------------
class CState_War_Run_Combat final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Run_Combat)

public:
	CState_War_Run_Combat();
	virtual ~CState_War_Run_Combat() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #7
// [State] CState_War_Atk_Light_01
// [Infom] ���1
// -------------------------------------------------
class CState_War_Atk_Light_01 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Light_01)

public:
	CState_War_Atk_Light_01();
	virtual ~CState_War_Atk_Light_01() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #8
// [State] CState_War_Atk_Light_02
// [Infom] ���2
// -------------------------------------------------
class CState_War_Atk_Light_02 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Light_02)

public:
	CState_War_Atk_Light_02();
	virtual ~CState_War_Atk_Light_02() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #9
// [State] CState_War_Atk_Light_03
// [Infom] ���3
// -------------------------------------------------
class CState_War_Atk_Light_03 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Light_03)

public:
	CState_War_Atk_Light_03();
	virtual ~CState_War_Atk_Light_03() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #10
// [State] CState_War_Atk_Light_04
// [Infom] ���4
// -------------------------------------------------
class CState_War_Atk_Light_04 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Light_04)

public:
	CState_War_Atk_Light_04();
	virtual ~CState_War_Atk_Light_04() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #11
// [State] CState_War_Atk_Heavy_01
// [Infom] ����1
// -------------------------------------------------
class CState_War_Atk_Heavy_01 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Heavy_01)

public:
	CState_War_Atk_Heavy_01();
	virtual ~CState_War_Atk_Heavy_01() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #12
// [State] CState_War_Atk_Heavy_02
// [Infom] ����2
// -------------------------------------------------
class CState_War_Atk_Heavy_02 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Heavy_02)

public:
	CState_War_Atk_Heavy_02();
	virtual ~CState_War_Atk_Heavy_02() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #13
// [State] CState_War_Atk_Heavy_03
// [Infom] ����1
// -------------------------------------------------
class CState_War_Atk_Heavy_03 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Heavy_03)

public:
	CState_War_Atk_Heavy_03();
	virtual ~CState_War_Atk_Heavy_03() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};