#include "stdafx.h"
#include "State_War.h"

// Headers
#include "GameObject.h"
#include "Input_Device.h"
#include "StateMachine.h"
#include "Transform.h"
#include "Model.h"
#include "..\Public\State_War.h"
#include "War.h"


// 
// Global War State Machine in War.cpp
// 
CStateMachine* g_pWar_State_Context;
CModel* g_pWar_Model_Context;
CTransform* g_pWar_Transform_Context;


/* ------------------------------------------------------------------------------
*
*	Global FSM 
* 
----------------------------------------------------------------------------------*/


// -------------------------------------------------
// Global #1
// [State] CGlobal_State_War
// [Infom] 글로벌 이벤트 체크
// -------------------------------------------------
CGlobal_State_War::CGlobal_State_War()
{
	m_pStateName = "CGlobal_State_War";
}

void CGlobal_State_War::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
}

void CGlobal_State_War::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 죽었다 
	// [State]  -> CState_War_Death
	if (CInput_Device::GetInstance()->Key_Down(DIK_B))
	{
		g_pWar_State_Context->ChangeState(CState_War_Death::GetInstance());
		return;
	}
}

void CGlobal_State_War::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CGlobal_State_War::Free()
{
}

// -------------------------------------------------
// Global #2
// [State] CState_War_Death
// [Infom] 죽었다
// -------------------------------------------------
CState_War_Death::CState_War_Death()
{
	m_pStateName = "CState_War_Death";
}

void CState_War_Death::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Death", false);
}

void CState_War_Death::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Death"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
}

void CState_War_Death::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Death::Free()
{
}









/* ------------------------------------------------------------------------------
*
*	FSM
*
----------------------------------------------------------------------------------*/

// -------------------------------------------------
// #1
// [State] CState_War_Idle
// [Infom] 무기 안들고 가만히 서있는 상태
// -------------------------------------------------
CState_War_Idle::CState_War_Idle()
{
	m_pStateName = "CState_War_Idle";
}

void CState_War_Idle::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Idle");
}

void CState_War_Idle::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 1번 스킬
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}


	// [Event] 스페이스 (점프)
	// [State]  -> CState_War_Jump 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump::GetInstance());
		return;
	}

	// [Event] 마우스 오른쪽(강공)
	// [State]  -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State]  -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}


	// [Event] 근처에 적이 있을시, R 버튼 눌렀을때 
	// [State]  -> CState_War_Idle_to_Idle_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_R))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_to_Idle_Combat::GetInstance());
		return;
	}


	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run::GetInstance());
		return;
	}


}

void CState_War_Idle::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Idle::Free()
{
}



// -------------------------------------------------
// #2
// [State] CState_War_Run
// [Infom] 무기 안들꼬 뛰는 상태 
// -------------------------------------------------
CState_War_Run::CState_War_Run()
{
	m_pStateName = "CState_War_Run";
}

void CState_War_Run::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();

	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Run_F");
}

void CState_War_Run::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 1번 스킬
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}

	// [Event] 스페이스 (점프)
	// [State]  -> CState_War_Jump 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump::GetInstance());
		return;
	}

	// [Event] 마우스 오른쪽(강공)
	// [State]  -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}


	// [Event] 방향키 하나라도 안 누를시
	// [State] -> CState_War_Idle ()
	bool isKeyDown = false;
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);


	if (isKeyDown == false)
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
}

void CState_War_Run::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Run::Free()
{
	Exit();
}


// -------------------------------------------------
// #3
// [State] CState_War_Idle_to_Idle_Combat
// [Infom] 아이들에서 검아이들까지 가는 중간 단계
// -------------------------------------------------
CState_War_Idle_to_Idle_Combat::CState_War_Idle_to_Idle_Combat()
{
	m_pStateName = "CState_War_Idle_to_Idle_Combat";
}

void CState_War_Idle_to_Idle_Combat::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();


	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Idle_to_Idle_Combat", false/*loop false이어야, Idle_Combat이 끝날때 마지막 상태를 유지한다 */);
}

void CState_War_Idle_to_Idle_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 1번 스킬
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}

	// [Event] 스페이스 (점프)
	// [State] -> CState_War_Jump_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat::GetInstance());
		return;
	}

	// [Event] 마우스 오른쪽(강공)
	// [State] -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}


	// [Event] 방향키 하나라도 누르게된다면
	// [State] -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}


	// [Event] CState_War_Idle_Combat 첫 번째 애니메이션 끝난경우
	// [State] -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Idle_to_Idle_Combat"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}

}

