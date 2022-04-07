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
	/* ������ü�� �ʱ�ȭ�ϱ����� �Լ�. (����,���Ϸκ��� �����͸� �о ���夾����. ) */
	virtual HRESULT NativeConstruct_Prototype();

	/* �纻��ü�� �ʱ�ȭ�ϱ����� �Լ�. (������ü�� �����͸� �޸� �����Ѵ�. ) */
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
	// Collider Component List�� ������������
	list <class CCollider*>				m_ColliderList;
	typedef list <class CCollider*>		COLLIDERS;

public:
	list <class CCollider*>* Get_ColliderList()	{ return &m_ColliderList; }
	_bool isColliderListEmpty() { return m_ColliderList.empty(); }
	HRESULT Add_Collider(CCollider::COLLIDERDESC* ColliderDesc, const _tchar* pColliderTag, bool initialDisable = false, _uint iLevel = 0, const _tchar* pColliderPrototypeTag = L"Prototype_Component_Collider");
	void Set_Collider_Attribute(const _tchar* pColliderTag/*define ���� �־�����*/, _bool disable); // Ư�� Tag�� �ݶ��̴� �Ӽ��� �����Ѵ�.
	virtual void Release_Collider();
	virtual _int Update_Colliders(_matrix wolrdMatrix = XMMatrixIdentity());
	virtual _int Render_Colliders();

public:
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	// =======================================================================

	// ======================================================
	// GameInfo
public:
	GAMEINFO m_tGameInfo = {};
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

#ifdef USE_IMGUI
	// ImGUI���� m_bUseImGui�� true�� �ٲٸ� m_CloneIdx ���� ���еǴ� ImGUI:Begin â�� ��������
public:
	bool m_bUseImGui = false;
protected:
	int m_CloneIdx = 0;
#endif

	// Render ��������
public:
	void Set_NotRender(_bool b) { m_bNotRender = b; }

protected:
	_bool m_bNotRender = false;

protected:
	const _tchar* m_pPrototypeTag = nullptr;
	const _tchar* m_pLayerTag = nullptr;

public:
	/* ������ü�� ȣ���Ѵ�. */
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END