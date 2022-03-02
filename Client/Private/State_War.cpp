#include "stdafx.h"
#include "State_War.h"

// Headers
#include "GameObject.h"
#include "Input_Device.h"
#include "StateMachine.h"
#include "Transform.h"
#include "Model.h"


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
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_RIGHT);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_LEFT);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_UP);
	dirty |= CInput_Device::GetInstance()->Key_Pressing(DIK_DOWN);
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
	bool isKeyDown = false; 
	if (CInput_Device::GetInstance()->Key_Pressing(DIK_RIGHT))
	{
		isKeyDown = true;
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fDeltaTime);
	}

	if (CInput_Device::GetInstance()->Key_Pressing(DIK_LEFT))
	{
		isKeyDown = true;
		m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fDeltaTime * -1.f);
	}

	if (CInput_Device::GetInstance()->Key_Pressing(DIK_UP))
	{
		isKeyDown = true;
		m_pTransform->Go_Straight(fDeltaTime);
	}

	if (CInput_Device::GetInstance()->Key_Pressing(DIK_DOWN))
	{
		isKeyDown = true;
		m_pTransform->Go_Backward(fDeltaTime);
	}

	if (isKeyDown == false) // 키4개 중 하나라도 눌러져있어야한다. 아니면 상태변환
	{
		g_pWar_State_Context->ChangeState(CState_War_Idle::GetInstance());
	}
}

void CState_War_Run::Exit(CGameObject* pOwner, _float fDeltaTime)
{
	Safe_Release(m_pModel);
	Safe_Release(m_pTransform);
	m_KeyUpCheckBit = 0b0;
}

void CState_War_Run::Free()
{
	Exit();
}