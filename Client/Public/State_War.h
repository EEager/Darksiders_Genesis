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
*	Global FSM  :  어느 상태에서나 발생할 수 있는 이벤트에 의한 동작하는 FSM
* 
[#]	[State]						[Event]							[ToState]
1	CGlobal_State_War			죽었다 							CState_War_Death
								LShift							CState_War_DashTo_F

2	CState_War_Death			애니메이션종료					CState_War_Idle

*------------------------------------------------------------------------------*/

// -------------------------------------------------
// #1
// [State] CGlobal_State_War
// [Infom] War_Key, 죽음 체크
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
	_float m_fShiftLockTimeAcc = 0.f; // Shift 간에 간격은 0.5초
	_bool	m_bShiftLockTimeAccStart = false; 

public:
	virtual void Free() final;
};



// -------------------------------------------------
// #2
// [State] CState_War_Death
// [Infom] 죽었다 
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
// 기본 움직임
1	CState_War_Idle					근처적,R,대쉬					CState_War_Idle_to_Idle_Combat
									방향키 하나라도 누름			CState_War_Run
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 
									스페이스 (점프)					CState_War_Jump
									1번 스킬						CState_War_Wrath_BladeGeyser
									H (말타기)						CState_War_Horse_Mount_Standing

	
2	CState_War_Run					방향키 하나도 안 누름			CState_War_Idle
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01
									스페이스 (점프)					CState_War_Jump
									1번 스킬						CState_War_Wrath_BladeGeyser
									H (말타기)						CState_War_Horse_Mount_Running

	
3	CState_War_Idle_to_Idle_Combat  애니메이션 종료					CState_War_Idle_Combat
									방향키 하나라도 누름			CState_War_Run_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 
									스페이스 (점프)					CState_War_Jump_Combat
									1번 스킬						CState_War_Wrath_BladeGeyser
									H (말타기)						CState_War_Horse_Mount_Standing

	
4	CState_War_Idle_Combat			방향키 하나라도 누름			CState_War_Run_Combat
									4초간 이벤트없음				CState_War_Idle_Combat_to_Idle
									R버튼							CState_War_Idle_Combat_to_Idle
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 
									스페이스 (점프)					CState_War_Jump_Combat
									1번 스킬						CState_War_Wrath_BladeGeyser
									H (말타기)						CState_War_Horse_Mount_Standing

5	CState_War_Idle_Combat_to_Idle  애니메이션 종료					CState_War_Idle
									방향키 하나라도 누름			CState_War_Run_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_01 
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 
									스페이스 (점프)					CState_War_Jump_Combat
									1번 스킬						CState_War_Wrath_BladeGeyser
									H (말타기)						CState_War_Horse_Mount_Standing

	
6	CState_War_Run_Combat			방향키 하나도 안 누름			CState_War_Idle_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 
									스페이스 (점프)					CState_War_Jump_Combat
									1번 스킬						CState_War_Wrath_BladeGeyser
									H (말타기)						CState_War_Horse_Mount_Running


// 약공 
7	CState_War_Atk_Light_01			애니메이션 종료					CState_War_Idle_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_02
									방향키 하나라도 누름			CState_War_Run_Combat

8	CState_War_Atk_Light_02			애니메이션 종료					CState_War_Idle_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_02
									방향키 하나라도 누름			CState_War_Run_Combat

9	CState_War_Atk_Light_03			애니메이션 종료					CState_War_Idle_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_04
									방향키 하나라도 누름			CState_War_Run_Combat

10	CState_War_Atk_Light_04			애니메이션 종료					CState_War_Idle_Combat
									마우스 오른쪽(강공)				G스킬  ToDo
									방향키 하나라도 누름			CState_War_Run_Combat


// 강공
11	CState_War_Atk_Heavy_01			애니메이션 종료					CState_War_Idle_Combat
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_02
									마우스 오른쪽 1초이상			CState_War_Atk_EarthSplitter_Charge_Start
																	CState_War_Atk_Flamebrand_Start

									방향키 하나라도 누름			CState_War_Run_Combat

12	CState_War_Atk_Heavy_02			애니메이션 종료					CState_War_Idle_Combat
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_03
									방향키 하나라도 누름			CState_War_Run_Combat

