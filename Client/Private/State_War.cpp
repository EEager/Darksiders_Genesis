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

	// [Event] 마우스 오른쪽(강공)
	// [State] CState_War_Idle -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Idle -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}


	// [Event] 근처에 적이 있을시, R 버튼 눌렀을때 
	// [State] CState_War_Idle -> CState_War_Idle_to_Idle_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_R))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_to_Idle_Combat::GetInstance());
		return;
	}


	// [Event] 방향키 하나라도 누르게된다면
	// [State] CState_War_Idle -> CState_War_Run
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

	// [Event] 마우스 오른쪽(강공)
	// [State] CState_War_Idle -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Run -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}


	// [Event] 방향키 하나라도 안 누를시
	// [State] CState_War_Run -> CState_War_Idle ()
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

	// [Event] 마우스 오른쪽(강공)
	// [State] CState_War_Idle -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Idle_to_Idle_Combat -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}


	// [Event] 방향키 하나라도 누르게된다면
	// [State] CState_War_Idle_to_Idle_Combat -> CState_War_Run_Combat
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
	// [State] CState_War_Idle_Combat -> CState_War_Idle_Combat
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

	// [Event] 마우스 오른쪽(강공)
	// [State] CState_War_Idle -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Idle_Combat -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] 방향키 하나라도 누르게된다면
	// [State] CState_War_Idle -> CState_War_Run
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
	// [State] CState_War_Idle_Combat -> CState_War_Idle_Combat_to_Idle
	m_fIdle_Combat_time += fTimeDelta;
	if (m_fIdle_Combat_time > 4.f)
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat_to_Idle::GetInstance());
		m_fIdle_Combat_time = 0.f;
		return;
	}


	// [Event] R 버튼 눌렀을때 
	// [State] CState_War_Idle_Combat -> CState_War_Idle_Combat_to_Idle 
	if (CInput_Device::GetInstance()->Key_Down(DIK_R))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat_to_Idle::GetInstance());
		return;
	}


	// [Event] 
	// [State] Idle_Combat -> Light_Atk

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

	// [Event] 마우스 오른쪽(강공)
	// [State] CState_War_Idle -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Idle_Combat_to_Idle -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] 방향키 하나라도 누르게된다면
	// [State] CState_War_Idle_Combat_to_Idle -> CState_War_Run_Combat
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
	// [State] CState_War_Idle_Combat_to_Idle -> CState_War_Idle
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

	// [Event] 마우스 오른쪽(강공)
	// [State] CState_War_Idle -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] 마우스 왼쪽(약공)
	// [State] CState_War_Run_Combat -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] 방향키 하나도 안누를시, 칼Idle로 천이
	// [State] CState_War_Run_Combat -> CState_War_Idle_Combat
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

	//// [Event] 마우스 오른쪽(강공)
	//// [State] CState_War_Atk_Light_04 -> G스킬
	//if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Atk_Light_04::GetInstance());
	//	return;
	//}

	// [Event] CState_War_Atk_Light_04 애니메이션 끝나면
	// [State] CState_War_Atk_Light_04 -> CState_War_Idle_Combat
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

	// [Event] 마우스 오른쪽(강공)
	// [State] CState_War_Atk_Heavy_01 -> GCState_War_Atk_Heavy_02
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_02::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Heavy_01 애니메이션 끝나면
	// [State] CState_War_Atk_Heavy_01 -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Heavy_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Heavy_01::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
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

	// [Event] 마우스 오른쪽(강공)
	// [State] CState_War_Atk_Heavy_02 -> GCState_War_Atk_Heavy_03
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_03::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Heavy_02 애니메이션 끝나면
	// [State] CState_War_Atk_Heavy_02 -> CState_War_Idle_Combat
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

	//// [Event] 마우스 오른쪽(강공)
	//// [State] CState_War_Atk_Heavy_03 -> G스킬
	//if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_02::GetInstance());
	//	return;
	//}

	// [Event] CState_War_Atk_Heavy_03 애니메이션 끝나면
	// [State] CState_War_Atk_Heavy_03 -> CState_War_Idle_Combat
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