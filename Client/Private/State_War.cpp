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


// =====================
// Global War State Machine in 
// =====================
CStateMachine* g_pWar_State_Context;
CModel* g_pWar_Model_Context;
CTransform* g_pWar_Transform_Context;


// -------------------------------------------------
// CState_War_Idle
// Desc : 무기 안들고 가만히 서있는 상태
// -------------------------------------------------
IMPLEMENT_SINGLETON(CState_War_Idle)

CState_War_Idle::CState_War_Idle()
{
}

void CState_War_Idle::Enter(CGameObject* pOwner, _float fDeltaTime)
{	
	// 애니메이션 Idx
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Idle");
}

void CState_War_Idle::Execute(CGameObject* pOwner, _float fDeltaTime)
{
	// ----------------------------------
	// Idle -> Idle_to_Idle_Combat 
	// #1. 공격시 
	if (CInput_Device::GetInstance()->Mouse_Down(CInput_Device::MOUSEBUTTONSTATE::DIMB_LBUTTON))
	{
		
	}
	
	
	// ----------------------------------
	// Idle -> Idle_Run
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

void CState_War_Idle::Exit(CGameObject* pOwner, _float fDeltaTime)
{

}

void CState_War_Idle::Free()
{
}



// -------------------------------------------------
// CState_War_Run
// Desc : 무기 안들꼬 뛰는 상태 
// -------------------------------------------------
IMPLEMENT_SINGLETON(CState_War_Run)

CState_War_Run::CState_War_Run()
{
}

void CState_War_Run::Enter(CGameObject* pOwner, _float fDeltaTime)
{
	// 애니메이션 Idx
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Run_F");
}

void CState_War_Run::Execute(CGameObject* pOwner, _float fDeltaTime)
{
	
	bool isKeyDown = false;
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	isKeyDown |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);

	// Idle_Run -> Idle (키 하나라도 눌러져있어야한다)
	if (isKeyDown == false) 
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}
}

void CState_War_Run::Exit(CGameObject* pOwner, _float fDeltaTime)
{
}

_float CState_War_Run::GetDegree(_ubyte downedKey)
{
	_float retDegree = 0.f;//12 Default는 정면을 바라보게하자.

	// 왼위오아
	if (downedKey == 0b0001)//6
		retDegree = 180.f;
	else if (downedKey == 0b0011)//4
		retDegree = 135.f; 
	else if (downedKey == 0b0010)//3
		retDegree = 90.f;
	else if (downedKey == 0b0110)//2
		retDegree = 45.f;
	else if (downedKey == 0b0100)//12
		retDegree = 0.f;
	else if (downedKey == 0b1100)//11
		retDegree = 315.f;
	else if (downedKey == 0b1000)//9
		retDegree = 270.f;
	else if (downedKey == 0b1001)//7
		retDegree =	225.f;

	return retDegree;

}

_bool CState_War_Run::KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit)
{
	if (CInput_Device::GetInstance()->Key_Pressing(key))
	{
		_ubyte dirBit = 0b0; // 왼위오아
		switch (key)
		{
		case DIK_A:
			dirBit = 0b1000;
			break;
		case DIK_W:
			dirBit = 0b0100;
			break;
		case DIK_D:
			dirBit = 0b0010;
			break;
		case DIK_S:
			dirBit = 0b0001;
			break;
		default:
			break;
		}

		keyDownCheckBit |= dirBit;
		return true;
	}

	return false;
}


void CState_War_Run::Free()
{
	Exit();
}


// -------------------------------------------------
// CState_War_Idle_to_Idle_Combat
// Desc : Idle 에서 Idle_Combat 까지 중간 단계 
// -------------------------------------------------
IMPLEMENT_SINGLETON(CState_War_Idle_to_Idle_Combat)

CState_War_Idle_to_Idle_Combat::CState_War_Idle_to_Idle_Combat()
{
}

void CState_War_Idle_to_Idle_Combat::Enter(CGameObject* pOwner, _float fDeltaTime)
{
	// 애니메이션 Idx
	g_pWar_Model_Context->SetUp_Animation("War_Mesh.ao|War_Idle_to_Idle_Combat");
}

void CState_War_Idle_to_Idle_Combat::Execute(CGameObject* pOwner, _float fDeltaTime)
{
	// Idle_to_Idle_Combat -> Idle_Combat (키 하나라도 눌러져있어야한다)
	
}

void CState_War_Idle_to_Idle_Combat::Exit(CGameObject* pOwner, _float fDeltaTime)
{
}

void CState_War_Idle_to_Idle_Combat::Free()
{
	Exit();
}