#pragma once

#include "Base.h"
#include "Collider.h"

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
	/* ������ü�� �ʱ�ȭ�ϱ����� �Լ�. (����,���Ϸκ��� �����͸� �о ���夾����. ) */
	virtual HRESULT NativeConstruct_Prototype();

	/* �纻��ü�� �ʱ�ȭ�ϱ����� �Լ�. (������ü�� �����͸� �޸� �����Ѵ�. ) */
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();
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
	// Collider Component List�� ������������
	list <class CCollider*>				m_ColliderList;
	typedef list <class CCollider*>		COLLIDERS;

protected:
	_bool isColliderListEmpty() { return m_ColliderList.empty(); }
	HRESULT Add_Collider(class CCollider* pCollider)
	{
		m_ColliderList.push_back(pCollider);
		return S_OK;
	}
	void Release_Collider()
	{
		for (auto pCollider : m_ColliderList)
		{
			Safe_Release(pCollider);
		}
		m_ColliderList.clear();
	}

	_int Update_Colliders(_matrix wolrdMatrix = XMMatrixIdentity())
	{
		for (auto pCollider : m_ColliderList)
		{
			pCollider->Update(wolrdMatrix);
		}
		return 0;
	}

	_int Render_Colliders()
	{
		for (auto pCollider : m_ColliderList)
		{
			pCollider->Render();
		}
		return 0;
	}
	// ======================================================


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
	/* ������ü�� ȣ���Ѵ�. */
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END