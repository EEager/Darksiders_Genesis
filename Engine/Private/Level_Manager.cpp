#include "..\public\Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{

}

HRESULT CLevel_Manager::Open_Level(_uint iLevelIndex, CLevel * pNextLevel)
{
	if (nullptr == pNextLevel)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if(nullptr != m_pCurrentLevel)
		pGameInstance->Clear_Level(m_iCurrentLevelIndex);

	RELEASE_INSTANCE(CGameInstance);

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNextLevel;

	m_iCurrentLevelIndex = iLevelIndex;

	return S_OK;
}

_int CLevel_Manager::Tick(_float fTimeDelta)
{
	if (nullptr == m_pCurrentLevel)
		return -1;

	m_pCurrentLevel->Tick(fTimeDelta);

	return _int();
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();	
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
