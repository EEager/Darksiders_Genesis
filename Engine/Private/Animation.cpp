#include "..\Public\Animation.h"



CAnimation::CAnimation()
{
}

HRESULT CAnimation::NativeConstruct(char * pName, _double Duration, _double TickPerSecond)
{
	strcpy_s(m_szName, pName);
	m_Duration = Duration;
	m_TickPerSecond = TickPerSecond;

	return S_OK;
}

CAnimation * CAnimation::Create(char * pName, _double Duration, _double TickPerSecond)
{
	CAnimation*	pInstance = new CAnimation();

	if (FAILED(pInstance->NativeConstruct(pName, Duration, TickPerSecond)))
	{
		MSG_BOX("Failed To Creating CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimation::Free()
{
	
}
