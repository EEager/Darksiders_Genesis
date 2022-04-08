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

HRESULT CObject_Manager::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pPrototypeTag, void * pArg, OUT CGameObject** pGameObjectClone)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CGameObject*	pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject*	pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	*pGameObjectClone = pGameObject;

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

HRESULT CObject_Manager::Add_GameObjectToLayer(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pPrototypeTag, void* pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

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
	// iLevelIndex == -1 �� ���, ���� ���� ��ȸ�ϸ� pLayerTag�� �ش��ϴ� CLayer�� List�� �����Ѵ�. 
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

	// iLevelIndex == -1 �� ���, ���� ���� ��ȸ�ϸ� pLayerTag�� �ش��ϴ� CLayer�� List�� �����Ѵ�. 
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
	// iLevelIndex != -1 �� ���, �ش� ���� ������ �����´�.
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


// pLayerTag �� �ִ� ������Ʈ���� ��ȸ�ϸ� ������ �����Ѵ�.
HRESULT CObject_Manager::Save_ObjectsToFile(const _tchar* pLayerTag, _uint iLevelIndex)
{ 
	_tchar pFilePath[MAX_PATH] = L"";
	wsprintfW(pFilePath, L"../Bin/Data/ObjectData/%s.dat", pLayerTag);

	if (iLevelIndex > m_iNumLevels)
		return E_FAIL;

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, L"INVALID_HANDLE_VALUE", L"Save ���� �� �����", MB_OK);
		return E_FAIL;
	}

	DWORD dwBytes = 0;
	auto pLayer = m_pLayers[iLevelIndex].find(pLayerTag);
	for (auto& gameObject : *(*pLayer).second->Get_List_Adr())
	{
		// #1.
		// ���̾� �±� ���� ex) Layer_Tile, Layer_Monster
		_tchar saveBuf[MAX_PATH] = L"";
		lstrcpyW(saveBuf, gameObject->Get_LayerTag());
		::WriteFile(hFile, &saveBuf, sizeof(saveBuf), &dwBytes, nullptr);

		// #2.
		// ������Ÿ�� ���� 
		lstrcpyW(saveBuf, gameObject->Get_PrototypeTag());
		::WriteFile(hFile, &saveBuf, sizeof(saveBuf), &dwBytes, nullptr);

		// #3.
		// ���� ��� ����
		CTransform* pTransform = (CTransform*)gameObject->Get_ComponentPtr(L"Com_Transform");
		if (pTransform)
		{
			_float4x4* vPos = pTransform->Get_WorldMatrix_4x4();
			::WriteFile(hFile, vPos, sizeof(_float4x4), &dwBytes, nullptr);
		}
		else
		{
			assert(0);
		}
	}

	::CloseHandle(hFile);

	return S_OK;
}

// ���۾��� �±׸� ���ͳ� �����ͷ� �����Ͽ� �� ������Ʈ���� ������ �մ� ��������� �Ҵ�������
const _tchar* ToLiteralTag(const _tchar* pTag)
{
	if (!lstrcmpW(pTag, L"Prototype_GameObject_Fork")) return L"Prototype_GameObject_Fork";
	if (!lstrcmpW(pTag, L"Layer_Fork")) return L"Layer_Fork";

	if (!lstrcmpW(pTag, L"Layer_Ballista"))	return L"Layer_Ballista";
	if (!lstrcmpW(pTag, L"Prototype_GameObject_Ballista"))	return L"Prototype_GameObject_Ballista";

	if (!lstrcmpW(pTag, L"Layer_Breakables"))	return L"Layer_Breakables";
	if (!lstrcmpW(pTag, L"Layer_Breakables_1"))	return L"Layer_Breakables_1";
	if (!lstrcmpW(pTag, L"Prototype_GameObject_Breakable1"))	return L"Prototype_GameObject_Breakable1";
	if (!lstrcmpW(pTag, L"Prototype_GameObject_Breakable2"))	return L"Prototype_GameObject_Breakable2";
	if (!lstrcmpW(pTag, L"Prototype_GameObject_Breakable3"))	return L"Prototype_GameObject_Breakable3";
	if (!lstrcmpW(pTag, L"Prototype_GameObject_Breakable4"))	return L"Prototype_GameObject_Breakable4";
	if (!lstrcmpW(pTag, L"Prototype_GameObject_Breakable5"))	return L"Prototype_GameObject_Breakable5";

	assert(0);

	return nullptr;
}

// ������ �о� ��ü�� Ŭ���Ѵ�.
HRESULT CObject_Manager::Load_ObjectsFromFile(const _tchar* pLayerTag, _uint iLevelIndex)
{
	_tchar pFilePath[MAX_PATH] = L"";
	wsprintfW(pFilePath, L"../Bin/Data/ObjectData/%s.dat", pLayerTag);

	HANDLE hFile = ::CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(NULL, pFilePath, L"Load_ObjectsFromFile ���� �������", MB_OK);
	}
	// ����Ȱ� ������ �ҷ�����
	DWORD dwBytes = 0;

	while (true)
	{
		// #1.
		// ���̾� �±� ex) Layer_Tile, Layer_Monster
		_tchar layerTag[MAX_PATH] = L"";
		::ReadFile(hFile, &layerTag, sizeof(layerTag), &dwBytes, nullptr);
		if (0 == dwBytes)
			break;

		// #2.
		// ������Ÿ�� �ε� 
		_tchar prototypeTag[MAX_PATH] = L"";
		::ReadFile(hFile, &prototypeTag, sizeof(prototypeTag), &dwBytes, nullptr);

#if 1 // HARD CODING
		// �̷��� ���� �����͸� �����ָ� ���� ���������鼭 ������ ����� ���ͳ� �����ͷ� ���������. 
		const _tchar* pLayerTag = ToLiteralTag(layerTag);
		const _tchar* pPrototypeTag = ToLiteralTag(prototypeTag);
#endif
		CGameObject* pGameObject = nullptr;
		Add_GameObjectToLayer(iLevelIndex, pLayerTag, pPrototypeTag, nullptr, &pGameObject);

		// #3. ���� ����
		_float4x4 vWorldMat;
		::ReadFile(hFile, &vWorldMat, sizeof(_float4x4), &dwBytes, nullptr);
		CTransform* pTransform = (CTransform*)pGameObject->Get_ComponentPtr(TEXT("Com_Transform"));
		if (pTransform)
			pTransform->Set_WorldMatrix(XMLoadFloat4x4(&vWorldMat));
	}

	::CloseHandle(hFile);

	return S_OK;
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
