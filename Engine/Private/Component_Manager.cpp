#include "..\public\Component_Manager.h"
#include "Component.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
	
}

HRESULT CComponent_Manager::Reserve_Container(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == pPrototype ||
		iLevelIndex >= m_iNumLevels || 
		nullptr != Find_Component(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void* pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CComponent*	pPrototype = Find_Component(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent*	pComponent = pPrototype->Clone(pArg);	
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;	
}

HRESULT CComponent_Manager::Clear(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pPrototypes[iLevelIndex])
		Safe_Release(Pair.second);

	m_pPrototypes[iLevelIndex].clear();

	return S_OK;
}

CComponent * CComponent_Manager::Find_Component(_uint iLevelIndex, const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTagFinder(pPrototypeTag));

	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}
	Safe_Delete_Array(m_pPrototypes);

}