13	CState_War_Atk_Heavy_03			애니메이션 종료					CState_War_Idle_Combat
									방향키 하나라도 누름			CState_War_Run_Combat
									마우스 오른쪽(강공)				G스킬




// 일반 점프
14	CState_War_Jump					애니메이션 종료					CState_War_Jump_Fall
									스페이스(점프)					CState_War_Jump_Double
									마우스 오른쪽(강공)				CState_War_Atk_Air_Light_03_NoImpulse
									착지시1							CState_War_Jump_Land
									착지시 + 달리기					CState_War_Jump_Land_Run

15	CState_War_Jump_Fall			착지시1							CState_War_Jump_Land
									착지시2							CState_War_Jump_Land_Heavy
									착지시 + 달리기					CState_War_Jump_Land_Run
									스페이스(점프)					CState_War_Jump_Double
									마우스 오른쪽(강공)				CState_War_Atk_Air_Light_03_NoImpulse


16	CState_War_Jump_Land			애니메이션 종료					CState_War_Idle
									방향키 하나라도 누름			CState_War_Run
17	CState_War_Jump_Land_Heavy		애니메이션 종료					CState_War_Idle
									방향키 하나라도 누름			CState_War_Run

18	CState_War_Jump_Land_Run		애니메이션 종료					CState_War_Run
									방향키 하나라도 누름			CState_War_Run


// Combat 점프
19	CState_War_Jump_Combat			애니메이션 종료					CState_War_Jump_Fall_Combat
									스페이스(점프)					CState_War_Jump_Double
									마우스 오른쪽(강공)				CState_War_Atk_Air_Light_03_NoImpulse
									착지시							CState_War_Jump_Combat_Land
									착지시 + 방향					CState_War_Jump_Combat_Land_Run

20	CState_War_Jump_Fall_Combat		착지시							CState_War_Jump_Combat_Land
									착지시 + 방향					CState_War_Jump_Combat_Land_Run
									스페이스(점프)					CState_War_Jump_Double
									마우스 오른쪽(강공)				CState_War_Atk_Air_Light_03_NoImpulse

21	CState_War_Jump_Combat_Land		애니메이션 종료					CState_War_Idle_Combat
									방향키 하나라도 누름			CState_War_Run_Combat

22	CState_War_Jump_Combat_Land_Run	애니메이션 종료					CState_War_Run_Combat
									방향키 하나라도 누름			CState_War_Run_Combat


// 더블점프
23	CState_War_Jump_Double			애니메이션 종료					CState_War_Jump_Fall
									마우스 오른쪽(강공)				CState_War_Atk_Air_Light_03_NoImpulse
									착지시1							CState_War_Jump_Land
									착지시2							CState_War_Jump_Land_Heavy
									착지시 + 달리기					CState_War_Jump_Land_Run
									H 버튼							CState_War_Horse_Mount_Running


// 점프 중 강공격 - War_Atk_Air_Light_03_NoImpulse 만하자
24	CState_War_Atk_Air_Light_03_NoImpulse	애니메이션 종료			CState_War_Atk_Air_Light_03_Fall
											착지시					CState_War_Atk_Air_Land

25	CState_War_Atk_Air_Light_03_Fall		착지시					CState_War_Atk_Air_Land

26	CState_War_Atk_Air_Land			애니메이션 종료					CState_War_Idle_Combat
									방향키 하나라도 누름			CState_War_Run_Combat


// 1스킬 
27	CState_War_Wrath_BladeGeyser	애니메이션 종료					CState_War_Idle_Combat
									방향키 하나라도 누름			CState_War_Run_Combat



// 콤보 스킬 - 땅가르기
28	CState_War_Atk_EarthSplitter_Charge_Start		애니메이션 종료		CState_War_Atk_EarthSplitter_Charge_Loop	
29	CState_War_Atk_EarthSplitter_Charge_Loop		마우스 오른쪽떼면	CState_War_Atk_EarthSplitter_Level1

30	CState_War_Atk_EarthSplitter_Level1				애니메이션 종료		CState_War_Idle_Combat
													방향키 하나라도 누름	CState_War_Run_Combat

// 콤보 스킬 - 풍차돌리기
31	CState_War_Atk_Flamebrand_Start		애니메이션 종료		CState_War_Atk_Flamebrand

32	CState_War_Atk_Flamebrand			Max 시간지나거나 오른쪽 마우스 떼면		CState_War_Atk_Flamebrand_End

