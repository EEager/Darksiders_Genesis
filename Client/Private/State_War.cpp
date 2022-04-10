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
// [Infom]  War_Key, ���� üũ
// -------------------------------------------------

// �������� �޺� �̾����� �ð�
#define WAR_COMBO_TIME_INIT 2.f
_float g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT;
CState* g_pLatestWarAtkCombo = nullptr;
CState* g_pLatestWarAtkHeavyCombo = nullptr;

// �����Ҷ��� ����Ű �����̰� ���� �ʴ� �ð�
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

	// �޺� ���� �ð��� ��� ����
	g_fWarAtkComboTimeAcc -= fTimeDelta;
	if (g_fWarAtkComboTimeAcc < 0) 
	{
		// �޺� ���� �ð� ������ nullptr�� �ٲپ� ó�� ���ۿ��� �����ϵ������� 
		g_pLatestWarAtkCombo = nullptr;
		g_pLatestWarAtkHeavyCombo = nullptr;
		g_fWarAtkComboTimeAcc = 0.f;
	}




	// [Event] ��� �°� �ִ�. 
	// [State] HitPower�� ��� ���ҽ�Ű��
	if (g_pWar->m_bHitted)
	{
		g_pWar->m_fHitPower -= 0.01f;
		if (g_pWar->m_fHitPower < 0)
		{
			g_pWar->m_fHitPower = 0.f;
			g_pWar->m_bHitted = false;
		}
	}


	// [Event] �ǰݴ����� ��� �ǰݻ��·� õ��
	// [State] m_iHitDir�� ���� ��� �ǰ� ������� õ������ �����Ѵ�
	if (g_pWar->m_iHitDir != -1) // g_pWar->m_iHitDir �ʱ�ȭ�� �� �ǰ� ���¿��� ������.
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


	// [Event] �׾���
	// [State]  -> CState_War_Death
	if (CInput_Device::GetInstance()->Key_Down(DIK_B)) // TEST
	{
		g_pWar_State_Context->ChangeState(CState_War_Death::GetInstance());
		return;
	}


	if (static_cast<CWar*>(pOwner)->Get_War_On_Ruin_State() == false) // ��Ÿ�� ���� ������ SHIFT�� �뽬�̴�
	{
		// [Event] �뽬
		// [State]  -> CState_War_DashTo_F
		if (m_bShiftLockTimeAccStart)
		{
			m_fShiftLockTimeAcc += fTimeDelta;
			if (m_fShiftLockTimeAcc > SHIFT_LOCK_TIME) // ������ shift������ SHIFT_LOCK_TIME������ �������ϰ� ����
			{
				m_bShiftLockTimeAccStart = false;
				m_fShiftLockTimeAcc = 0.f;
			}
		}

		if (m_bShiftLockTimeAccStart == false && CInput_Device::GetInstance()->Key_Down(DIK_LSHIFT))
		{
			// ---------------------
			// ���� : ����Ʈ ���߿� ����Ű�� ����, �ش� �������� ������ �ǵ��� ����
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
	else // ��Ÿ�� �ִ� ���
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
// [Infom] �׾���
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

	// [Event] �ִϸ��̼� ����
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
// [Infom] ���� �ȵ�� ������ ���ִ� ����
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

	// [Event] H (��Ÿ��)	
	// [State]  -> CState_War_Horse_Mount_Standing 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Standing::GetInstance());
		return;
	}

	// [Event] 1�� ��ų
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}


	// [Event] �����̽� (����)
	// [State]  -> CState_War_Jump 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump::GetInstance());
		return;
	}

	// [Event] ���콺 ������(����)
	// [State]  -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] ���콺 ����(���)
	// [State]  -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;	
	}


	// [Event] ��ó�� ���� ������, R ��ư �������� 
	// [State]  -> CState_War_Idle_to_Idle_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_R))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_to_Idle_Combat::GetInstance());
		return;
	}


	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
