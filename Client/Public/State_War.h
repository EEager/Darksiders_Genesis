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
*	Global FSM  :  ��� ���¿����� �߻��� �� �ִ� �̺�Ʈ�� ���� �����ϴ� FSM
* 
[#]	[State]						[Event]							[ToState]
1	CGlobal_State_War			�׾��� 							CState_War_Death
								LShift							CState_War_DashTo_F

2	CState_War_Death			�ִϸ��̼�����					CState_War_Idle

*------------------------------------------------------------------------------*/

// -------------------------------------------------
// #1
// [State] CGlobal_State_War
// [Infom] War_Key, ���� üũ
// -------------------------------------------------
class CGlobal_State_War final : public CState
{
	DECLATRE_STATIC_SINGLETON(CGlobal_State_War)

public:
	CGlobal_State_War();
	virtual ~CGlobal_State_War() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

private:
#define SHIFT_LOCK_TIME 0.5f
	_float m_fShiftLockTimeAcc = 0.f; // Shift ���� ������ 0.5��
	_bool	m_bShiftLockTimeAccStart = false; 

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #2
// [State] CState_War_Death
// [Infom] �׾��� 
// -------------------------------------------------
class CState_War_Death final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Death)

public:
	CState_War_Death();
	virtual ~CState_War_Death() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};