void CState_War_Idle_to_Idle_Combat::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Idle_to_Idle_Combat::Free()
{
}

// -------------------------------------------------
// #4
// [State] CState_War_Idle_Combat
// [Infom] 칼 Idle
// -------------------------------------------------
CState_War_Idle_Combat::CState_War_Idle_Combat()
{
	m_pStateName = "CState_War_Idle_Combat";
}

void CState_War_Idle_Combat::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();

	// Loop 애니메이션
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Idle_Combat");
}

void CState_War_Idle_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 1번 스킬
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}


	// [Event] 스페이스 (점프)
	// [State] -> CState_War_Jump_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat::GetInstance());
		return;
	}

	// [Event] 마우스 오른쪽(강공)
	// [State] -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] 방향키 하나라도 누르게된다면
	// [State] -> CState_War_Run
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	// [Event] Idle_Combat에서 4초간 아무것도 안하면.
	// [State] -> CState_War_Idle_Combat_to_Idle
	m_fIdle_Combat_time += fTimeDelta;
	if (m_fIdle_Combat_time > 4.f)
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat_to_Idle::GetInstance());
		m_fIdle_Combat_time = 0.f;
		return;
	}


	// [Event] R 버튼 눌렀을때 
	// [State] -> CState_War_Idle_Combat_to_Idle 
	if (CInput_Device::GetInstance()->Key_Down(DIK_R))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat_to_Idle::GetInstance());
		return;
	}


	// [Event] 
	// [State] -> Light_Atk

	return;

}

void CState_War_Idle_Combat::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	m_fIdle_Combat_time = 0.f;
}

void CState_War_Idle_Combat::Free()
{
}


// -------------------------------------------------
// #5
// [State] CState_War_Idle_Combat_to_Idle
// [Infom] combat idle -> idle 로 가는 상태 
// -------------------------------------------------
CState_War_Idle_Combat_to_Idle::CState_War_Idle_Combat_to_Idle()
{
	m_pStateName = "CState_War_Idle_Combat_to_Idle";
}

void CState_War_Idle_Combat_to_Idle::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Idle_Combat_to_Idle", false);
}

void CState_War_Idle_Combat_to_Idle::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 1번 스킬
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}

	// [Event] 스페이스 (점프)
	// [State] -> CState_War_Jump_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat::GetInstance());
		return;
	}

	// [Event] 마우스 오른쪽(강공)
	// [State] -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] 방향키 하나라도 누르게된다면
	// [State] -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}


	// [Event] CState_War_Idle_Combat_to_Idle 애니메이션 끝나면
	// [State] -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Idle_Combat_to_Idle"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
}

void CState_War_Idle_Combat_to_Idle::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Idle_Combat_to_Idle::Free()
{
}


// -------------------------------------------------
// #6
// [State] CState_War_Run_Combat
// [Infom] 칼들고 뛰는 상태
// -------------------------------------------------
CState_War_Run_Combat::CState_War_Run_Combat()
{
	m_pStateName = "CState_War_Run_Combat";
}

void CState_War_Run_Combat::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Run_F_Combat");
}

void CState_War_Run_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 1번 스킬
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}


	// [Event] 스페이스 (점프)
	// [State] -> CState_War_Jump_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat::GetInstance());
		return;
	}

	// [Event] 마우스 오른쪽(강공)
	// [State] -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] 방향키 하나도 안누를시, 칼Idle로 천이
	// [State] -> CState_War_Idle_Combat
	bool isKeyDown = false;
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);

	if (isKeyDown == false)
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Run_Combat::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Run_Combat::Free()
{
}

// -------------------------------------------------
// #7
// [State] CState_War_Atk_Light_01
// [Infom] 약공1
// -------------------------------------------------
CState_War_Atk_Light_01::CState_War_Atk_Light_01()
{
	m_pStateName = "CState_War_Atk_Light_01";
}

void CState_War_Atk_Light_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Light_01", false);
}

void CState_War_Atk_Light_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Atk_Light_01 -> CState_War_Atk_Light_02
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_02::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Light_01 애니메이션 끝나면
	// [State] CState_War_Atk_Light_01 -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Light_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Light_01::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Light_01::Free()
{
}


// -------------------------------------------------
// #8
// [State] CState_War_Atk_Light_02
// [Infom] 약공2
// -------------------------------------------------
CState_War_Atk_Light_02::CState_War_Atk_Light_02()
{
	m_pStateName = "CState_War_Atk_Light_02";
}

