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
#include "PipeLine.h"


// 
// Global War State Machine in War.cpp
// 
CWar* g_pWar;
CStateMachine* g_pWar_State_Context;
CModel* g_pWar_Model_Context;
CModel* g_pWar_Model_Gauntlet_Context;
CModel* g_pWar_Model_Ruin_Context;
CModel* g_pWar_Model_Sword_Context;
CTransform* g_pWar_Transform_Context;


/* ------------------------------------------------------------------------------
*
*	Global FSM 
* 
----------------------------------------------------------------------------------*/

// -------------------------------------------------
// Global #1
// [State] CGlobal_State_War
// [Infom]  War_Key, 죽음 체크
// -------------------------------------------------

// 이전공격 콤보 이어지는 시간
#define WAR_COMBO_TIME_INIT 2.f
_float g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT;
CState* g_pLatestWarAtkCombo = nullptr;
CState* g_pLatestWarAtkHeavyCombo = nullptr;

// 공격할때는 방향키 움직이게 하지 않는 시간
#define WAR_KEY_LOCK_TIME_CAUSE_OF_ATK 1.5f

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

	// 콤보 지속 시간은 계속 빼자
	g_fWarAtkComboTimeAcc -= fTimeDelta;
	if (g_fWarAtkComboTimeAcc < 0) 
	{
		// 콤보 지속 시간 끝나면 nullptr로 바꾸어 처음 동작에서 시작하도록하자 
		g_pLatestWarAtkCombo = nullptr;
		g_pLatestWarAtkHeavyCombo = nullptr;
		g_fWarAtkComboTimeAcc = 0.f;
	}




	// [Event] 계속 맞고 있다. 
	// [State] HitPower를 계속 감소시키자
	if (g_pWar->m_bHitted)
	{
		g_pWar->m_fHitPower -= 0.01f;
		if (g_pWar->m_fHitPower < 0)
		{
			g_pWar->m_fHitPower = 0.f;
			g_pWar->m_bHitted = false;
		}
	}


	// [Event] 피격당했을 경우 피격상태로 천이
	// [State] m_iHitDir를 보고 어느 피격 모션으로 천이할지 결정한다
	if (g_pWar->m_iHitDir != -1) // g_pWar->m_iHitDir 초기화는 각 피격 상태에서 해주자.
	{
		switch (g_pWar->m_iHitDir)
		{
		case HIT_FROM_FRONT:
			g_pWar_State_Context->ChangeState(CState_War_Impact_From_Front_01::GetInstance());
			break;
		case HIT_FROM_BACK:
			g_pWar_State_Context->ChangeState(CState_War_Impact_From_Back_01::GetInstance());
			break;
		case HIT_FROM_RIGHT:
			g_pWar_State_Context->ChangeState(CState_War_Impact_From_Right_01::GetInstance());
			break;
		case HIT_FROM_LEFT:
			g_pWar_State_Context->ChangeState(CState_War_Impact_From_Left_01::GetInstance());
			break;
		}
		return;
	}


	// [Event] 죽었다
	// [State]  -> CState_War_Death
	if (CInput_Device::GetInstance()->Key_Down(DIK_B)) // TEST
	{
		g_pWar_State_Context->ChangeState(CState_War_Death::GetInstance());
		return;
	}


	if (static_cast<CWar*>(pOwner)->Get_War_On_Ruin_State() == false) // 말타고 있지 않을때 SHIFT는 대쉬이다
	{
		// [Event] 대쉬
		// [State]  -> CState_War_DashTo_F
		if (m_bShiftLockTimeAccStart)
		{
			m_fShiftLockTimeAcc += fTimeDelta;
			if (m_fShiftLockTimeAcc > SHIFT_LOCK_TIME) // 이전에 shift했으면 SHIFT_LOCK_TIME까지는 하지못하게 하자
			{
				m_bShiftLockTimeAccStart = false;
				m_fShiftLockTimeAcc = 0.f;
			}
		}

		if (m_bShiftLockTimeAccStart == false && CInput_Device::GetInstance()->Key_Down(DIK_LSHIFT))
		{
			// ---------------------
			// 보완 : 쉬프트 도중에 방향키를 눌러, 해당 방향으로 쉬프팅 되도록 하자
			unsigned char keyDownCheckBit = 0b0;
			bool isKeyDown = false;
			auto const dirtyCheck = [&isKeyDown, &keyDownCheckBit](_bool b) { isKeyDown |= b; };
			dirtyCheck(g_pWar->KeyCheck(DIK_A, keyDownCheckBit));
			dirtyCheck(g_pWar->KeyCheck(DIK_W, keyDownCheckBit));
			dirtyCheck(g_pWar->KeyCheck(DIK_D, keyDownCheckBit));
			dirtyCheck(g_pWar->KeyCheck(DIK_S, keyDownCheckBit));
			if (isKeyDown)
			{
				g_pWar_Transform_Context->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(g_pWar->GetDegree(keyDownCheckBit)));
			}
			// ---------------------

			m_bShiftLockTimeAccStart = true;
			m_fShiftLockTimeAcc = 0.f;
			g_pWar_State_Context->ChangeState(CState_War_DashTo_F::GetInstance());
			return;
		}

		// Global War_Key
		static_cast<CWar*>(pOwner)->War_Key(fTimeDelta);
	}
	else // 말타고 있는 경우
	{
		// Global War_Key
		static_cast<CWar*>(pOwner)->War_Key(fTimeDelta);
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

	// [Event] H (말타기)	
	// [State]  -> CState_War_Horse_Mount_Standing 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Standing::GetInstance());
		return;
	}

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

	static_cast<CWar*>(pOwner)->Set_Speed(WAR_NO_WEAPON_SPEED);
}

