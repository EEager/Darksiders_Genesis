#include "..\public\Object_Manager.h"
#include "GameObject.h"
#include "Component.h"
#include "Layer.h"


IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

CComponent * CObject_Manager::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Get_ComponentPtr(pComponentTag, iIndex);	
}

HRESULT CObject_Manager::Reserve_Container(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_iNumLevels = iNumLevels;

	m_pLayers = new LAYERS[iNumLevels];
	
	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == pPrototype)
		return E_FAIL;

	if (nullptr != Find_Prototype(pPrototypeTag))
	{
		Safe_Release(pPrototype);
		return S_OK;
	}

	m_Prototypes.insert(PROTOTYPES::value_type(pPrototypeTag, pPrototype));

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*		pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		if (nullptr == pLayer)
			return E_FAIL;

		pLayer->Add_GameObject(pGameObject);

		m_pLayers[iLevelIndex].insert(LAYERS::value_type(pLayerTag, pLayer));
	}
	else
		pLayer->Add_GameObject(pGameObject);


	// Set PrototypeTag n LayerTag
	pGameObject->Set_PrototypeTag(pPrototypeTag);
	pGameObject->Set_LayerTag(pLayerTag);


	return S_OK;
}

_int CObject_Manager::Tick(_float fTimeDelta)
{
	_int iProgress = 0;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i]) 
		{
			iProgress = Pair.second->Tick(fTimeDelta);
			if (0 > iProgress)
				return -1;
		}
	}
	return 0;
}

_int CObject_Manager::LateTick(_float fTimeDelta)
{
	_int iProgress = 0;

	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			iProgress = Pair.second->LateTick(fTimeDelta);
			if (0 > iProgress)
				return -1;
		}
	}
	return 0;
}

HRESULT CObject_Manager::Clear_LevelLayers(_uint iLevelIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

// For.Client
CGameObject* CObject_Manager::Get_War(int iLevelIndex)
{
	// iLevelIndex == -1 인 경우, 레벨 전부 순회하며 pLayerTag에 해당하는 CLayer의 List를 리턴한다. 
	if (iLevelIndex == -1)
	{
		for (_uint i = 0; i < m_iNumLevels; i++)
		{
			auto	iterFind = find_if(m_pLayers[i].begin(), m_pLayers[i].end(), CTagFinder(L"Layer_War"));
			if (iterFind != m_pLayers[i].end())
			{
				return (*iterFind->second)[0];
			}
		}
	}
	else
	{
		auto	iterFind = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(L"Layer_War"));
		if (iterFind != m_pLayers[iLevelIndex].end())
		{
			return (*iterFind->second)[0];
		}
	}
	return nullptr;
}

list<CGameObject*>* CObject_Manager::Get_GameObject_CloneList(const _tchar* pLayerTag, int iLevelIndex)
{
	if (iLevelIndex >= (int)m_iNumLevels)
		return nullptr;

	// iLevelIndex == -1 인 경우, 레벨 전부 순회하며 pLayerTag에 해당하는 CLayer의 List를 리턴한다. 
	if (iLevelIndex == -1)
	{
		for (_uint i = 0; i < m_iNumLevels; i++)
		{
			auto	iterFind = find_if(m_pLayers[i].begin(), m_pLayers[i].end(), CTagFinder(pLayerTag));
			if (iterFind != m_pLayers[i].end())
			{
				return iterFind->second->Get_List_Adr();
			}
		}
	}
	// iLevelIndex != -1 인 경우, 해당 레벨 정보만 가져온다.
	else
	{
		auto	iterFind = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
		if (iterFind != m_pLayers[iLevelIndex].end())
		{
			return iterFind->second->Get_List_Adr();

		}
	}

	return nullptr;
}

unordered_map<const _tchar*, class CGameObject*>* CObject_Manager::Get_GameObject_PrototypeUMap()
{
	return &m_Prototypes;;
}

unordered_map<const _tchar*, class CLayer*>* CObject_Manager::Get_GameObject_LayerUMap()
{
	return m_pLayers;
}

CGameObject * CObject_Manager::Find_Prototype(const _tchar * pPrototypeTag)
{
	auto	iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTagFinder(pPrototypeTag));

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;	
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar * pLayerTag)
{
	auto	iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));

	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}


void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();
	
}