33	CState_War_Atk_Flamebrand_End		애니메이션 종료			CState_War_Idle_Combat
										hore방향키 하나라도 누름	CState_War_Run_Combat


// 대쉬
34	CState_War_DashTo_F					애니메이션 종료			CState_War_Idle_Combat
		
-----------------------------------------------------------------------------------*/



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

// -------------------------------------------------
// #6
// [State] CState_War_Run_Combat
// [Infom] 칼들고 뛰는 상태
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
// [Infom] 약공1
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
// [Infom] 약공2
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
// [Infom] 약공3
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
// [Infom] 약공4
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
// [Infom] 강공1
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
// [Infom] 강공2
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
// [Infom] 강공1
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
// [Infom] 무기안들고 점프
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
// [Infom] 무기안들고 점프중
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
// [Infom] 땋닿1
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
// [Infom] 땋닿2
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
// [Infom] 땋닿뛰
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
// [Infom] 칼들고 점프
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
// [Infom] 칼들고 점프중
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
// [Infom] 칼들고 착지
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
// [Infom] 칼들고 착지하면서 뛰기
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
// [Infom] 더블점프 
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
// [Infom] 점프중 강공격
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
// [Infom] 점프 공격 중
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
// [Infom] 점프 공격후 착지
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
// [Infom] 1번 스킬
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
// [Infom] G스킬 - 땅 기모으기 시작
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
// [Infom] G스킬 - 땅 기모으는 중
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
// [Infom] G스킬 - 땅 기모으기 끝. 발사 
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
// [Infom] G스킬 - 불풍차 돌리기
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
// [Infom] G스킬 - 불풍차 돌리는 중
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
// [Infom] G스킬 - 불풍차 돌리기 끝
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
// [Infom] 대쉬
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
// [Infom] 앞에서 충돌박았다. 뒤로 가진다.
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
// [Infom] 뒤에서 충돌박았다. 앞으로 가진다.
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
// [Infom] 왼쪽에서 박았다. 오른쪽으로 밀려난다.
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
// [Infom] 앞에서 충돌박았다. 뒤로 가진다.
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
// [Infom] FallenDog 처형모션
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
// 말타기, 말내리기
1	CState_War_Horse_Mount_Standing		애니메이션 종료				CState_War_Horse_Idle

2	CState_War_Horse_Mount_Running		애니메이션 종료				CState_War_Horse_Gallop

3	CState_War_Horse_Dismount			애니메이션 종료				CState_War_Horse_Jump_Land_Heavy


// 아이들
4	CState_War_Horse_Idle				H 버튼						CState_War_Horse_Dismount
										방향키하나라도				CState_War_Horse_Gallop
// 달리기
5	CState_War_Horse_Gallop				방향키하나라도안누르면		CState_War_Horse_Idle	
										LSHIFT						CState_War_Horse_Gallop_Fast_Start
										H 버튼						CState_War_Horse_Dismount

6	CState_War_Horse_Gallop_Fast_Start	애니메이션 종료				CState_War_Horse_Gallop_Fast

7	CState_War_Horse_Gallop_Fast		LSHIFT 뗀경우				CState_War_Horse_Gallop
										방향키하나라도안누르면		CState_War_Horse_Stop

8	CState_War_Horse_Stop				애니메이션 종료				CState_War_Horse_Idle

9	CState_War_Horse_Jump_Land_Heavy	애니메이션 종료				CState_War_Idle
										방향키 하나라도 누름		CState_War_Run

										

--------------------------------------------------------------------------------*/


#define WAR_SPEED 5.0f
#define WAR_NO_WEAPON_SPEED 7.f
#define RUIN_SPEED 10.f
#define RUIN_SHIFT_SPEED 18.f

// -------------------------------------------------
// #Horse1
// [State] CState_War_Horse_Mount_Standing
// [Infom] 서있는 상태에서 말타기 시작 
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
// [Infom] 달리는 상태에서 말타기 버튼 누름
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
// [Infom] 말타기 종료
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
// [Infom] 기본 상태
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
// [Infom] 기본 달리기 
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
// [Infom] LSHIFT 달리기 시작 
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
// [Infom] LSHIFT 달리기
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
// [Infom] LSHIFT 달리기다가 멈추는 동작
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
// [Infom] 말내리기 이후 착지
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