void CState_War_Atk_Light_02::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Light_02", false);
}

void CState_War_Atk_Light_02::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Atk_Light_02 -> CState_War_Atk_Light_03
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_03::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Light_02 애니메이션 끝나면
	// [State] CState_War_Atk_Light_02 -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Light_02"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Light_02::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Light_02::Free()
{
}

// -------------------------------------------------
// #9
// [State] CState_War_Atk_Light_03
// [Infom] 약공3
// -------------------------------------------------
CState_War_Atk_Light_03::CState_War_Atk_Light_03()
{
	m_pStateName = "CState_War_Atk_Light_03";
}

void CState_War_Atk_Light_03::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Light_03", false);
}

void CState_War_Atk_Light_03::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Atk_Light_03 -> CState_War_Atk_Light_04
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_04::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Light_03 애니메이션 끝나면
	// [State] CState_War_Atk_Light_03 -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Light_03"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Light_03::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Light_03::Free()
{
}

// -------------------------------------------------
// #10
// [State] CState_War_Atk_Light_04
// [Infom] 약공4
// -------------------------------------------------
CState_War_Atk_Light_04::CState_War_Atk_Light_04()
{
	m_pStateName = "CState_War_Atk_Light_04";
}

void CState_War_Atk_Light_04::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Light_04", false);
}

void CState_War_Atk_Light_04::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	//// [Event] 마우스 오른쪽(강공)
	//// [State]  -> G스킬
	//if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Atk_Light_04::GetInstance());
	//	return;
	//}

	// [Event] CState_War_Atk_Light_04 애니메이션 끝나면
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Light_04"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Light_04::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Light_04::Free()
{
}

// -------------------------------------------------
// #11
// [State] CState_War_Atk_Heavy_01
// [Infom] 강공1
// -------------------------------------------------
CState_War_Atk_Heavy_01::CState_War_Atk_Heavy_01()
{
	m_pStateName = "CState_War_Atk_Heavy_01";
}

void CState_War_Atk_Heavy_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Heavy_01", false);
}

void CState_War_Atk_Heavy_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		m_bChargeStart = true;
	}

	if (m_bChargeStart)
	{
		// 누르고 있는 경우
		if (CInput_Device::GetInstance()->Mouse_Pressing(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
		{
			m_fBntPressTime += fTimeDelta;
#ifdef _DEBUG
			printf("m_fBntPressTime(%lf)\n", m_fBntPressTime);
#endif
			if (m_fBntPressTime > .5f) // 1초 이상 차징하면 G스킬로
			{
				g_pWar_State_Context->ChangeState(CState_War_Atk_EarthSplitter_Charge_Start::GetInstance());
				return;
			}
		}
		// 뗀 경우
		else
		{
			if (m_fBntPressTime < 0.5f) // 바로 뗀다면 콤보 스킬로
			{
				g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_02::GetInstance());
				return;
			}
		}
	}


	// [Event] CState_War_Atk_Heavy_01 애니메이션 끝나면
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Heavy_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Heavy_01::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	m_fBntPressTime = 0.f;
	m_bChargeStart = false;
}

void CState_War_Atk_Heavy_01::Free()
{
}

// -------------------------------------------------
// #12
// [State] CState_War_Atk_Heavy_02
// [Infom] 강공2
// -------------------------------------------------
CState_War_Atk_Heavy_02::CState_War_Atk_Heavy_02()
{
	m_pStateName = "CState_War_Atk_Heavy_02";
}

void CState_War_Atk_Heavy_02::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Heavy_02", false);
}

void CState_War_Atk_Heavy_02::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	// [Event] 마우스 오른쪽(강공)
	// [State]  -> GCState_War_Atk_Heavy_03
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_03::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Heavy_02 애니메이션 끝나면
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Heavy_02"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Heavy_02::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Heavy_02::Free()
{
}

// -------------------------------------------------
// #13
// [State] CState_War_Atk_Heavy_03
// [Infom] 강공3
// -------------------------------------------------
CState_War_Atk_Heavy_03::CState_War_Atk_Heavy_03()
{
	m_pStateName = "CState_War_Atk_Heavy_03";
}

void CState_War_Atk_Heavy_03::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Heavy_03", false);
}

void CState_War_Atk_Heavy_03::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	//// [Event] 마우스 오른쪽(강공)
	//// [State]  -> G스킬
	//if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_02::GetInstance());
	//	return;
	//}

	// [Event] CState_War_Atk_Heavy_03 애니메이션 끝나면
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Heavy_03"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Heavy_03::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Heavy_03::Free()
{
}