// [Infom] ���� �ȵ鲿 �ٴ� ���� 
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

	// [Event] H (��Ÿ��)	
	// [State]  -> CState_War_Horse_Mount_Running 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Running::GetInstance());
		return;
	}
	// [Event] 1�� ��ų
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}

	// [Event] �����̽� (����)
	// [State]  -> CState_War_Jump 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump::GetInstance());
		return;
	}

	// [Event] ���콺 ������(����)
	// [State]  -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] ���콺 ����(���)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}


	// [Event] ����Ű �ϳ��� �� ������
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
// [Infom] ���̵鿡�� �˾��̵���� ���� �߰� �ܰ�
// -------------------------------------------------
CState_War_Idle_to_Idle_Combat::CState_War_Idle_to_Idle_Combat()
{
	m_pStateName = "CState_War_Idle_to_Idle_Combat";
}

void CState_War_Idle_to_Idle_Combat::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();


	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Idle_to_Idle_Combat", false/*loop false�̾��, Idle_Combat�� ������ ������ ���¸� �����Ѵ� */);
}

void CState_War_Idle_to_Idle_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] H (��Ÿ��)	
	// [State]  -> CState_War_Horse_Mount_Standing 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Standing::GetInstance());
		return;
	}

	// [Event] 1�� ��ų
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}

	// [Event] �����̽� (����)
	// [State] -> CState_War_Jump_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat::GetInstance());
		return;
	}

	// [Event] ���콺 ������(����)
	// [State] -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] ���콺 ����(���)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}


	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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


	// [Event] CState_War_Idle_Combat ù ��° �ִϸ��̼� �������
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
// [Infom] Į Idle
// -------------------------------------------------
CState_War_Idle_Combat::CState_War_Idle_Combat()
{
	m_pStateName = "CState_War_Idle_Combat";
}

void CState_War_Idle_Combat::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();

	// Loop �ִϸ��̼�
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Idle_Combat");
}

void CState_War_Idle_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] H (��Ÿ��)	
	// [State]  -> CState_War_Horse_Mount_Standing 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Standing::GetInstance());
		return;
	}

	// [Event] 1�� ��ų
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}


	// [Event] �����̽� (����)
	// [State] -> CState_War_Jump_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat::GetInstance());
		return;
	}

	// [Event] ���콺 ������(����)
	// [State] -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] ���콺 ����(���)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] Idle_Combat���� 4�ʰ� �ƹ��͵� ���ϸ�.
	// [State] -> CState_War_Idle_Combat_to_Idle
	m_fIdle_Combat_time += fTimeDelta;
	if (m_fIdle_Combat_time > 4.f)
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat_to_Idle::GetInstance());
		m_fIdle_Combat_time = 0.f;
		return;
	}


	// [Event] R ��ư �������� 
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
// [Infom] combat idle -> idle �� ���� ���� 
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

	// [Event] H (��Ÿ��)	
	// [State]  -> CState_War_Horse_Mount_Standing 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Standing::GetInstance());
		return;
	}

	// [Event] 1�� ��ų
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}

	// [Event] �����̽� (����)
	// [State] -> CState_War_Jump_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat::GetInstance());
		return;
	}

	// [Event] ���콺 ������(����)
	// [State] -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] ���콺 ����(���)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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


	// [Event] CState_War_Idle_Combat_to_Idle �ִϸ��̼� ������
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
// [Infom] Į��� �ٴ� ����
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

	// [Event] H (��Ÿ��)	
	// [State]  -> CState_War_Horse_Mount_Running 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Running::GetInstance());
		return;
	}

	// [Event] 1�� ��ų
	// [State]  -> CState_War_Wrath_BladeGeyser 
	if (CInput_Device::GetInstance()->Key_Down(DIK_1))
	{
		g_pWar_State_Context->ChangeState(CState_War_Wrath_BladeGeyser::GetInstance());
		return;
	}


	// [Event] �����̽� (����)
	// [State] -> CState_War_Jump_Combat 
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat::GetInstance());
		return;
	}

	// [Event] ���콺 ������(����)
	// [State] -> CState_War_Atk_Heavy_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_01::GetInstance());
		return;
	}

	// [Event] ���콺 ����(���)
	// [State] -> CState_War_Atk_Light_01 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_01::GetInstance());
		return;
	}

	// [Event] ����Ű �ϳ��� �ȴ�����, ĮIdle�� õ��
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
// [Infom] ���1
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
		
	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);

	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);

	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Light_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// ���� �޺��� �����Ǿ��ִٸ� �װɷ� ���� ��ȯ
	if (g_pLatestWarAtkCombo)
	{
		g_pWar_State_Context->ChangeState(g_pLatestWarAtkCombo);
		return;
	}

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
		// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] ���콺 ����(���)
	// [State] CState_War_Atk_Light_01 -> CState_War_Atk_Light_02
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_02::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Light_01 �ִϸ��̼� ������
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
	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT; // ���� �޺�Ÿ�� �ʱ�ȭ
	g_pLatestWarAtkCombo = CState_War_Atk_Light_02::GetInstance(); // ���� �޺��� ����������


	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;

}