void CState_War_Run::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] H (말타기)	
	// [State]  -> CState_War_Horse_Mount_Running 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Running::GetInstance());
		return;
	}
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
	static_cast<CWar*>(pOwner)->Set_Speed(WAR_SPEED);
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

	// [Event] H (말타기)	
	// [State]  -> CState_War_Horse_Mount_Standing 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Standing::GetInstance());
		return;
	}

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
	// [Event] H (말타기)	
	// [State]  -> CState_War_Horse_Mount_Standing 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Standing::GetInstance());
		return;
	}

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

	// [Event] H (말타기)	
	// [State]  -> CState_War_Horse_Mount_Standing 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Standing::GetInstance());
		return;
	}

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

	// [Event] H (말타기)	
	// [State]  -> CState_War_Horse_Mount_Running 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Running::GetInstance());
		return;
	}

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
		
	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);

	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);

	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Light_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// 다음 콤보가 지정되어있다면 그걸로 상태 변환
	if (g_pLatestWarAtkCombo)
	{
		g_pWar_State_Context->ChangeState(g_pLatestWarAtkCombo);
		return;
	}

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
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
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	m_fTimeAcc = 0.f;

	// Combo
	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT; // 끝에 콤보타임 초기화
	g_pLatestWarAtkCombo = CState_War_Atk_Light_02::GetInstance(); // 다음 콤보를 지정해주자


	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;

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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Light_02::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	m_fTimeAcc = 0.f;


	// 다음 콤보 동작 넣어주자
	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT;
	g_pLatestWarAtkCombo = CState_War_Atk_Light_03::GetInstance();

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Light_03::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	m_fTimeAcc = 0.f;
	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT;
	g_pLatestWarAtkCombo = g_pLatestWarAtkCombo = CState_War_Atk_Light_04::GetInstance();;

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);

	// 0과 널로 만들어서 1콤보로 가게 하자 
	g_fWarAtkComboTimeAcc = 0;


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Light_04::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	m_fTimeAcc = 0.f;

	g_fWarAtkComboTimeAcc = 0.f;
	g_pLatestWarAtkCombo = nullptr;

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Heavy_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// 이전콤보시간이 남아있다면 그 콤보 지정해준 state로 가자
	if (g_pLatestWarAtkHeavyCombo)
	{
		g_pWar_State_Context->ChangeState(g_pLatestWarAtkHeavyCombo);
		return;
	}

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
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
	}

	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		m_bChargeStart = true;
	}

	if (m_bChargeStart)
	{
		// [Event] 오른쪽 마우스 1초 이상 차징하면 콤보 스킬로
		// [State]  -> CState_War_Atk_EarthSplitter_Charge_Start
		// 누르고 있는 경우
		if (CInput_Device::GetInstance()->Mouse_Pressing(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
		{
			m_fBntPressTime += fTimeDelta;
#ifdef _DEBUG
			printf("m_fBntPressTime(%lf)\n", m_fBntPressTime);
#endif
			if (m_fBntPressTime > .5f) // 1초 이상 차징하면 콤보 스킬로
			{
				if (static_cast<CWar*>(pOwner)->Get_GType() == CWar::G_TYPE_FIRE)
					g_pWar_State_Context->ChangeState(CState_War_Atk_Flamebrand_Start::GetInstance());
				else
					g_pWar_State_Context->ChangeState(CState_War_Atk_EarthSplitter_Charge_Start::GetInstance());
				return;
			}
		}
		// [Event] 오른쪽 마우스
		// [State]  -> CState_War_Atk_Heavy_02
		else
		{
			if (m_fBntPressTime < 0.5f) // 바로 뗀다면 다음 구분 동작
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

	m_fTimeAcc = 0.f; // 키락시간 초기화하자

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);

	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT; // 끝에 콤보타임넣어주자
	g_pLatestWarAtkHeavyCombo = CState_War_Atk_Heavy_02::GetInstance();

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Heavy_02::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// 공격후 몇초간 바로 움직이게 하지말자
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
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

	m_fTimeAcc = 0.f; // 키락시간 초기화하자

		// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);

	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT; // 끝에 콤보 시간 초기화시켜주자
	g_pLatestWarAtkHeavyCombo = CState_War_Atk_Heavy_03::GetInstance();

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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

	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Heavy_03::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// 공격후 몇초간 바로 움직이게 하지말자
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
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
	m_fTimeAcc = 0.f;
	// 공격하고 몇초간은 방향키 못누르게하자
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);


	g_fWarAtkComboTimeAcc = 0.f;
	g_pLatestWarAtkHeavyCombo = nullptr;

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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


	// 점프 셋팅
	static_cast<CWar*>(pOwner)->Set_Jump();
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
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

	// 땅에 닿았다
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] 땋에 닿으면서 방향키 누르고 있는경우 
		// [State]  -> CState_War_Jump_Land_Run
		bool dirty = false;
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
		if (dirty)
		{
			g_pWar_State_Context->ChangeState(CState_War_Jump_Land_Run::GetInstance());
			return;
		}

		// [Event] 땅닿1
		// [State]  -> CState_War_Jump_Land
		g_pWar_State_Context->ChangeState(CState_War_Jump_Land::GetInstance());
		return;
	}
}