/* ------------------------------------------------------------------------------
* 
*	War Finite State Machine
* 
[#]	[State]							[Event]							[ToState]
= = = = = = == = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// �⺻ ������
1	CState_War_Idle					��ó��,R,�뽬					CState_War_Idle_to_Idle_Combat
									����Ű �ϳ��� ����			CState_War_Run
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 
									�����̽� (����)					CState_War_Jump
									1�� ��ų						CState_War_Wrath_BladeGeyser
									H (��Ÿ��)						CState_War_Horse_Mount_Standing

	
2	CState_War_Run					����Ű �ϳ��� �� ����			CState_War_Idle
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01
									�����̽� (����)					CState_War_Jump
									1�� ��ų						CState_War_Wrath_BladeGeyser
									H (��Ÿ��)						CState_War_Horse_Mount_Running

	
3	CState_War_Idle_to_Idle_Combat  �ִϸ��̼� ����					CState_War_Idle_Combat
									����Ű �ϳ��� ����			CState_War_Run_Combat
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 
									�����̽� (����)					CState_War_Jump_Combat
									1�� ��ų						CState_War_Wrath_BladeGeyser
									H (��Ÿ��)						CState_War_Horse_Mount_Standing

	
4	CState_War_Idle_Combat			����Ű �ϳ��� ����			CState_War_Run_Combat
									4�ʰ� �̺�Ʈ����				CState_War_Idle_Combat_to_Idle
									R��ư							CState_War_Idle_Combat_to_Idle
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 
									�����̽� (����)					CState_War_Jump_Combat
									1�� ��ų						CState_War_Wrath_BladeGeyser
									H (��Ÿ��)						CState_War_Horse_Mount_Standing

5	CState_War_Idle_Combat_to_Idle  �ִϸ��̼� ����					CState_War_Idle
									����Ű �ϳ��� ����			CState_War_Run_Combat
									���콺 ����(���)				CState_War_Atk_Light_01 
									���콺 ������(����)				CState_War_Atk_Heavy_01 
									�����̽� (����)					CState_War_Jump_Combat
									1�� ��ų						CState_War_Wrath_BladeGeyser
									H (��Ÿ��)						CState_War_Horse_Mount_Standing

	
6	CState_War_Run_Combat			����Ű �ϳ��� �� ����			CState_War_Idle_Combat
									���콺 ����(���)				CState_War_Atk_Light_01
									���콺 ������(����)				CState_War_Atk_Heavy_01 
									�����̽� (����)					CState_War_Jump_Combat
									1�� ��ų						CState_War_Wrath_BladeGeyser
									H (��Ÿ��)						CState_War_Horse_Mount_Running


// ��� 
7	CState_War_Atk_Light_01			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ����(���)				CState_War_Atk_Light_02
									����Ű �ϳ��� ����			CState_War_Run_Combat

8	CState_War_Atk_Light_02			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ����(���)				CState_War_Atk_Light_02
									����Ű �ϳ��� ����			CState_War_Run_Combat

9	CState_War_Atk_Light_03			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ����(���)				CState_War_Atk_Light_04
									����Ű �ϳ��� ����			CState_War_Run_Combat

10	CState_War_Atk_Light_04			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ������(����)				G��ų  ToDo
									����Ű �ϳ��� ����			CState_War_Run_Combat


// ����
11	CState_War_Atk_Heavy_01			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ������(����)				CState_War_Atk_Heavy_02
									���콺 ������ 1���̻�			CState_War_Atk_EarthSplitter_Charge_Start
																	CState_War_Atk_Flamebrand_Start

									����Ű �ϳ��� ����			CState_War_Run_Combat

12	CState_War_Atk_Heavy_02			�ִϸ��̼� ����					CState_War_Idle_Combat
									���콺 ������(����)				CState_War_Atk_Heavy_03
									����Ű �ϳ��� ����			CState_War_Run_Combat

13	CState_War_Atk_Heavy_03			�ִϸ��̼� ����					CState_War_Idle_Combat
									����Ű �ϳ��� ����			CState_War_Run_Combat
									���콺 ������(����)				G��ų




// �Ϲ� ����
14	CState_War_Jump					�ִϸ��̼� ����					CState_War_Jump_Fall
									�����̽�(����)					CState_War_Jump_Double
									���콺 ������(����)				CState_War_Atk_Air_Light_03_NoImpulse
									������1							CState_War_Jump_Land
									������ + �޸���					CState_War_Jump_Land_Run

15	CState_War_Jump_Fall			������1							CState_War_Jump_Land
									������2							CState_War_Jump_Land_Heavy
									������ + �޸���					CState_War_Jump_Land_Run
									�����̽�(����)					CState_War_Jump_Double
									���콺 ������(����)				CState_War_Atk_Air_Light_03_NoImpulse


16	CState_War_Jump_Land			�ִϸ��̼� ����					CState_War_Idle
									����Ű �ϳ��� ����			CState_War_Run
17	CState_War_Jump_Land_Heavy		�ִϸ��̼� ����					CState_War_Idle
									����Ű �ϳ��� ����			CState_War_Run

18	CState_War_Jump_Land_Run		�ִϸ��̼� ����					CState_War_Run
									����Ű �ϳ��� ����			CState_War_Run


// Combat ����
19	CState_War_Jump_Combat			�ִϸ��̼� ����					CState_War_Jump_Fall_Combat
									�����̽�(����)					CState_War_Jump_Double
									���콺 ������(����)				CState_War_Atk_Air_Light_03_NoImpulse
									������							CState_War_Jump_Combat_Land
									������ + ����					CState_War_Jump_Combat_Land_Run

20	CState_War_Jump_Fall_Combat		������							CState_War_Jump_Combat_Land
									������ + ����					CState_War_Jump_Combat_Land_Run
									�����̽�(����)					CState_War_Jump_Double
									���콺 ������(����)				CState_War_Atk_Air_Light_03_NoImpulse

21	CState_War_Jump_Combat_Land		�ִϸ��̼� ����					CState_War_Idle_Combat
									����Ű �ϳ��� ����			CState_War_Run_Combat

22	CState_War_Jump_Combat_Land_Run	�ִϸ��̼� ����					CState_War_Run_Combat
									����Ű �ϳ��� ����			CState_War_Run_Combat


// ��������
23	CState_War_Jump_Double			�ִϸ��̼� ����					CState_War_Jump_Fall
									���콺 ������(����)				CState_War_Atk_Air_Light_03_NoImpulse
									������1							CState_War_Jump_Land
									������2							CState_War_Jump_Land_Heavy
									������ + �޸���					CState_War_Jump_Land_Run
									H ��ư							CState_War_Horse_Mount_Running


// ���� �� ������ - War_Atk_Air_Light_03_NoImpulse ������
24	CState_War_Atk_Air_Light_03_NoImpulse	�ִϸ��̼� ����			CState_War_Atk_Air_Light_03_Fall
											������					CState_War_Atk_Air_Land

25	CState_War_Atk_Air_Light_03_Fall		������					CState_War_Atk_Air_Land

26	CState_War_Atk_Air_Land			�ִϸ��̼� ����					CState_War_Idle_Combat
									����Ű �ϳ��� ����			CState_War_Run_Combat


// 1��ų 
27	CState_War_Wrath_BladeGeyser	�ִϸ��̼� ����					CState_War_Idle_Combat
									����Ű �ϳ��� ����			CState_War_Run_Combat



// �޺� ��ų - ��������
28	CState_War_Atk_EarthSplitter_Charge_Start		�ִϸ��̼� ����		CState_War_Atk_EarthSplitter_Charge_Loop	
29	CState_War_Atk_EarthSplitter_Charge_Loop		���콺 �����ʶ���	CState_War_Atk_EarthSplitter_Level1

30	CState_War_Atk_EarthSplitter_Level1				�ִϸ��̼� ����		CState_War_Idle_Combat
													����Ű �ϳ��� ����	CState_War_Run_Combat

// �޺� ��ų - ǳ��������
31	CState_War_Atk_Flamebrand_Start		�ִϸ��̼� ����		CState_War_Atk_Flamebrand

32	CState_War_Atk_Flamebrand			Max �ð������ų� ������ ���콺 ����		CState_War_Atk_Flamebrand_End

33	CState_War_Atk_Flamebrand_End		�ִϸ��̼� ����			CState_War_Idle_Combat
										hore����Ű �ϳ��� ����	CState_War_Run_Combat


// �뽬
34	CState_War_DashTo_F					�ִϸ��̼� ����			CState_War_Idle_Combat
		
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

private:
	_float m_fTimeAcc = 0.f;

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

private:
	_float m_fTimeAcc = 0.f;

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

private:
	_float m_fTimeAcc = 0.f;

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

private:
	_float m_fTimeAcc = 0.f;

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

private:
	_float m_fBntPressTime = 0.f;
	_bool m_bChargeStart = false;
	_float m_fTimeAcc = 0.f;

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

private:
	_float m_fTimeAcc = 0.f;

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

private:
	_float m_fTimeAcc = 0.f;

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #14
// [State] CState_War_Jump
// [Infom] ����ȵ�� ����
// -------------------------------------------------
class CState_War_Jump final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump)

public:
	CState_War_Jump();
	virtual ~CState_War_Jump() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);


public:
	virtual void Free() final;
};

// -------------------------------------------------
// #15
// [State] CState_War_Jump_Fall
// [Infom] ����ȵ�� ������
// -------------------------------------------------
class CState_War_Jump_Fall final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Fall)

public:
	CState_War_Jump_Fall();
	virtual ~CState_War_Jump_Fall() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

private:
	_float m_fFlightTime = 0.f;

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #16
// [State] CState_War_Jump_Land
// [Infom] ����1
// -------------------------------------------------
class CState_War_Jump_Land final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Land)

public:
	CState_War_Jump_Land();
	virtual ~CState_War_Jump_Land() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #17
// [State] CState_War_Jump_Land_Heavy
// [Infom] ����2
// -------------------------------------------------
class CState_War_Jump_Land_Heavy final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Land_Heavy)

public:
	CState_War_Jump_Land_Heavy();
	virtual ~CState_War_Jump_Land_Heavy() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #18
// [State] CState_War_Jump_Land_Run
// [Infom] �����
// -------------------------------------------------
class CState_War_Jump_Land_Run final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Land_Run)

public:
	CState_War_Jump_Land_Run();
	virtual ~CState_War_Jump_Land_Run() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #19
// [State] CState_War_Jump_Combat
// [Infom] Į��� ����
// -------------------------------------------------
class CState_War_Jump_Combat final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Combat)

public:
	CState_War_Jump_Combat();
	virtual ~CState_War_Jump_Combat() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #20
// [State] CState_War_Jump_Fall_Combat
// [Infom] Į��� ������
// -------------------------------------------------
class CState_War_Jump_Fall_Combat final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Fall_Combat)

public:
	CState_War_Jump_Fall_Combat();
	virtual ~CState_War_Jump_Fall_Combat() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #21
// [State] CState_War_Jump_Combat_Land
// [Infom] Į��� ����
// -------------------------------------------------
class CState_War_Jump_Combat_Land final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Combat_Land)

public:
	CState_War_Jump_Combat_Land();
	virtual ~CState_War_Jump_Combat_Land() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #22
// [State] CState_War_Jump_Combat_Land_Run
// [Infom] Į��� �����ϸ鼭 �ٱ�
// -------------------------------------------------
class CState_War_Jump_Combat_Land_Run final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Combat_Land_Run)

public:
	CState_War_Jump_Combat_Land_Run();
	virtual ~CState_War_Jump_Combat_Land_Run() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #23
// [State] CState_War_Jump_Double
// [Infom] �������� 
// -------------------------------------------------
class CState_War_Jump_Double final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Jump_Double)

public:
	CState_War_Jump_Double();
	virtual ~CState_War_Jump_Double() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

private:
	_float m_fFlightTime = 0.f;

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #24
// [State] CState_War_Atk_Air_Light_03_NoImpulse
// [Infom] ������ ������
// -------------------------------------------------
class CState_War_Atk_Air_Light_03_NoImpulse final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Air_Light_03_NoImpulse)

public:
	CState_War_Atk_Air_Light_03_NoImpulse();
	virtual ~CState_War_Atk_Air_Light_03_NoImpulse() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #25
// [State] CState_War_Atk_Air_Light_03_Fall
// [Infom] ���� ���� ��
// -------------------------------------------------
class CState_War_Atk_Air_Light_03_Fall final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Air_Light_03_Fall)

public:
	CState_War_Atk_Air_Light_03_Fall();
	virtual ~CState_War_Atk_Air_Light_03_Fall() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #26
// [State] CState_War_Atk_Air_Land
// [Infom] ���� ������ ����
// -------------------------------------------------
class CState_War_Atk_Air_Land final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Air_Land)

public:
	CState_War_Atk_Air_Land();
	virtual ~CState_War_Atk_Air_Land() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #27
// [State] CState_War_Wrath_BladeGeyser
// [Infom] 1�� ��ų
// -------------------------------------------------
class CState_War_Wrath_BladeGeyser final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Wrath_BladeGeyser)

public:
	CState_War_Wrath_BladeGeyser();
	virtual ~CState_War_Wrath_BladeGeyser() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

private:
	bool m_bCreate = false;

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #28
// [State] CState_War_Atk_EarthSplitter_Charge_Start
// [Infom] G��ų - �� ������� ����
// -------------------------------------------------
class CState_War_Atk_EarthSplitter_Charge_Start final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_EarthSplitter_Charge_Start)

public:
	CState_War_Atk_EarthSplitter_Charge_Start();
	virtual ~CState_War_Atk_EarthSplitter_Charge_Start() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #29
// [State] CState_War_Atk_EarthSplitter_Charge_Loop
// [Infom] G��ų - �� ������� ��
// -------------------------------------------------
class CState_War_Atk_EarthSplitter_Charge_Loop final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_EarthSplitter_Charge_Loop)

public:
	CState_War_Atk_EarthSplitter_Charge_Loop();
	virtual ~CState_War_Atk_EarthSplitter_Charge_Loop() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #30
// [State] CState_War_Atk_EarthSplitter_Level1
// [Infom] G��ų - �� ������� ��. �߻� 
// -------------------------------------------------
class CState_War_Atk_EarthSplitter_Level1 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_EarthSplitter_Level1)

public:
	CState_War_Atk_EarthSplitter_Level1();
	virtual ~CState_War_Atk_EarthSplitter_Level1() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #31
// [State] CState_War_Atk_Flamebrand_Start
// [Infom] G��ų - ��ǳ�� ������
// -------------------------------------------------
class CState_War_Atk_Flamebrand_Start final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Flamebrand_Start)

public:
	CState_War_Atk_Flamebrand_Start();
	virtual ~CState_War_Atk_Flamebrand_Start() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #32
// [State] CState_War_Atk_Flamebrand
// [Infom] G��ų - ��ǳ�� ������ ��
// -------------------------------------------------
class CState_War_Atk_Flamebrand final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Flamebrand)

public:
	CState_War_Atk_Flamebrand();
	virtual ~CState_War_Atk_Flamebrand() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	
private:
	_float	m_fSpinningTimeAcc = 0.f;

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #33
// [State] CState_War_Atk_Flamebrand_End
// [Infom] G��ų - ��ǳ�� ������ ��
// -------------------------------------------------
class CState_War_Atk_Flamebrand_End final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Atk_Flamebrand_End)

public:
	CState_War_Atk_Flamebrand_End();
	virtual ~CState_War_Atk_Flamebrand_End() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #34
// [State] CState_War_DashTo_F
// [Infom] �뽬
// -------------------------------------------------
class CState_War_DashTo_F final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_DashTo_F)

public:
	CState_War_DashTo_F();
	virtual ~CState_War_DashTo_F() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

private:
	_float m_fMoveLockTimeAcc = 0.f;

	_bool m_bParticleOn = false;

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #35
// [State] CState_War_Impact_From_Front_01
// [Infom] �տ��� �浹�ھҴ�. �ڷ� ������.
// -------------------------------------------------
class CState_War_Impact_From_Front_01 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Impact_From_Front_01)

public:
	CState_War_Impact_From_Front_01();
	virtual ~CState_War_Impact_From_Front_01() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
private:
	_float m_fMoveLockTimeAcc = 0.f;

public:
	virtual void Free() final;
};

// -------------------------------------------------
// #36
// [State] CState_War_Impact_From_Back_01
// [Infom] �ڿ��� �浹�ھҴ�. ������ ������.
// -------------------------------------------------
class CState_War_Impact_From_Back_01 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Impact_From_Back_01)

public:
	CState_War_Impact_From_Back_01();
	virtual ~CState_War_Impact_From_Back_01() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
private:
	_float m_fMoveLockTimeAcc = 0.f;
public:
	virtual void Free() final;
};

// -------------------------------------------------
// #37
// [State] CState_War_Impact_From_Left_01
// [Infom] ���ʿ��� �ھҴ�. ���������� �з�����.
// -------------------------------------------------
class CState_War_Impact_From_Left_01 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Impact_From_Left_01)

public:
	CState_War_Impact_From_Left_01();
	virtual ~CState_War_Impact_From_Left_01() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
private:
	_float m_fMoveLockTimeAcc = 0.f;
public:
	virtual void Free() final;
};

// -------------------------------------------------
// #38
// [State] CState_War_Impact_From_Right_01
// [Infom] �տ��� �浹�ھҴ�. �ڷ� ������.
// -------------------------------------------------
class CState_War_Impact_From_Right_01 final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Impact_From_Right_01)

public:
	CState_War_Impact_From_Right_01();
	virtual ~CState_War_Impact_From_Right_01() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
private:
	_float m_fMoveLockTimeAcc = 0.f;
public:
	virtual void Free() final;
};

// -------------------------------------------------
// #39
// [State] CState_War_IA_Death_FallenDog
// [Infom] FallenDog ó�����
// -------------------------------------------------
class CState_War_IA_Death_FallenDog final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_IA_Death_FallenDog)

public:
	CState_War_IA_Death_FallenDog();
	virtual ~CState_War_IA_Death_FallenDog() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
public:
	virtual void Free() final;
};


/* ------------------------------------------------------------------------------
*
*	War n Horse Finite State Machine
*
[#]	[State]								[Event]						[ToState]
= = = = = = == = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// ��Ÿ��, ��������
1	CState_War_Horse_Mount_Standing		�ִϸ��̼� ����				CState_War_Horse_Idle

2	CState_War_Horse_Mount_Running		�ִϸ��̼� ����				CState_War_Horse_Gallop

3	CState_War_Horse_Dismount			�ִϸ��̼� ����				CState_War_Horse_Jump_Land_Heavy


// ���̵�
4	CState_War_Horse_Idle				H ��ư						CState_War_Horse_Dismount
										����Ű�ϳ���				CState_War_Horse_Gallop
// �޸���
5	CState_War_Horse_Gallop				����Ű�ϳ��󵵾ȴ�����		CState_War_Horse_Idle	
										LSHIFT						CState_War_Horse_Gallop_Fast_Start
										H ��ư						CState_War_Horse_Dismount

6	CState_War_Horse_Gallop_Fast_Start	�ִϸ��̼� ����				CState_War_Horse_Gallop_Fast

7	CState_War_Horse_Gallop_Fast		LSHIFT �����				CState_War_Horse_Gallop
										����Ű�ϳ��󵵾ȴ�����		CState_War_Horse_Stop

8	CState_War_Horse_Stop				�ִϸ��̼� ����				CState_War_Horse_Idle

9	CState_War_Horse_Jump_Land_Heavy	�ִϸ��̼� ����				CState_War_Idle
										����Ű �ϳ��� ����		CState_War_Run

										

--------------------------------------------------------------------------------*/