void CState_War_Atk_Light_01::Free()
{
}


// -------------------------------------------------
// #8
// [State] CState_War_Atk_Light_02
// [Infom] ���2
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Light_02::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
		// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] ���콺 ����(���)
	// [State] CState_War_Atk_Light_02 -> CState_War_Atk_Light_03
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_03::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Light_02 �ִϸ��̼� ������
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	m_fTimeAcc = 0.f;


	// ���� �޺� ���� �־�����
	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT;
	g_pLatestWarAtkCombo = CState_War_Atk_Light_03::GetInstance();

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Light_02::Free()
{
}

// -------------------------------------------------
// #9
// [State] CState_War_Atk_Light_03
// [Infom] ���3
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Light_03::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
		// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] ���콺 ����(���)
	// [State] CState_War_Atk_Light_03 -> CState_War_Atk_Light_04
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Light_04::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Light_03 �ִϸ��̼� ������
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	m_fTimeAcc = 0.f;
	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT;
	g_pLatestWarAtkCombo = g_pLatestWarAtkCombo = CState_War_Atk_Light_04::GetInstance();;

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Light_03::Free()
{
}

// -------------------------------------------------
// #10
// [State] CState_War_Atk_Light_04
// [Infom] ���4
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);

	// 0�� �η� ���� 1�޺��� ���� ���� 
	g_fWarAtkComboTimeAcc = 0;


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Light_04::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
		// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	//// [Event] ���콺 ������(����)
	//// [State]  -> G��ų
	//if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Atk_Light_04::GetInstance());
	//	return;
	//}

	// [Event] CState_War_Atk_Light_04 �ִϸ��̼� ������
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);
	m_fTimeAcc = 0.f;

	g_fWarAtkComboTimeAcc = 0.f;
	g_pLatestWarAtkCombo = nullptr;

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Light_04::Free()
{
}

// -------------------------------------------------
// #11
// [State] CState_War_Atk_Heavy_01
// [Infom] ����1
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Heavy_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// �����޺��ð��� �����ִٸ� �� �޺� �������� state�� ����
	if (g_pLatestWarAtkHeavyCombo)
	{
		g_pWar_State_Context->ChangeState(g_pLatestWarAtkHeavyCombo);
		return;
	}

	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
		// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
		// [Event] ������ ���콺 1�� �̻� ��¡�ϸ� �޺� ��ų��
		// [State]  -> CState_War_Atk_EarthSplitter_Charge_Start
		// ������ �ִ� ���
		if (CInput_Device::GetInstance()->Mouse_Pressing(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
		{
			m_fBntPressTime += fTimeDelta;
#ifdef _DEBUG
			printf("m_fBntPressTime(%lf)\n", m_fBntPressTime);
#endif
			if (m_fBntPressTime > .5f) // 1�� �̻� ��¡�ϸ� �޺� ��ų��
			{
				if (static_cast<CWar*>(pOwner)->Get_GType() == CWar::G_TYPE_FIRE)
					g_pWar_State_Context->ChangeState(CState_War_Atk_Flamebrand_Start::GetInstance());
				else
					g_pWar_State_Context->ChangeState(CState_War_Atk_EarthSplitter_Charge_Start::GetInstance());
				return;
			}
		}
		// [Event] ������ ���콺
		// [State]  -> CState_War_Atk_Heavy_02
		else
		{
			if (m_fBntPressTime < 0.5f) // �ٷ� ���ٸ� ���� ���� ����
			{
				g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_02::GetInstance());
				return;
			}
		}
	}


	// [Event] CState_War_Atk_Heavy_01 �ִϸ��̼� ������
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

	m_fTimeAcc = 0.f; // Ű���ð� �ʱ�ȭ����

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);

	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT; // ���� �޺�Ÿ�ӳ־�����
	g_pLatestWarAtkHeavyCombo = CState_War_Atk_Heavy_02::GetInstance();

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Heavy_01::Free()
{
}

