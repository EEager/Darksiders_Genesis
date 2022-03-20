#pragma once

#include "Base.h"
#include "Collider.h"
#include "GameInstance.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;
public:
	class CComponent* Get_ComponentPtr(const _tchar* pComponentTag);
	_float Get_CamDistance() const {
		return m_fCamDistance;
	}
public:
	/* 원형객체를 초기화하기위한 함수. (서버,파일로부터 데이터를 읽어서 저장ㅎ나다. ) */
	virtual HRESULT NativeConstruct_Prototype();

	/* 사본객체를 초기화하기위한 함수. (원형객체의 데이터를 메모리 복사한다. ) */
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

public:
	HRESULT Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

	unordered_map<const _tchar*, class CComponent*>				m_Components;
	typedef unordered_map<const _tchar*, class CComponent*>		COMPONENTS;


	
	// ======================================================
	// Collider  
protected:
	// Collider Component List를 가지도록하자
	list <class CCollider*>				m_ColliderList;
	typedef list <class CCollider*>		COLLIDERS;

public:
	list <class CCollider*>* Get_ColliderList()
	{
		return &m_ColliderList;
	}

	_bool isColliderListEmpty() { return m_ColliderList.empty(); }
	HRESULT Add_Collider(CCollider::COLLIDERDESC* ColliderDesc, const _tchar* pColliderTag, _uint iLevel = 0, const _tchar* pColliderPrototypeTag = L"Prototype_Component_Collider")
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		CCollider* pCollider = static_cast<CCollider*>(pGameInstance->Clone_Component(iLevel, pColliderPrototypeTag, ColliderDesc));
		pCollider->Set_Owner(this);
		pCollider->Set_ColliderTag(pColliderTag);
		m_ColliderList.push_back(pCollider);
		RELEASE_INSTANCE(CGameInstance);
		return S_OK;
	}

	virtual void Release_Collider()
	{
		for (auto pCollider : m_ColliderList)
		{
			Safe_Release(pCollider);
		}
		m_ColliderList.clear();
	}

	virtual _int Update_Colliders(_matrix wolrdMatrix = XMMatrixIdentity())
	{
		for (auto pCollider : m_ColliderList)
		{
			pCollider->Update(wolrdMatrix);
		}
		return 0;
	}

	virtual _int Render_Colliders()
	{
		for (auto pCollider : m_ColliderList)
		{
			pCollider->Render();
		}
		return 0;
	}

public:
	virtual void OnCollision_Enter(class CGameObject* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(class CGameObject* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(class CGameObject* pDst, float fTimeDelta);
	// =======================================================================

	// For Test Shadow Map 
public:
	_bool m_bShadowEnable = false;



protected:
	_float					m_fCamDistance = 0.f;

private:
	CComponent* Find_Component(const _tchar* pComponentTag);

protected:
	MTRLDESC m_tMtrlDesc;


	// Used In ImGUI
public:
	const _tchar* Get_LayerTag() { return m_pLayerTag; }
	void Set_LayerTag(const _tchar* pPrototypeTag) { m_pLayerTag = pPrototypeTag; }

	const _tchar* Get_PrototypeTag() { return m_pPrototypeTag; }
	void Set_PrototypeTag(const _tchar* pPrototypeTag) { m_pPrototypeTag = pPrototypeTag; }

protected:
	const _tchar* m_pPrototypeTag = nullptr;
	const _tchar* m_pLayerTag = nullptr;

public:
	/* 원형객체가 호출한다. */
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END