void CState_War_Jump::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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

	// 땅에 닿았다
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] 땋에 닿으면서 방향키 누르고 있는경우 
		// [State]  -> CState_War_Jump_Land_Run
		bool dirty = false;
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
		if (dirty)
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

	// 점프 셋팅
	static_cast<CWar*>(pOwner)->Set_Jump();
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
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


	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] 땋에 닿으면서 방향키 누르고 있는경우 
		// [State]  -> CState_War_Jump_Combat_Land_Run
		bool dirty = false;
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
		if (dirty)
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

void CState_War_Jump_Combat::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Jump_Combat::Free()
{
}

// -------------------------------------------------
// #20
// [State] CState_War_Jump_Fall_Combat
// [Infom] 칼들고 점프한 뒤 떨어지는중
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


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
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

	//if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] 땋에 닿으면서 방향키 누르고 있는경우 
		// [State]  -> CState_War_Jump_Combat_Land_Run
		bool dirty = false;
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
		if (dirty)
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

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
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


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
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

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
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

	// 점프 셋팅
	static_cast<CWar*>(pOwner)->Set_Jump();
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Jump_Double::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] H (말타기)	
	// [State]  -> CState_War_Horse_Mount_Running 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Running::GetInstance());
		return;
	}

	// [Event] 마우스 오른쪽(강공)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Jump_Fall
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Double"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Fall::GetInstance());
		return;
	}


	m_fFlightTime += fTimeDelta;
	// 땅에 닿았다
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] 땋에 닿으면서 방향키 누르고 있는경우 
		// [State]  -> CState_War_Jump_Land_Run
		bool dirty = false;
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
		dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
		if (dirty)
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