// -------------------------------------------------
// #12
// [State] CState_War_Atk_Heavy_02
// [Infom] ����2
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Heavy_02::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// ������ ���ʰ� �ٷ� �����̰� ��������
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
		// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] ���콺 ������(����)
	// [State]  -> GCState_War_Atk_Heavy_03
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_03::GetInstance());
		return;
	}

	// [Event] CState_War_Atk_Heavy_02 �ִϸ��̼� ������
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

	m_fTimeAcc = 0.f; // Ű���ð� �ʱ�ȭ����

		// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);

	g_fWarAtkComboTimeAcc = WAR_COMBO_TIME_INIT; // ���� �޺� �ð� �ʱ�ȭ��������
	g_pLatestWarAtkHeavyCombo = CState_War_Atk_Heavy_03::GetInstance();

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Heavy_02::Free()
{
}

// -------------------------------------------------
// #13
// [State] CState_War_Atk_Heavy_03
// [Infom] ����3
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

	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Heavy_03::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// ������ ���ʰ� �ٷ� �����̰� ��������
	m_fTimeAcc += fTimeDelta;
	if (m_fTimeAcc > WAR_KEY_LOCK_TIME_CAUSE_OF_ATK)
	{
		// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	//// [Event] ���콺 ������(����)
	//// [State]  -> G��ų
	//if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	//{
	//	g_pWar_State_Context->ChangeState(CState_War_Atk_Heavy_02::GetInstance());
	//	return;
	//}

	// [Event] CState_War_Atk_Heavy_03 �ִϸ��̼� ������
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
	// �����ϰ� ���ʰ��� ����Ű ������������
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false);


	g_fWarAtkComboTimeAcc = 0.f;
	g_pLatestWarAtkHeavyCombo = nullptr;

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Heavy_03::Free()
{
}

// -------------------------------------------------
// #14
// [State] CState_War_Jump
// [Infom] ����ȵ�� ����
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


	// ���� ����
	static_cast<CWar*>(pOwner)->Set_Jump();
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Jump::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] ���콺 ������(����)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	// [Event] �����̽� (����)
	// [State]  -> CState_War_Jump_Double
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Double::GetInstance());
		return;
	}

	// [Event] CState_War_Jump �ִϸ��̼� ������
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Fall::GetInstance());
		return;
	}

	// ���� ��Ҵ�
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] ���� �����鼭 ����Ű ������ �ִ°�� 
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

		// [Event] ����1
		// [State]  -> CState_War_Jump_Land
		g_pWar_State_Context->ChangeState(CState_War_Jump_Land::GetInstance());
		return;
	}
}

void CState_War_Jump::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Jump::Free()
{
}

// -------------------------------------------------
// #15
// [State] CState_War_Jump_Fall
// [Infom] ����ȵ�� ������
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
	// [Event] ���콺 ������(����)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}


	m_fFlightTime += fTimeDelta;
	// [Event] �����̽� (����)
	// [State]  -> CState_War_Jump_Double
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Double::GetInstance());
		return;
	}

	// ���� ��Ҵ�
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] ���� �����鼭 ����Ű ������ �ִ°�� 
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
			// [Event] ����1
			// [State]  -> CState_War_Jump_Land
			g_pWar_State_Context->ChangeState(CState_War_Jump_Land::GetInstance());
			return;
		}
		else
		{
			// [Event] ����2 - �������� �̻�Ǹ� ����2�� 
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
// [Infom] ����1
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

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] CState_War_Jump_Land �ִϸ��̼� ������
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
// [Infom] ����2
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

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] CState_War_Jump_Land_Heavy �ִϸ��̼� ������
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
// [Infom] �����
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

	// [Event] CState_War_Jump_Land_Run �ִϸ��̼� ������
	// [State]  -> CState_War_Run
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Land_Run"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Run::GetInstance());
		return;
	}

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
// [Infom] Į��� ����
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

	// ���� ����
	static_cast<CWar*>(pOwner)->Set_Jump();
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Jump_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] ���콺 ������(����)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	// [Event] �����̽� (����)
	// [State]  -> CState_War_Jump_Double
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Double::GetInstance());
		return;
	}

	// [Event] CState_War_Jump_Combat �ִϸ��̼� ������
	// [State]  -> CState_War_Jump_Fall_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Combat"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Fall_Combat::GetInstance());
		return;
	}


	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] ���� �����鼭 ����Ű ������ �ִ°�� 
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

		// [Event] ����
		// [State]  -> CState_War_Jump_Combat_Land
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat_Land::GetInstance());
		return;
	}
}