// -------------------------------------------------
// #14
// [State] CState_War_Jump
// [Infom] 무기안들고 점프
// -------------------------------------------------
CState_War_Jump::CState_War_Jump()
{
	m_pStateName = "CState_War_Jump";
}

void CState_War_Jump::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump", false);
}

void CState_War_Jump::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 마우스 오른쪽(강공)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	// [Event] 스페이스 (점프)
	// [State]  -> CState_War_Jump_Double
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Double::GetInstance());
		return;
	}

	// [Event] CState_War_Jump 애니메이션 끝나면
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Fall::GetInstance());
		return;
	}
}

void CState_War_Jump::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump::Free()
{
}

// -------------------------------------------------
// #15
// [State] CState_War_Jump_Fall
// [Infom] 무기안들고 점프중
// -------------------------------------------------
CState_War_Jump_Fall::CState_War_Jump_Fall()
{
	m_pStateName = "CState_War_Jump_Fall";
}

void CState_War_Jump_Fall::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Fall");
}

void CState_War_Jump_Fall::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 마우스 오른쪽(강공)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	m_fFlightTime += fTimeDelta;

	// [Event] 스페이스 (점프)
	// [State]  -> CState_War_Jump_Double
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Double::GetInstance());
		return;
	}

	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		// [Event] 땋에 닿으면서 방향키 누르고 있는경우 
		// [State]  -> CState_War_Jump_Land_Run
		if (0)
		{
			g_pWar_State_Context->ChangeState(CState_War_Jump_Land_Run::GetInstance());
			return;
		}

		if (m_fFlightTime <= 2.f)
		{
			// [Event] 땅닿1
			// [State]  -> CState_War_Jump_Land
			g_pWar_State_Context->ChangeState(CState_War_Jump_Land::GetInstance());
			return;
		}
		else
		{
			// [Event] 땅닿2 - 일정높이 이상되면 땋닿2임 
			// [State]  -> CState_War_Jump_Land
			g_pWar_State_Context->ChangeState(CState_War_Jump_Land_Heavy::GetInstance());
			return;
		}
	}
}

void CState_War_Jump_Fall::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	m_fFlightTime = 0.f;
}

void CState_War_Jump_Fall::Free()
{
}

// -------------------------------------------------
// #16
// [State] CState_War_Jump_Land
// [Infom] 땋닿1
// -------------------------------------------------
CState_War_Jump_Land::CState_War_Jump_Land()
{
	m_pStateName = "CState_War_Jump_Land";
}

void CState_War_Jump_Land::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Land", false);
}

void CState_War_Jump_Land::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run::GetInstance());
		return;
	}

	// [Event] CState_War_Jump_Land 애니메이션 끝나면
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Land"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
}

void CState_War_Jump_Land::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump_Land::Free()
{
}

// -------------------------------------------------
// #17
// [State] CState_War_Jump_Land_Heavy
// [Infom] 땋닿2
// -------------------------------------------------
CState_War_Jump_Land_Heavy::CState_War_Jump_Land_Heavy()
{
	m_pStateName = "CState_War_Jump_Land_Heavy";
}

void CState_War_Jump_Land_Heavy::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not, Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Land_Heavy", false);
}

void CState_War_Jump_Land_Heavy::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run::GetInstance());
		return;
	}

	// [Event] CState_War_Jump_Land_Heavy 애니메이션 끝나면
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Land_Heavy"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
}

void CState_War_Jump_Land_Heavy::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump_Land_Heavy::Free()
{
}

// -------------------------------------------------
// #18
// [State] CState_War_Jump_Land_Run
// [Infom] 땋닿뛰
// -------------------------------------------------
CState_War_Jump_Land_Run::CState_War_Jump_Land_Run()
{
	m_pStateName = "CState_War_Jump_Land_Run";
}

void CState_War_Jump_Land_Run::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not, Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Land_Run", false);
}

void CState_War_Jump_Land_Run::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] CState_War_Jump_Land_Run 애니메이션 끝나면
	// [State]  -> CState_War_Run
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Land_Run"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Run::GetInstance());
		return;
	}
}

void CState_War_Jump_Land_Run::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump_Land_Run::Free()
{
}


// -------------------------------------------------
// #19
// [State] CState_War_Jump_Combat
// [Infom] 칼들고 점프
// -------------------------------------------------
CState_War_Jump_Combat::CState_War_Jump_Combat()
{
	m_pStateName = "CState_War_Jump_Combat";
}