void CState_War_Jump_Double::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	m_fFlightTime = 0.f;
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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

	// 잠시 공중에서 멈추자
	g_pWar_Transform_Context->Set_JumpDy(+9.5f);
	static_cast<CWar*>(pOwner)->Set_DontTurn_OnlyMove(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
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


	// [Event] 착지시
	// [State]  -> CState_War_Atk_Air_Land
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{

		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Land::GetInstance());
		return;
	}

}

void CState_War_Atk_Air_Light_03_NoImpulse::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	static_cast<CWar*>(pOwner)->Set_DontTurn_OnlyMove(false);

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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

	static_cast<CWar*>(pOwner)->Set_DontTurn_OnlyMove(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Air_Light_03_Fall::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 착지시
	// [State]  -> CState_War_Atk_Air_Land
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{

		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Land::GetInstance());
		return;
	}

}

void CState_War_Atk_Air_Light_03_Fall::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	static_cast<CWar*>(pOwner)->Set_DontTurn_OnlyMove(false);
	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	
}

void CState_War_Atk_Air_Land::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	//// [Event] 방향키 하나라도 누르게된다면
	//// [State]  -> CState_War_Run_Combat
	//bool dirty = false;
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	//if (dirty)
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
	//	return;
	//}

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
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
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
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Wrath_BladeGeyser::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	//// [Event] 방향키 하나라도 누르게된다면
	//// [State]  -> CState_War_Run_Combat
	//bool dirty = false;
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	//if (dirty)
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
	//	return;
	//}

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
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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
	static_cast<CWar*>(pOwner)->Set_DontMove_OnlyTurn(true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
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
	static_cast<CWar*>(pOwner)->Set_DontMove_OnlyTurn(false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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
	static_cast<CWar*>(pOwner)->Set_DontMove_OnlyTurn(true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
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
	static_cast<CWar*>(pOwner)->Set_DontMove_OnlyTurn(false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
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
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);

	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
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

	//// [Event] 방향키 하나라도 누르게된다면
	//// [State]  -> CState_War_Run_Combat
	//bool dirty = false;
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	//dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	//if (dirty)
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Run_Combat::GetInstance());
	//	return;
	//}

}

void CState_War_Atk_EarthSplitter_Level1::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_EarthSplitter_Level1::Free()
{
}

// -------------------------------------------------
// #31
// [State] CState_War_Atk_Flamebrand_Start
// [Infom] G스킬 - 불풍차 돌리기
// -------------------------------------------------
CState_War_Atk_Flamebrand_Start::CState_War_Atk_Flamebrand_Start()
{
	m_pStateName = "CState_War_Atk_Flamebrand_Start";
}

void CState_War_Atk_Flamebrand_Start::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Flamebrand_Start", false);// Not Loop


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Flamebrand_Start::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Atk_Flamebrand
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Flamebrand_Start"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Flamebrand::GetInstance());
		return;
	}

}

void CState_War_Atk_Flamebrand_Start::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Flamebrand_Start::Free()
{
}

// -------------------------------------------------
// #32
// [State] CState_War_Atk_Flamebrand
// [Infom] G스킬 - 불풍차 돌리는 중
// -------------------------------------------------
CState_War_Atk_Flamebrand::CState_War_Atk_Flamebrand()
{
	m_pStateName = "CState_War_Atk_Flamebrand";
}

void CState_War_Atk_Flamebrand::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Flamebrand");// Loop
	static_cast<CWar*>(pOwner)->Set_DontTurn_OnlyMove(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Flamebrand::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	m_fSpinningTimeAcc += fTimeDelta;

	// [Event] 오른쪽 마우스 떼거나 m_fSpinningTimeAcc 3초 이상 지나면 
	// [State]  -> CState_War_Atk_Flamebrand
	if (m_fSpinningTimeAcc > 5.f || CInput_Device::GetInstance()->Mouse_Pressing(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON) == false)
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Flamebrand_End::GetInstance());
		return;
	}

}