void CState_War_Jump_Combat::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Jump_Combat::Free()
{
}

// -------------------------------------------------
// #20
// [State] CState_War_Jump_Fall_Combat
// [Infom] Į��� ������ �� ����������
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


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
}

void CState_War_Jump_Fall_Combat::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);
	// [Event] ���콺 ������(����)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	// [Event] �����̽� (����)
	// [State]  -> CState_War_Jump_Double
	if (CInput_Device::GetInstance()->Key_Down(DIK_SPACE))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Double::GetInstance());
		return;
	}

	//if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] ���� �����鼭 ����Ű ������ �ִ°�� 
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

		// [Event] ����
		// [State]  -> CState_War_Jump_Combat_Land
		g_pWar_State_Context->ChangeState(CState_War_Jump_Combat_Land::GetInstance());
		return;
	}
}

void CState_War_Jump_Fall_Combat::Exit(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Exit();

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
}

void CState_War_Jump_Fall_Combat::Free()
{
}

// -------------------------------------------------
// #21
// [State] CState_War_Jump_Combat_Land
// [Infom] Į��� ����
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


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
}

void CState_War_Jump_Combat_Land::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] �ִϸ��̼� ����
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

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
}

void CState_War_Jump_Combat_Land::Free()
{
}

// -------------------------------------------------
// #22
// [State] CState_War_Jump_Combat_Land_Run
// [Infom] Į��� �����ϸ鼭 �ٱ�
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

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] �ִϸ��̼� ����
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
// [Infom] �������� 
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

	// ���� ����
	static_cast<CWar*>(pOwner)->Set_Jump();
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Jump_Double::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] H (��Ÿ��)	
	// [State]  -> CState_War_Horse_Mount_Running 
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Mount_Running::GetInstance());
		return;
	}

	// [Event] ���콺 ������(����)
	// [State]  -> CState_War_Atk_Air_Light_03_NoImpulse
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_RBUTTON))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_NoImpulse::GetInstance());
		return;
	}

	// [Event] �ִϸ��̼� ����
	// [State]  -> CState_War_Jump_Fall
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Jump_Double"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Jump_Fall::GetInstance());
		return;
	}


	m_fFlightTime += fTimeDelta;
	// ���� ��Ҵ�
	if (static_cast<CWar*>(pOwner)->Get_Jump() == false)
	{
		// [Event] ���� �����鼭 ����Ű ������ �ִ°�� 
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
			// [Event] ����1
			// [State]  -> CState_War_Jump_Land
			g_pWar_State_Context->ChangeState(CState_War_Jump_Land::GetInstance());
			return;
		}
		else
		{
			// [Event] ����2 - �������� �̻�Ǹ� ����2�� 
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
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Jump_Double::Free()
{
}


// -------------------------------------------------
// #24
// [State] CState_War_Atk_Air_Light_03_NoImpulse
// [Infom] ������ ������
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

	// ��� ���߿��� ������
	g_pWar_Transform_Context->Set_JumpDy(+9.5f);
	static_cast<CWar*>(pOwner)->Set_DontTurn_OnlyMove(true);


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Air_Light_03_NoImpulse::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
	// [State]  -> CState_War_Atk_Air_Light_03_Fall
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_Air_Light_03_NoImpulse"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Atk_Air_Light_03_Fall::GetInstance());
		return;
	}


	// [Event] ������
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

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Air_Light_03_NoImpulse::Free()
{
}


// -------------------------------------------------
// #25
// [State] CState_War_Atk_Air_Light_03_Fall
// [Infom] ���� ���� ��
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


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Air_Light_03_Fall::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] ������
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
	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Air_Light_03_Fall::Free()
{
}


// -------------------------------------------------
// #26
// [State] CState_War_Atk_Air_Land
// [Infom] ���� ������ ����
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


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	
}

void CState_War_Atk_Air_Land::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	//// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] �ִϸ��̼� ����
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

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
}

void CState_War_Atk_Air_Land::Free()
{
}

