#pragma once

#include "Base.h"
#include "Collider.h"


BEGIN(Engine)

class ENGINE_DLL CCollider_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollider_Manager)

public:
	union COLLIDER_ID
	{
		struct {
			UINT Src_id;
			UINT Dst_id;
		};
		ULONGLONG ID;
	};

private:
	CCollider_Manager() = default;
	virtual ~CCollider_Manager() = default;

public:
	void Add_Collision(class CGameObject* pGameObject);
	void Collision(float fTimeDelta);
	void Collision(class CGameObject* pSrc, class CGameObject* pDst, float fTimeDelta);
	bool CheckCollision(class CCollider * pSrc, class CCollider* pDst, float deltaTime);
	bool CheckCollisionList(class CCollider* pSrc, class CCollider* pDst);

	bool CollisionAABBToAABB(CCollider* pSrc, CCollider* pDst);
	bool CollisionAABBToOBB(CCollider* pSrc, CCollider* pDst);
	bool CollisionAABBToSPHERE(CCollider* pSrc, CCollider* pDst);
	

private:
	list<class CGameObject*> m_CollisionList;
	map <ULONGLONG, bool> m_mapColInfo;


public:
	virtual void Free() override;
};

END