#include "..\public\Layer.h"
#include "GameObject.h"
#include "Component.h"

CLayer::CLayer()
{
}

CComponent * CLayer::Get_ComponentPtr(const _tchar * pComponentTag, _uint iIndex)
{
	auto	iter = m_Objects.begin();

	for (_uint i = 0; i < iIndex; ++i)
		++iter;

	return (*iter)->Get_ComponentPtr(pComponentTag);	
}

HRESULT CLayer::NativeConstruct()
{
	return S_OK;
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_Objects.push_back(pGameObject);

	return S_OK;
}

_int CLayer::Tick(_float fTimeDelta)
{
	_int	iProgress = 0;

	for (auto& pGameObject : m_Objects)
	{
		if (nullptr != pGameObject)
		{
			iProgress = pGameObject->Tick(fTimeDelta);
			if (0 > iProgress)
				return -1;
		}
	}		

	return _int();
}

_int CLayer::LateTick(_float fTimeDelta)
{
	_int	iProgress = 0;

	for (auto& pGameObject : m_Objects)
	{
		if (nullptr != pGameObject)
		{
			iProgress = pGameObject->LateTick(fTimeDelta);
			if (0 > iProgress)
				return -1;
		}
	}

	return 0;
}

CLayer * CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX("Failed To Creating CLayer");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_Objects)
	{
		Safe_Release(pGameObject);
	}
	m_Objects.clear();
}