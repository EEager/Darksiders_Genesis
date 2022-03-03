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
// Global War State Machine
// =====================
CStateMachine* g_pWar_State_Context;


// ---------------------
// CState_War_Idle
// ---------------------
IMPLEMENT_SINGLETON(CState_War_Idle)

CState_War_Idle::CState_War_Idle()
{
}

void CState_War_Idle::Enter(CGameObject* pOwner, _float fDeltaTime)
{
	// 애니메이션 Idx
	CModel* pModel = (CModel*)pOwner->Get_ComponentPtr(TEXT("Com_Model_War"));
	assert(pModel);
	pModel->SetUp_Animation("War_Mesh.ao|War_Idle");
}

void CState_War_Idle::Execute(CGameObject* pOwner, _float fDeltaTime)
{
	bool dirty = false;
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_A);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_W);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_D);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_S);
	if (dirty)
		g_pWar_State_Context->ChangeState(CState_War_Run::GetInstance());
}

void CState_War_Idle::Exit(CGameObject* pOwner, _float fDeltaTime)
{

}

void CState_War_Idle::Free()
{
}


// ---------------------
// CState_War_Run
// ---------------------
IMPLEMENT_SINGLETON(CState_War_Run)

CState_War_Run::CState_War_Run()
{
}

void CState_War_Run::Enter(CGameObject* pOwner, _float fDeltaTime)
{
	// 애니메이션 Idx
	m_pModel = (CModel*)pOwner->Get_ComponentPtr(TEXT("Com_Model_War"));
	assert(m_pModel);
	Safe_AddRef(m_pModel);
	m_pModel->SetUp_Animation("War_Mesh.ao|War_Run_F");

	// War Transform
	m_pTransform = (CTransform*)pOwner->Get_ComponentPtr(TEXT("Com_Transform"));
	assert(m_pTransform);
	Safe_AddRef(m_pTransform);
}

void CState_War_Run::Execute(CGameObject* pOwner, _float fDeltaTime)
{
	unsigned char keyDownCheckBit = 0b0; 
	bool isKeyDown = false; 
	auto const dirtyCheck = [&isKeyDown, &keyDownCheckBit](_bool b) { isKeyDown |= b; };
	dirtyCheck(KeyCheck(DIK_A, keyDownCheckBit));
	dirtyCheck(KeyCheck(DIK_W, keyDownCheckBit));
	dirtyCheck(KeyCheck(DIK_D, keyDownCheckBit));
	dirtyCheck(KeyCheck(DIK_S, keyDownCheckBit));

	// 키4개 중 하나라도 눌러져있어야한다. 아니면 상태 IDLE로.
	if (isKeyDown == false) 
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
		return;
	}

	// 누른 키에 맞게 움직이자.
	// 1) 회전
	//m_pTransform->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(GetDegree(keyDownCheckBit)));
	m_pTransform->TurnTo_AxisY_Degree(GetDegree(keyDownCheckBit), fDeltaTime*10);
	
	// 2) 전진.
	m_pTransform->Go_Straight(fDeltaTime);
}

void CState_War_Run::Exit(CGameObject* pOwner, _float fDeltaTime)
{
	Safe_Release(m_pModel);
	Safe_Release(m_pTransform);
}


_float CState_War_Run::GetDegree(_ubyte downedKey)
{
	_float retDegree = 0.f;//12 Default는 정면을 바라보게하자.
	//_float offsetDegree = 90.f;

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

	return retDegree;// + offsetDegree;

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