#define WAR_SPEED 5.0f
#define WAR_NO_WEAPON_SPEED 7.f
#define RUIN_SPEED 10.f
#define RUIN_SHIFT_SPEED 18.f

// -------------------------------------------------
// #Horse1
// [State] CState_War_Horse_Mount_Standing
// [Infom] ���ִ� ���¿��� ��Ÿ�� ���� 
// -------------------------------------------------
class CState_War_Horse_Mount_Standing final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Mount_Standing)

public:
	CState_War_Horse_Mount_Standing();
	virtual ~CState_War_Horse_Mount_Standing() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #Horse2
// [State] CState_War_Horse_Mount_Running
// [Infom] �޸��� ���¿��� ��Ÿ�� ��ư ����
// -------------------------------------------------
class CState_War_Horse_Mount_Running final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Mount_Running)

public:
	CState_War_Horse_Mount_Running();
	virtual ~CState_War_Horse_Mount_Running() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #Horse3
// [State] CState_War_Horse_Dismount
// [Infom] ��Ÿ�� ����
// -------------------------------------------------
class CState_War_Horse_Dismount final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Dismount)

public:
	CState_War_Horse_Dismount();
	virtual ~CState_War_Horse_Dismount() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};




// -------------------------------------------------
// #Horse4
// [State] CState_War_Horse_Idle
// [Infom] �⺻ ����
// -------------------------------------------------
class CState_War_Horse_Idle final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Idle)

