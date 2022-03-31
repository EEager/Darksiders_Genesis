#include "..\public\GameObject.h"
#include "Component.h"

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: m_pDeviceContext(pDeviceContext)
	, m_pDevice(pDevice)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);

}

#ifdef USE_IMGUI
int g_CloneIdx;
#endif
CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pDevice(rhs.m_pDevice)
	, m_tMtrlDesc(rhs.m_tMtrlDesc)
#ifdef USE_IMGUI
	, m_CloneIdx(g_CloneIdx++)
#endif
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CComponent * CGameObject::Get_ComponentPtr(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;	
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void* pArg)
{
	return S_OK;
}

_int CGameObject::Tick(_float fTimeDelta)
{
	return _int();
}

_int CGameObject::LateTick(_float fTimeDelta)
{
	return _int();
}

HRESULT CGameObject::Render(_uint iPassIndex)
{
	return S_OK;
}

HRESULT CGameObject::PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont)
{
	return S_OK;
}


HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg)
{
	CGameInstance*	pGameInstance = GET_INSTANCE(CGameInstance);

	CComponent*		pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.insert(COMPONENTS::value_type(pComponentTag, pComponent));

	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CGameObject::OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{

}

void CGameObject::OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{

}

void CGameObject::OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta)
{
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Release_Collider();

}