// -------------------------------------------------
// #27
// [State] CState_War_Wrath_BladeGeyser
// [Infom] 1�� ��ų
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
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Wrath_BladeGeyser::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	//// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] �ִϸ��̼� ����
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
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Wrath_BladeGeyser::Free()
{
}





// -------------------------------------------------
// #28
// [State] CState_War_Atk_EarthSplitter_Charge_Start
// [Infom] G��ų - �� ������� ����
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
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_EarthSplitter_Charge_Start::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
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
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_EarthSplitter_Charge_Start::Free()
{
}


// -------------------------------------------------
// #29
// [State] CState_War_Atk_EarthSplitter_Charge_Loop
// [Infom] G��ų - �� ������� ��
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
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_EarthSplitter_Charge_Loop::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] ���콺 ������ ����
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
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_EarthSplitter_Charge_Loop::Free()
{
}

// -------------------------------------------------
// #30
// [State] CState_War_Atk_EarthSplitter_Level1
// [Infom] G��ų - �� ������� ��. �߻� 
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


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);

	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_EarthSplitter_Level1::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Atk_EarthSplitter_Level1"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}

	//// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);

	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_EarthSplitter_Level1::Free()
{
}

// -------------------------------------------------
// #31
// [State] CState_War_Atk_Flamebrand_Start
// [Infom] G��ų - ��ǳ�� ������
// -------------------------------------------------
CState_War_Atk_Flamebrand_Start::CState_War_Atk_Flamebrand_Start()
{
	m_pStateName = "CState_War_Atk_Flamebrand_Start";
}

void CState_War_Atk_Flamebrand_Start::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Atk_Flamebrand_Start", false);// Not Loop


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Flamebrand_Start::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
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

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Flamebrand_Start::Free()
{
}

// -------------------------------------------------
// #32
// [State] CState_War_Atk_Flamebrand
// [Infom] G��ų - ��ǳ�� ������ ��
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


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Atk_Flamebrand::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	m_fSpinningTimeAcc += fTimeDelta;

	// [Event] ������ ���콺 ���ų� m_fSpinningTimeAcc 3�� �̻� ������ 
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

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_Atk_Flamebrand::Free()
{
}



// -------------------------------------------------
// #33
// [State] CState_War_Atk_Flamebrand_End
// [Infom] G��ų - ��ǳ�� ������ ��
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


	// �ش� ���¿��� ���� �ݶ��̴� Ű��
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, false);
}

void CState_War_Atk_Flamebrand_End::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
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

	// �ش� ���¿��� ���� �ݶ��̴� ����
	g_pWar->Set_Collider_Attribute(COL_WAR_WEAPON, true);
}

void CState_War_Atk_Flamebrand_End::Free()
{
}


// -------------------------------------------------
// #34
// [State] CState_War_DashTo_F
// [Infom] �뽬
// -------------------------------------------------
CState_War_DashTo_F::CState_War_DashTo_F()
{
	m_pStateName = "CState_War_DashTo_F";
}

void CState_War_DashTo_F::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_DashTo_F", false);//Not Loop

	// �������� �ְų�.
	// �ܻ� �ִϸ��̼� ȿ�� �����ϴ� ���� �ְų�
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true); // ���� ��� Ű�� �����Ҳ���
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_DashTo_F::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);


	// [Event] �ִϸ��̼� ����
	// [State]  -> CState_War_Idle_Combat
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_DashTo_F"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle_Combat::GetInstance());
		return;
	}
	
	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < 1.f) // 1�� ������ Ű�Էµǰ� ��������
		return;
	
	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
	static_cast<CWar*>(pOwner)->Set_Dont_Key(false); // ���� ��� Ű�� �����Ҳ���
	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;
}

void CState_War_DashTo_F::Free()
{
}







// -------------------------------------------------
// #35
// [State] CState_War_Impact_From_Front_01
// [Infom] �տ��� �ھҴ�. �ڷ� �з�����.
// -------------------------------------------------
CState_War_Impact_From_Front_01::CState_War_Impact_From_Front_01()
{
	m_pStateName = "CState_War_Impact_From_Front_01";
}

void CState_War_Impact_From_Front_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // �ʱ�ȭ�� �����־� ����ؼ� �� ���·� ���� �ʰԲ�����.
	g_pWar->m_eDir = OBJECT_DIR::DIR_B;
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Impact_From_Front_01", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_Impact_From_Front_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Impact_From_Front_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}

	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < .5f) // 1�� ������ Ű�Էµǰ� ��������
		return;

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
// [Infom] �տ��� �ھҴ�. �ڷ� �з�����.
// -------------------------------------------------
CState_War_Impact_From_Back_01::CState_War_Impact_From_Back_01()
{
	m_pStateName = "CState_War_Impact_From_Back_01";
}