void CState_War_Atk_Flamebrand::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	m_fSpinningTimeAcc = 0.f;
	static_cast<CWar*>(pOwner)->Set_DontTurn_OnlyMove(false);

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Flamebrand::Free()
{
}



// -------------------------------------------------
// #33
// [State] CState_War_Atk_Flamebrand_End
// [Infom] G스킬 - 불풍차 돌리기 끝
// -------------------------------------------------
CState_War_Atk_Flamebrand_End::CState_War_Atk_Flamebrand_End()
{
	m_pStateName = "CState_War_Atk_Flamebrand_End";
}

void CState_War_Atk_Flamebrand_End::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Flamebrand_End", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// 해당 상태에서 무기 콜라이더 키고
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
}

void CState_War_Atk_Flamebrand_End::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Flamebrand_End"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_Atk_Flamebrand_End::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);

	// 해당 상태에서 무기 콜라이더 끄자
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
}

void CState_War_Atk_Flamebrand_End::Free()
{
}


// -------------------------------------------------
// #34
// [State] CState_War_DashTo_F
// [Infom] 대쉬
// -------------------------------------------------
CState_War_DashTo_F::CState_War_DashTo_F()
{
	m_pStateName = "CState_War_DashTo_F";
}

void CState_War_DashTo_F::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_DashTo_F", false);//Not Loop

	// 무적판정 넣거나.
	// 잔상 애니메이션 효과 시작하는 변수 넣거나
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true); // 내가 모든 키를 위임할꼬야
	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_DashTo_F::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);


	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_DashTo_F"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
	
	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < 1.f) // 1초 동안은 키입력되게 하지말자
		return;
	
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

}

void CState_War_DashTo_F::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	m_fMoveLockTimeAcc = 0.f;
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false); // 내가 모든 키를 위임할꼬야
	// 슈퍼아머
	g_pWar->m_bSuperArmor = false;
}

void CState_War_DashTo_F::Free()
{
}







// -------------------------------------------------
// #35
// [State] CState_War_Impact_From_Front_01
// [Infom] 앞에서 박았다. 뒤로 밀려난다.
// -------------------------------------------------
CState_War_Impact_From_Front_01::CState_War_Impact_From_Front_01()
{
	m_pStateName = "CState_War_Impact_From_Front_01";
}

void CState_War_Impact_From_Front_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // 초기화를 시켜주어 계속해서 이 상태로 오지 않게끔하자.
	g_pWar->m_eDir = OBJECT_DIR::DIR_B;
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Impact_From_Front_01", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_Impact_From_Front_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Impact_From_Front_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}

	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < .5f) // 1초 동안은 키입력되게 하지말자
		return;

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

void CState_War_Impact_From_Front_01::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	m_fMoveLockTimeAcc = 0.f;
	g_pWar->m_eDir = OBJECT_DIR::DIR_F;	
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
}

void CState_War_Impact_From_Front_01::Free()
{
}



// -------------------------------------------------
// #36
// [State] CState_War_Impact_From_Back_01
// [Infom] 앞에서 박았다. 뒤로 밀려난다.
// -------------------------------------------------
CState_War_Impact_From_Back_01::CState_War_Impact_From_Back_01()
{
	m_pStateName = "CState_War_Impact_From_Back_01";
}

void CState_War_Impact_From_Back_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // 초기화를 시켜주어 계속해서 이 상태로 오지 않게끔하자.
	g_pWar->m_eDir = OBJECT_DIR::DIR_F;
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Impact_From_Back_01", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_Impact_From_Back_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Impact_From_Back_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < .5f) // 1초 동안은 키입력되게 하지말자
		return;
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

void CState_War_Impact_From_Back_01::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	g_pWar->m_eDir = OBJECT_DIR::DIR_F;	m_fMoveLockTimeAcc = 0.f;
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);

}

void CState_War_Impact_From_Back_01::Free()
{
}



// -------------------------------------------------
// #37
// [State] CState_War_Impact_From_Left_01
// [Infom] 왼쪽에서 박았다. 오른쪽으로 밀려난다.
// -------------------------------------------------
CState_War_Impact_From_Left_01::CState_War_Impact_From_Left_01()
{
	m_pStateName = "CState_War_Impact_From_Left_01";
}

