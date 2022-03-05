#include "..\Public\State.h"

void CState::Enter(CGameObject* pOwner, _float dt)
{
#ifdef _DEBUG
	printf("-----------------------------\n");
	printf("[%s]::Enter\n", m_pStateName);
#endif // _DEBUG

}

void CState::Execute(CGameObject* pOwner, _float dt)
{
	m_fExecuteTime += dt;
}

void CState::Exit(CGameObject* pOwner, _float dt)
{
#ifdef _DEBUG
	printf("m_fExecuteTime : %.3f\n", m_fExecuteTime);
	printf("[%s]::Exit\n", m_pStateName);
#endif // _DEBUG
	m_fExecuteTime = 0.f;
}

void CState::Free()
{
}