void CState_War_Impact_From_Back_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // �ʱ�ȭ�� �����־� ����ؼ� �� ���·� ���� �ʰԲ�����.
	g_pWar->m_eDir = OBJECT_DIR::DIR_F;
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Impact_From_Back_01", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_Impact_From_Back_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Impact_From_Back_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < .5f) // 1�� ������ Ű�Էµǰ� ��������
		return;
	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
// [Infom] ���ʿ��� �ھҴ�. ���������� �з�����.
// -------------------------------------------------
CState_War_Impact_From_Left_01::CState_War_Impact_From_Left_01()
{
	m_pStateName = "CState_War_Impact_From_Left_01";
}

void CState_War_Impact_From_Left_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // �ʱ�ȭ�� �����־� ����ؼ� �� ���·� ���� �ʰԲ�����.
	g_pWar->m_eDir = OBJECT_DIR::DIR_R;
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Impact_From_Left_01", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_Impact_From_Left_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Impact_From_Left_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < .5f) // 1�� ������ Ű�Էµǰ� ��������
		return;
	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
// [Infom] �����ʿ��� �ھҴ�. �������� �̤��ó��� 
// -------------------------------------------------
CState_War_Impact_From_Right_01::CState_War_Impact_From_Right_01()
{
	m_pStateName = "CState_War_Impact_From_Right_01";
}

void CState_War_Impact_From_Right_01::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // �ʱ�ȭ�� �����־� ����ؼ� �� ���·� ���� �ʰԲ�����.
	g_pWar->m_eDir = OBJECT_DIR::DIR_L;
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Impact_From_Right_01", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_Impact_From_Right_01::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
	// [State]  -> CState_War_Idle
	if (g_pWar_Model_Context->Get_Animation_isFinished("War_Mesh.ao|War_Impact_From_Right_01"))
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
	m_fMoveLockTimeAcc += fTimeDelta;

	if (m_fMoveLockTimeAcc < .5f) // 1�� ������ Ű�Էµǰ� ��������
		return;
	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
// [Infom] FallenDog ó�����
// -------------------------------------------------
CState_War_IA_Death_FallenDog::CState_War_IA_Death_FallenDog()
{
	m_pStateName = "CState_War_IA_Death_FallenDog";
}

void CState_War_IA_Death_FallenDog::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar->m_iHitDir = -1; // �ʱ�ȭ�� �����־� ����ؼ� �� ���·� ���� �ʰԲ�����.
	g_pWar->m_bSuperArmor = true;
	g_pWar->m_bDontMoveInWorld = true; // �ִϸ��̼ǵ��� ���� ������������.
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_IA_Death_FallenDog", false);//Not Loop
	static_cast<CWar*>(pOwner)->Set_Dont_Key(true);
}

void CState_War_IA_Death_FallenDog::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
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
// [Infom] ���ִ� ���¿��� ��Ÿ�� ���� 
// -------------------------------------------------
CState_War_Horse_Mount_Standing::CState_War_Horse_Mount_Standing()
{
	m_pStateName = "CState_War_Horse_Mount_Standing";
}

void CState_War_Horse_Mount_Standing::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	static_cast<CWar*>(pOwner)->Set_War_On_Ruin_State(true);
	// ��Ÿ��(Ruin) �����϶��� �̰ɷ� �����ؾ��Ѵ�
	g_pWar_Model_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());
	g_pWar_Model_Gauntlet_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());
	g_pWar_Model_Sword_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());

	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Mount_Standing", false);//Not Loop
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Mount_Standing", false, false);//Not Loop

	static_cast<CWar*>(pOwner)->Set_Speed(RUIN_SPEED);

	static_cast<CWar*>(pOwner)->Set_Dont_Key(true); // ����������!

		// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Horse_Mount_Standing::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
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

	static_cast<CWar*>(pOwner)->Set_Dont_Key(false); // ���� ��� Ű�� �����Ҳ���
}

void CState_War_Horse_Mount_Standing::Free()
{
}