void CState_War_Impact_From_Left_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // 초기화를 시켜주어 계속해서 이 상태로 오지 않게끔하자.
	g_pWar->m_eDir = OBJECT_DIR::DIR_R;
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Impact_From_Left_01", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_Impact_From_Left_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Impact_From_Left_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < .5f) // 1초 동안은 키입력되게 하지말자
		return;
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

void CState_War_Impact_From_Left_01::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	g_pWar->m_eDir = OBJECT_DIR::DIR_F;	m_fMoveLockTimeAcc = 0.f;
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
}

void CState_War_Impact_From_Left_01::Free()
{
}



// -------------------------------------------------
// #38
// [State] CState_War_Impact_From_Right_01
// [Infom] 오른쪽에서 박았다. 왼쪽으로 미ㅏ련난다 
// -------------------------------------------------
CState_War_Impact_From_Right_01::CState_War_Impact_From_Right_01()
{
	m_pStateName = "CState_War_Impact_From_Right_01";
}

void CState_War_Impact_From_Right_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // 초기화를 시켜주어 계속해서 이 상태로 오지 않게끔하자.
	g_pWar->m_eDir = OBJECT_DIR::DIR_L;
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Impact_From_Right_01", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_Impact_From_Right_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Impact_From_Right_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < .5f) // 1초 동안은 키입력되게 하지말자
		return;
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

void CState_War_Impact_From_Right_01::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	g_pWar->m_eDir = OBJECT_DIR::DIR_F;	m_fMoveLockTimeAcc = 0.f;
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
}

void CState_War_Impact_From_Right_01::Free()
{
}

// -------------------------------------------------
// #39 
// [State] CState_War_IA_Death_FallenDog
// [Infom] FallenDog 처형모션
// -------------------------------------------------
CState_War_IA_Death_FallenDog::CState_War_IA_Death_FallenDog()
{
	m_pStateName = "CState_War_IA_Death_FallenDog";
}

void CState_War_IA_Death_FallenDog::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // 초기화를 시켜주어 계속해서 이 상태로 오지 않게끔하자.
	g_pWar->m_bSuperArmor = true;
	g_pWar->m_bDontMoveInWorld = true; // 애니메이션동안 월드 움직이지말자.
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_IA_Death_FallenDog", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_IA_Death_FallenDog::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_IA_Death_FallenDog"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
}

void CState_War_IA_Death_FallenDog::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	g_pWar->m_bDontMoveInWorld = false;
	g_pWar->m_bSuperArmor = false;
}

void CState_War_IA_Death_FallenDog::Free()
{
}












/* ------------------------------------------------------------------------------
*
*	War n Horse Finite State Machine
*
--------------------------------------------------------------------------------*/


// -------------------------------------------------
// #Horse1
// [State] CState_War_Horse_Mount_Standing
// [Infom] 서있는 상태에서 말타기 시작 
// -------------------------------------------------
CState_War_Horse_Mount_Standing::CState_War_Horse_Mount_Standing()
{
	m_pStateName = "CState_War_Horse_Mount_Standing";
}

void CState_War_Horse_Mount_Standing::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	static_cast<CWar*>(pOwner)->Set_War_On_Ruin_State(true);
	// 말타기(Ruin) 상태일때는 이걸로 적용해야한다
	g_pWar_Model_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());
	g_pWar_Model_Gauntlet_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());
	g_pWar_Model_Sword_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());

	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Mount_Standing", false);//Not Loop
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Mount_Standing", false, false);//Not Loop

	static_cast<CWar*>(pOwner)->Set_Speed(RUIN_SPEED);

	static_cast<CWar*>(pOwner)->Set_Dont_Key(true); // 움직이지마!

		// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Horse_Mount_Standing::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Horse_Mount_Standing"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Idle::GetInstance());
		return;
	}
}

void CState_War_Horse_Mount_Standing::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();

	static_cast<CWar*>(pOwner)->Set_Dont_Key(false); // 내가 모든 키를 위임할꼬야
}