void CState_War_Jump_Combat::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not, Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Combat", false);
}

void CState_War_Jump_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 마우스 오른쪽(강공)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	// [Event] 스페이스 (점프)
	// [State]  -> CState_War_Jump_Double
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Double::GetInstance());
		return;
	}

	// [Event] CState_War_Jump_Combat 애니메이션 끝나면
	// [State]  -> CState_War_Jump_Fall_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Combat"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Fall_Combat::GetInstance());
		return;
	}
}

void CState_War_Jump_Combat::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump_Combat::Free()
{
}

// -------------------------------------------------
// #20
// [State] CState_War_Jump_Fall_Combat
// [Infom] 칼들고 점프중
// -------------------------------------------------
CState_War_Jump_Fall_Combat::CState_War_Jump_Fall_Combat()
{
	m_pStateName = "CState_War_Jump_Fall_Combat";
}

void CState_War_Jump_Fall_Combat::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Fall_Combat", true);
}

void CState_War_Jump_Fall_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] 마우스 오른쪽(강공)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	// [Event] 스페이스 (점프)
	// [State]  -> CState_War_Jump_Double
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Double::GetInstance());
		return;
	}

	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		// [Event] 땋에 닿으면서 방향키 누르고 있는경우 
		// [State]  -> CState_War_Jump_Combat_Land_Run
		if (0)
		{
			g_pWar_State_Context->ChangeState(CState_War_Jump_Combat_Land_Run::GetInstance());
			return;
		}

		// [Event] 땅닿
		// [State]  -> CState_War_Jump_Combat_Land
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat_Land::GetInstance());
		return;
	}
}

void CState_War_Jump_Fall_Combat::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump_Fall_Combat::Free()
{
}

// -------------------------------------------------
// #21
// [State] CState_War_Jump_Combat_Land
// [Infom] 칼들고 착지
// -------------------------------------------------
CState_War_Jump_Combat_Land::CState_War_Jump_Combat_Land()
{
	m_pStateName = "CState_War_Jump_Combat_Land";
}

void CState_War_Jump_Combat_Land::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not, Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Combat_Land", false);
}

void CState_War_Jump_Combat_Land::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Combat_Land"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}

}

void CState_War_Jump_Combat_Land::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump_Combat_Land::Free()
{
}

// -------------------------------------------------
// #22
// [State] CState_War_Jump_Combat_Land_Run
// [Infom] 칼들고 착지하면서 뛰기
// -------------------------------------------------
CState_War_Jump_Combat_Land_Run::CState_War_Jump_Combat_Land_Run()
{
	m_pStateName = "CState_War_Jump_Combat_Land_Run";
}

void CState_War_Jump_Combat_Land_Run::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not, Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Combat_Land_Run", false);
}

void CState_War_Jump_Combat_Land_Run::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Run_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Combat_Land_Run"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

}

void CState_War_Jump_Combat_Land_Run::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump_Combat_Land_Run::Free()
{
}


// -------------------------------------------------
// #23
// [State] CState_War_Jump_Double
// [Infom] 더블점프 
// -------------------------------------------------
CState_War_Jump_Double::CState_War_Jump_Double()
{
	m_pStateName = "CState_War_Jump_Double";
}

void CState_War_Jump_Double::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not, Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Double", false);
}

void CState_War_Jump_Double::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Jump_Fall
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Double"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Fall::GetInstance());
		return;
	}

}

void CState_War_Jump_Double::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Jump_Double::Free()
{
}


// -------------------------------------------------
// #24
// [State] CState_War_Atk_Air_Light_03_NoImpulse
// [Infom] 점프중 강공격
// -------------------------------------------------
CState_War_Atk_Air_Light_03_NoImpulse::CState_War_Atk_Air_Light_03_NoImpulse()
{
	m_pStateName = "CState_War_Atk_Air_Light_03_NoImpulse";
}

void CState_War_Atk_Air_Light_03_NoImpulse::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not, Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Air_Light_03_NoImpulse", false);
}

void CState_War_Atk_Air_Light_03_NoImpulse::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Atk_Air_Light_03_Fall
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Air_Light_03_NoImpulse"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_Fall::GetInstance());
		return;
	}

}

void CState_War_Atk_Air_Light_03_NoImpulse::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Air_Light_03_NoImpulse::Free()
{
}