// -------------------------------------------------
// #Horse2
// [State] CState_War_Horse_Mount_Running
// [Infom] �޸��� ���¿��� ��Ÿ�� ��ư ����
// -------------------------------------------------

CState_War_Horse_Mount_Running::CState_War_Horse_Mount_Running()
{
	m_pStateName = "CState_War_Horse_Mount_Running";
}

void CState_War_Horse_Mount_Running::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	static_cast<CWar*>(pOwner)->Set_War_On_Ruin_State(true);
	// ��Ÿ��(Ruin) �����϶�, ��, ��Ʋ��, �� �������� ��ȭ���Ѿ��Ѵ�
	g_pWar_Model_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());
	g_pWar_Model_Gauntlet_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());
	g_pWar_Model_Sword_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarRuinPivot());

	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Horse_Mount_Running", false);//Not Loop
	g_pWar_Model_Ruin_Context->SetUp_Animation("War_Ruin_Mesh.ao|War_Horse_Mount_Running", false, false);//Not Loop

	static_cast<CWar*>(pOwner)->Set_Speed(RUIN_SPEED);

	// ���۾Ƹ�
	g_pWar->m_bSuperArmor = true;
}

void CState_War_Horse_Mount_Running::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
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
// [Infom] ��Ÿ�� ����
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

	static_cast<CWar*>(pOwner)->Set_Dont_Key(true); // ����������!

		// ���۾Ƹ�
	g_pWar->m_bSuperArmor = false;

}

void CState_War_Horse_Dismount::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] �ִϸ��̼� ����
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
	// ��Ÿ�� ����Ǹ� ���� WarPivot�� �������
	g_pWar_Model_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarPivot());
	g_pWar_Model_Gauntlet_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarPivot());
	g_pWar_Model_Sword_Context->Set_PivotMatrix(static_cast<CWar*>(pOwner)->Get_WarPivot());

	static_cast<CWar*>(pOwner)->Set_Speed(WAR_SPEED);

	static_cast<CWar*>(pOwner)->Set_Dont_Key(false); // ����������!

}

void CState_War_Horse_Dismount::Free()
{
}



// -------------------------------------------------
// #Horse4
// [State] CState_War_Horse_Idle
// [Infom] �⺻ ����
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

	// [Event] H ��ư	
	// [State]  -> CState_War_Horse_Dismount
	if (CInput_Device::GetInstance()->Key_Down(DIK_H)) 
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Dismount::GetInstance()); 
		return;
	}

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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
// [Infom] �޸��� ����
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
	// [Event] H ��ư	
	// [State]  -> CState_War_Horse_Dismount
	if (CInput_Device::GetInstance()->Key_Down(DIK_H))
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Dismount::GetInstance());
		return;
	}

	// [Event] ����Ű �ϳ��� �ȴ����Եȴٸ�
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


	// [Event] LSHIFT ������
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
// [Infom] LSHIFT �޸��� ����
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

	// [Event] �ִϸ��̼� ����
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
// [Infom] LSHIFT �޸���
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


	// [Event] LSHIFT �����	
	// [State]  -> CState_War_Horse_Gallop
	if (CInput_Device::GetInstance()->Key_Pressing(DIK_LSHIFT) == false)
	{
		g_pWar_State_Context->ChangeState(CState_War_Horse_Gallop::GetInstance());
		return;
	}


	// [Event] ����Ű�ϳ��󵵾ȴ�����
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
// [Infom] LSHIFT �޸���ٰ� ���ߴ� ����
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

	// [Event] �ִϸ��̼� ����
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
// [Infom] (War��) �������� ���� ����
// -------------------------------------------------
CState_War_Horse_Jump_Land_Heavy::CState_War_Horse_Jump_Land_Heavy()
{
	m_pStateName = "CState_War_Horse_Jump_Land_Heavy";
}

void CState_War_Horse_Jump_Land_Heavy::Enter(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Enter();
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Land_Heavy", false, false); // Loop �ƴϰ�, ������������
	//g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Jump_Land_Heavy", false);
}

void CState_War_Horse_Jump_Land_Heavy::Execute(CGameObject* pOwner, _float fTimeDelta)
{
	CState::Execute(pOwner, fTimeDelta);

	// [Event] ����Ű �ϳ��� �����Եȴٸ�
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

	// [Event] �ִϸ��̼� ������
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
