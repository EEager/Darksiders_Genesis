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
1									피격시							넉백 또는 이건 랜덤하게? 





* 
*	Finite State Machine
* 
[#]	[State]							[Event]							[ToState]
= = = = = = == = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// 기본 움직임
1	CState_War_Idle					근처적,R,대쉬					CState_War_Idle_to_Idle_Combat
									방향키 하나라도 누름			CState_War_Run
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 ToDo

	
2	CState_War_Run					방향키 하나도 안 누름			CState_War_Idle
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 ToDo

	
3	CState_War_Idle_to_Idle_Combat  애니메이션 종료					CState_War_Idle_Combat
									방향키 하나라도 누름			CState_War_Run_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 ToDo

	
4	CState_War_Idle_Combat			방향키 하나라도 누름			CState_War_Run_Combat
									4초간 이벤트없음				CState_War_Idle_Combat_to_Idle
									R버튼							CState_War_Idle_Combat_to_Idle
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 ToDo

5	CState_War_Idle_Combat_to_Idle  애니메이션 종료					CState_War_Idle
									방향키 하나라도 누름			CState_War_Run_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_01 
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 ToDo

	
6	CState_War_Run_Combat			방향키 하나도 안 누름			CState_War_Idle_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_01
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_01 ToDo


// 약공
7	CState_War_Atk_Light_01			애니메이션 종료					CState_War_Idle_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_02

8	CState_War_Atk_Light_02			애니메이션 종료					CState_War_Idle_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_02

9	CState_War_Atk_Light_03			애니메이션 종료					CState_War_Idle_Combat
									마우스 왼쪽(약공)				CState_War_Atk_Light_04

10	CState_War_Atk_Light_04			애니메이션 종료					CState_War_Idle_Combat
									마우스 오른쪽(강공)				G스킬  ToDo


// 강공
11	CState_War_Atk_Heavy_01			애니메이션 종료					CState_War_Idle_Combat
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_02

12	CState_War_Atk_Heavy_02			애니메이션 종료					CState_War_Idle_Combat
									마우스 오른쪽(강공)				CState_War_Atk_Heavy_03

13	CState_War_Atk_Heavy_03			애니메이션 종료					CState_War_Idle_Combat
									마우스 오른쪽(강공)				G스킬

// 일반 점프
14	CState_War_Jump					애니메이션 종료					CState_War_Jump_Fall
									스페이스(점프)					CState_War_Jump_Double

15	CState_War_Jump_Double			애니메이션 종료					CState_War_Jump_Fall

16	CState_War_Jump_Fall			땅에 닿으면1					CState_War_Jump_Land
									땅에 닿으면2					CState_War_Jump_Land_Heavy
									땅에 닿으면서 달리기			CState_War_Jump_Land_Run

17	CState_War_Jump_Land			애니메이션 종료					CState_War_Idle
18	CState_War_Jump_Land_Heavy		애니메이션 종료					CState_War_Idle
19	CState_War_Jump_Land_Run		애니메이션 종료					CState_War_Idle





// Combat 점프
20	CState_War_Jump_Combat			애니메이션 종료					CState_War_Jump_Fall_Combat
21	CState_War_Jump_Fall_Combat		땅에 닿으면						CState_War_Jump_Combat_Land
22	



// 점공 


// G스킬
??	CState_War_Atk_EarthSplitter_Level1			애니메이션 종료					CState_War_Idle_Combat
												마우스 오른쪽(강공)				CState_War_Atk_EarthSplitter_Level1
									

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

public:
	virtual void Free() final;
};