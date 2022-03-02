#include "..\Public\StateMachine.h"
#include "State.h"

CStateMachine::CStateMachine(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
}

CStateMachine::CStateMachine(const CStateMachine & rhs)
	: CComponent(rhs)
{
}

HRESULT CStateMachine::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CStateMachine::NativeConstruct(void * pArg)
{
	if (pArg == nullptr)
		return E_FAIL;

	STATEMACHINEDESC* pFsmDesc = (STATEMACHINEDESC*)pArg;

	m_pOwner = pFsmDesc->pOwner;
	m_pCurrentState = pFsmDesc->pInitState;
	m_pCurrentState->Enter(m_pOwner);

	return S_OK;
}


HRESULT CStateMachine::Tick(_float fDeltaTime)
{
	if (m_pGlobalState)
		m_pGlobalState->Execute(m_pOwner, fDeltaTime);

	if (m_pCurrentState)
		m_pCurrentState->Execute(m_pOwner, fDeltaTime);
	

	return S_OK;
}

HRESULT CStateMachine::ChangeState(CState* pNewState)
{
	if (m_pCurrentState == m_pPreviousState)
		return S_OK;

	assert(m_pCurrentState && pNewState);

	m_pPreviousState = m_pCurrentState;

	m_pCurrentState->Exit(m_pOwner);
	m_pCurrentState = pNewState;
	m_pCurrentState->Enter(m_pOwner);

	return S_OK;
}

HRESULT CStateMachine::RevertToPreviousState()
{
	ChangeState(m_pPreviousState);

	return S_OK;
}

CStateMachine * CStateMachine::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStateMachine*	pInstance = new CStateMachine(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed To Creating CStateMachine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CStateMachine::Clone(void * pArg)
{
	CStateMachine*	pInstance = new CStateMachine(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CStateMachine");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CStateMachine::Free()
{
	__super::Free();
}