// -------------------------------------------------
// #25
// [State] CState_War_Atk_Air_Light_03_Fall
// [Infom] 점프 공격 중
// -------------------------------------------------
CState_War_Atk_Air_Light_03_Fall::CState_War_Atk_Air_Light_03_Fall()
{
	m_pStateName = "CState_War_Atk_Air_Light_03_Fall";
}

void CState_War_Atk_Air_Light_03_Fall::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Air_Light_03_Fall");
}

void CState_War_Atk_Air_Light_03_Fall::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		// [Event] 착지시
		// [State]  -> CState_War_Atk_Air_Land
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Land::GetInstance());
		return;
	}

}

void CState_War_Atk_Air_Light_03_Fall::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Air_Light_03_Fall::Free()
{
}


// -------------------------------------------------
// #26
// [State] CState_War_Atk_Air_Land
// [Infom] 점프 공격후 착지
// -------------------------------------------------
CState_War_Atk_Air_Land::CState_War_Atk_Air_Land()
{
	m_pStateName = "CState_War_Atk_Air_Land";
}

void CState_War_Atk_Air_Land::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Air_Land", false);
}

void CState_War_Atk_Air_Land::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Air_Land"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}

}

void CState_War_Atk_Air_Land::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_Air_Land::Free()
{
}

// -------------------------------------------------
// #27
// [State] CState_War_Wrath_BladeGeyser
// [Infom] 1번 스킬
// -------------------------------------------------
CState_War_Wrath_BladeGeyser::CState_War_Wrath_BladeGeyser()
{
	m_pStateName = "CState_War_Wrath_BladeGeyser";
}

void CState_War_Wrath_BladeGeyser::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Wrath_BladeGeyser", false);
}

void CState_War_Wrath_BladeGeyser::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Run_Combat
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
		return;
	}

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Wrath_BladeGeyser"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}

}

void CState_War_Wrath_BladeGeyser::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Wrath_BladeGeyser::Free()
{
}





// -------------------------------------------------
// #28
// [State] CState_War_Atk_EarthSplitter_Charge_Start
// [Infom] G스킬 - 땅 기모으기 시작
// -------------------------------------------------
CState_War_Atk_EarthSplitter_Charge_Start::CState_War_Atk_EarthSplitter_Charge_Start()
{
	m_pStateName = "CState_War_Atk_EarthSplitter_Charge_Start";
}

void CState_War_Atk_EarthSplitter_Charge_Start::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	// Not Loop
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_EarthSplitter_Charge_Start", false);
}

void CState_War_Atk_EarthSplitter_Charge_Start::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_War_Atk_EarthSplitter_Charge_Loop
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_EarthSplitter_Charge_Start"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_EarthSplitter_Charge_Loop::GetInstance());
		return;
	}

}

void CState_War_Atk_EarthSplitter_Charge_Start::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_EarthSplitter_Charge_Start::Free()
{
}


// -------------------------------------------------
// #29
// [State] CState_War_Atk_EarthSplitter_Charge_Loop
// [Infom] G스킬 - 땅 기모으는 중
// -------------------------------------------------
CState_War_Atk_EarthSplitter_Charge_Loop::CState_War_Atk_EarthSplitter_Charge_Loop()
{
	m_pStateName = "CState_War_Atk_EarthSplitter_Charge_Loop";
}

void CState_War_Atk_EarthSplitter_Charge_Loop::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_EarthSplitter_Charge_Loop");// Loop
}

void CState_War_Atk_EarthSplitter_Charge_Loop::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 마우스 오른쪽 떼면
	// [State]  -> CState_War_Atk_EarthSplitter_Level1
	if (CInput_Device::GetInstance()->Mouse_Pressing(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON) == false)
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_EarthSplitter_Level1::GetInstance());
		return;
	}
}

void CState_War_Atk_EarthSplitter_Charge_Loop::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_EarthSplitter_Charge_Loop::Free()
{
}

// -------------------------------------------------
// #30
// [State] CState_War_Atk_EarthSplitter_Level1
// [Infom] G스킬 - 땅 기모으기 끝. 발사 
// -------------------------------------------------
CState_War_Atk_EarthSplitter_Level1::CState_War_Atk_EarthSplitter_Level1()
{
	m_pStateName = "CState_War_Atk_EarthSplitter_Level1";
}

void CState_War_Atk_EarthSplitter_Level1::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_EarthSplitter_Level1", false);// Not Loop
}

void CState_War_Atk_EarthSplitter_Level1::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_EarthSplitter_Level1"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}

}

void CState_War_Atk_EarthSplitter_Level1::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Atk_EarthSplitter_Level1::Free()
{
}