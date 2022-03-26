#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
	friend class CGameObject;
	friend class CCollider_Manager;

public:
	enum COL_TYPE { COL_TYPE_AABB, COL_TYPE_OBB, COL_TYPE_SPHERE, COL_TYPE_END };

	// COLLISION_STATE
	enum CS_TYPE { CS_ENTER, CS_STAY, CS_LEAVE, CS_END };

public:
	typedef struct ColliderDesc
	{
		COL_TYPE	eColType;
		_float3		vPivot;
		_float3		vSize;
		_float		fRadius;
	}COLLIDERDESC;

	// For.OBB collision check
	typedef struct OBBDesc
	{
		_float3		vCenter;
		_float3		vExtentDirs[3];
		_float3		vAlignAxis[3];
	}OBBDESC;

private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;


	// ===================================
	// 내가 추가한 충돌보완
public:
	class CGameObject* Get_Owner(){	return m_pOwner;}
	void Set_Owner(class CGameObject* pOwner){m_pOwner = pOwner;}
	UINT Get_ID() { return m_iID; }
	COL_TYPE Get_ColliderType() { return m_ColliderDesc.eColType; }
	void Set_ColliderTag(const _tchar* p) { m_pColliderTag = p; }
	const _tchar* Get_ColliderTag() { return m_pColliderTag; }

	void OnCollision_Enter(class CCollider* pDst, _float fTimeDelta);
	void OnCollision_Stay(class CCollider* pDst, _float fTimeDelta);
	void OnCollision_Leave(class CCollider* pDst, _float fTimeDelta);

	bool Collider_Intersects(_In_ const CCollider* pCollider) const; 
	

private:
	static UINT g_iNextID;
	UINT			m_iID; // 충돌체 고유한 ID 값
	const _tchar* m_pColliderTag = L"";
	_bool m_bColliderDisble = true;
	// =====================


public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif

public:
	void Update(_fmatrix TransformMatrix);
	_bool Collision_AABB(class CCollider* pTargetCollider);
	_bool Collision_OBB(class CCollider* pTargetCollider);

private:
	_matrix Remove_Rotation(_fmatrix TransformMatrix);
	OBBDESC Compute_OBBDesc();


private:
	BoundingBox*			m_pAABB = nullptr;
	BoundingOrientedBox*	m_pOBB = nullptr;
	BoundingSphere*			m_pSphere = nullptr;
	COLLIDERDESC			m_ColliderDesc;

public:
	_bool					m_isCollision = false;
private:
	class CGameObject*		m_pOwner = nullptr;

private:
	BasicEffect*			m_pEffect = nullptr;
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = nullptr;
	ID3D11InputLayout*		m_pInputLayout = nullptr;

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
	
};

END