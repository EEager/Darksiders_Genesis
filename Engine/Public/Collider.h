#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

public:
	typedef struct ColliderDesc
	{
		_float3		vPivot;
		_float3		vSize;
	}COLLIDERDESC;

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

public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eType);
	virtual HRESULT NativeConstruct(void* pArg) override;

public:
	virtual HRESULT Render();

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

	_bool					m_isCollision = false;

	COLLIDERDESC			m_ColliderDesc;

private:
	TYPE					m_eType = TYPE_END;
	BasicEffect*			m_pEffect = nullptr;
	PrimitiveBatch<DirectX::VertexPositionColor>*	m_pBatch = nullptr;
	ID3D11InputLayout*		m_pInputLayout = nullptr;

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
	
};

END