public:
	CState_War_Horse_Idle();
	virtual ~CState_War_Horse_Idle() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #Horse5
// [State] CState_War_Horse_Gallop
// [Infom] �⺻ �޸��� 
// -------------------------------------------------
class CState_War_Horse_Gallop final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Gallop)

public:
	CState_War_Horse_Gallop();
	virtual ~CState_War_Horse_Gallop() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};




// -------------------------------------------------
// #Horse6
// [State] CState_War_Horse_Gallop_Fast_Start
// [Infom] LSHIFT �޸��� ���� 
// -------------------------------------------------
class CState_War_Horse_Gallop_Fast_Start final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Gallop_Fast_Start)

public:
	CState_War_Horse_Gallop_Fast_Start();
	virtual ~CState_War_Horse_Gallop_Fast_Start() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #Horse7
// [State] CState_War_Horse_Gallop_Fast
// [Infom] LSHIFT �޸���
// -------------------------------------------------
class CState_War_Horse_Gallop_Fast final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Gallop_Fast)

public:
	CState_War_Horse_Gallop_Fast();
	virtual ~CState_War_Horse_Gallop_Fast() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

private:
	bool m_bParticleOn = false;

public:
	virtual void Free() final;
};


// -------------------------------------------------
// #Horse8
// [State] CState_War_Horse_Stop
// [Infom] LSHIFT �޸���ٰ� ���ߴ� ����
// -------------------------------------------------
class CState_War_Horse_Stop final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Stop)

public:
	CState_War_Horse_Stop();
	virtual ~CState_War_Horse_Stop() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #Horse9
// [State] CState_War_Horse_Jump_Land_Heavy
// [Infom] �������� ���� ����
// -------------------------------------------------
class CState_War_Horse_Jump_Land_Heavy final : public CState
{
	DECLATRE_STATIC_SINGLETON(CState_War_Horse_Jump_Land_Heavy)

public:
	CState_War_Horse_Jump_Land_Heavy();
	virtual ~CState_War_Horse_Jump_Land_Heavy() {}

public:
	virtual void Enter(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Execute(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);
	virtual void Exit(class CGameObject* pOwner = nullptr, _float fTimeDelta = 0.f);

public:
	virtual void Free() final;
};