void CState_War_Horse_Mount_Standing::Free()
{
}

// -------------------------------------------------
// #Horse2
// [State] CState_War_Horse_Mount_Running
// [Infom] 달리는 상태에서 말타기 버튼 누름
// -------------------------------------------------

CState_War_Horse_Mount_Running::CState_War_Horse_Mount_Running()
{
	m_pStateName = "CState_War_Horse_Mount_Running";
}

void CState_War_Horse_Mount_Running::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	static_cast<CWar*>(pOwner)->Set_War_On_Ruin_State(true);
	// 말타기(Ruin) 상태일때, 모델, 건틀렛, 검 오프셋은 변화시켜야한다
	g_pWar_Model_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());
	g_pWar_Model_Gauntlet_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());
	g_pWar_Model_Sword_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());

	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Mount_Running", false);//Not Loop
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Mount_Running", false, false);//Not Loop

	static_cast<CWar*>(pOwner)->Set_Speed(RUIN_SPEED);

	// 슈퍼아머
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Horse_Mount_Running::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Horse_Gallop
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Horse_Mount_Running"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Gallop::GetInstance());
		return;
	}
}

void CState_War_Horse_Mount_Running::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Horse_Mount_Running::Free()
{
}



// -------------------------------------------------
// #Horse3
// [State] CState_War_Horse_Dismount
// [Infom] 말타기 종료
// -------------------------------------------------
CState_War_Horse_Dismount::CState_War_Horse_Dismount()
{
	m_pStateName = "CState_War_Horse_Dismount";
}

void CState_War_Horse_Dismount::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Dismount", false);// Not Loop
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Dismount", false);// Not loop

	static_cast<CWar*>(pOwner)->Set_Dont_Key(true); // 움직이지마!

		// 슈퍼아머
	g_pWar->m_bSuperArmor = false;

}

void CState_War_Horse_Dismount::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Jump_Land_Heavy
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Horse_Dismount"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Jump_Land_Heavy::GetInstance());
		return;
	}

}

void CState_War_Horse_Dismount::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	static_cast<CWar*>(pOwner)->Set_War_On_Ruin_State(false);
	// 말타기 종료되면 기존 WarPivot을 사용하자
	g_pWar_Model_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarPivot());
	g_pWar_Model_Gauntlet_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarPivot());
	g_pWar_Model_Sword_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarPivot());

	static_cast<CWar*>(pOwner)->Set_Speed(WAR_SPEED);

	static_cast<CWar*>(pOwner)->Set_Dont_Key(false); // 움직이지마!

}

void CState_War_Horse_Dismount::Free()
{
}



// -------------------------------------------------
// #Horse4
// [State] CState_War_Horse_Idle
// [Infom] 기본 상태
// -------------------------------------------------
CState_War_Horse_Idle::CState_War_Horse_Idle()
{
	m_pStateName = "CState_War_Horse_Idle";
}

void CState_War_Horse_Idle::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Idle");// Loop
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Idle");// Loop
}

void CState_War_Horse_Idle::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] H 버튼	
	// [State]  -> CState_War_Horse_Dismount
	if (CInput_Device::GetInstance()->Key_Down(DIK_H)) 
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Dismount::GetInstance()); 
		return;
	}

	// [Event] 방향키 하나라도 누르게된다면
	// [State]  -> CState_War_Horse_Gallop	
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Gallop::GetInstance()); 
		return;
	}

}

void CState_War_Horse_Idle::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Horse_Idle::Free()
{
}


// -------------------------------------------------
// #Horse5
// [State] CState_War_Horse_Gallop
// [Infom] 달리는 상태
// -------------------------------------------------

CState_War_Horse_Gallop::CState_War_Horse_Gallop()
{
	m_pStateName = "CState_War_Horse_Gallop";
}

void CState_War_Horse_Gallop::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Gallop");// Loop
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Gallop");// Loop
}

void CState_War_Horse_Gallop::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] H 버튼	
	// [State]  -> CState_War_Horse_Dismount
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Dismount::GetInstance());
		return;
	}

	// [Event] 방향키 하나라도 안누르게된다면
	// [State]  -> CState_War_Horse_Stop
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty == false)
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Idle::GetInstance()); 
		return;
	}


	// [Event] LSHIFT 누르면
	// [State]  -> CState_War_Horse_Gallop_Fast_Start
	if (CInput_Device::GetInstance()->Key_Down(DIK_LSHIFT))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Gallop_Fast_Start::GetInstance());
		return;
	}


}

void CState_War_Horse_Gallop::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Horse_Gallop::Free()
{
}



// -------------------------------------------------
// #Horse6
// [State] CState_War_Horse_Gallop_Fast_Start
// [Infom] LSHIFT 달리기 시작
// -------------------------------------------------
CState_War_Horse_Gallop_Fast_Start::CState_War_Horse_Gallop_Fast_Start()
{
	m_pStateName = "CState_War_Horse_Gallop_Fast_Start";
}

void CState_War_Horse_Gallop_Fast_Start::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Gallop_Fast_Start", false);
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Gallop_Fast_Start", false);

	static_cast<CWar*>(pOwner)->Set_Speed(RUIN_SHIFT_SPEED);

}

void CState_War_Horse_Gallop_Fast_Start::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Horse_Gallop_Fast
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Horse_Gallop_Fast_Start"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Gallop_Fast::GetInstance());
		return;
	}

}

void CState_War_Horse_Gallop_Fast_Start::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Horse_Gallop_Fast_Start::Free()
{
}


// -------------------------------------------------
// #Horse7
// [State] CState_War_Horse_Gallop_Fast
// [Infom] LSHIFT 달리기
// -------------------------------------------------
CState_War_Horse_Gallop_Fast::CState_War_Horse_Gallop_Fast()
{
	m_pStateName = "CState_War_Horse_Gallop_Fast";
}

void CState_War_Horse_Gallop_Fast::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Gallop_Fast");
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Gallop_Fast");



}

void CState_War_Horse_Gallop_Fast::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);


	// [Event] LSHIFT 뗀경우	
	// [State]  -> CState_War_Horse_Gallop
	if (CInput_Device::GetInstance()->Key_Pressing(DIK_LSHIFT) == false)
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Gallop::GetInstance());
		return;
	}


	// [Event] 방향키하나라도안누르면
	// [State] -> CState_War_Horse_Stop ()
	bool isKeyDown = false;
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);

	if (isKeyDown == false)
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Stop::GetInstance());
		return;
	}
}

void CState_War_Horse_Gallop_Fast::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();

	static_cast<CWar*>(pOwner)->Set_Speed(RUIN_SPEED);
}

void CState_War_Horse_Gallop_Fast::Free()
{
}


// -------------------------------------------------
// #Horse8
// [State] CState_War_Horse_Stop
// [Infom] LSHIFT 달리기다가 멈추는 동작
// -------------------------------------------------
CState_War_Horse_Stop::CState_War_Horse_Stop()
{
	m_pStateName = "CState_War_Horse_Stop";
}

void CState_War_Horse_Stop::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Stop", false);
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Stop", false);
}

void CState_War_Horse_Stop::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] 애니메이션 종료
	// [State]  -> CState_War_Horse_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Horse_Stop"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Idle::GetInstance());
		return;
	}
}

void CState_War_Horse_Stop::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Horse_Stop::Free()
{
}


// -------------------------------------------------
// #Horse9
// [State] CState_War_Horse_Jump_Land_Heavy
// [Infom] (War만) 말내리기 이후 착지
// -------------------------------------------------
CState_War_Horse_Jump_Land_Heavy::CState_War_Horse_Jump_Land_Heavy()
{
	m_pStateName = "CState_War_Horse_Jump_Land_Heavy";
}

void CState_War_Horse_Jump_Land_Heavy::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Land_Heavy", false, false); // Loop 아니고, 보간하지말자
	//g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Land_Heavy", false);
}

void CState_War_Horse_Jump_Land_Heavy::Execute(CGameObject* pOwner, _float fTimeDelta)
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

	// [Event] 애니메이션 끝나면
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Land_Heavy"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
}

void CState_War_Horse_Jump_Land_Heavy::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
}

void CState_War_Horse_Jump_Land_Heavy::Free()
{
}
