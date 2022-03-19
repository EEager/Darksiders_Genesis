#include "..\public\Collider_Manager.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollider_Manager)


void CCollider_Manager::Add_Collision(CGameObject* pGameObject)
{
	if (pGameObject->isColliderListEmpty())
		return;

	Safe_AddRef(pGameObject);
	m_CollisionList.push_back(pGameObject);
}

void CCollider_Manager::Collision(float fTimeDelta)
{
	if (m_CollisionList.empty())
		return;

	if (m_CollisionList.size() == 1)
	{
		Safe_Release(m_CollisionList.front());
		m_CollisionList.clear();
		return;
	}

	// iter 2개를 이용하여 m_CollisionList에 등록된 GameObject를 빈틈없이 순회하자 
	list<CGameObject*>::iterator iter;
	list<CGameObject*>::iterator iterEnd = m_CollisionList.end();
	--iterEnd;

	for (iter = m_CollisionList.begin(); iter != iterEnd; ++iter)
	{
		auto iter1 = iter; ++iter1;
		auto iter1End = m_CollisionList.end();
		for (; iter1 != iter1End; ++iter1)
		{
			Collision(*iter, *iter1, fTimeDelta);
		}
		Safe_Release(*iter);
	}

	// 마지막 iter도 release
	// ex) 0 1 2 3 -> 인경우 0 1 2 까지 위에서 release를 했다. 여기서 3을 release하자
	Safe_Release(*iter);


	m_CollisionList.clear();
}

void CCollider_Manager::Collision(CGameObject* pSrc, CGameObject* pDst, float fTimeDelta)
{
	const list<CCollider*>* pSrcList = pSrc->Get_ColliderList();
	if (pSrcList->empty())
		return;

	const list<CCollider*>* pDstList = pDst->Get_ColliderList();
	if (pDstList->empty())
		return;

	// CGameObject가 가지고 있는 CCollider 끼리 충돌체크를 확인하다
	for (auto& pSrc : *pSrcList)
	{
		for (auto& pDst : *pDstList)
		{
			COLLIDER_ID ID;
			ID.Src_id = pSrc->Get_ID();
			ID.Dst_id = pDst->Get_ID();
			map<ULONGLONG, bool>::iterator iter = m_mapColInfo.find(ID.ID);
			if (iter == m_mapColInfo.end()) // 처음 등록된 상태
			{
				m_mapColInfo.insert(make_pair(ID.ID, false));
				iter = m_mapColInfo.find(ID.ID);
			}

			if (CheckCollision(pSrc, pDst, fTimeDelta)) // 둘이 충돌했다.
			{
				// 충돌목록에서 이전에 충돌된적이 없다면
				// #1. 처음 막 충돌된 상태
				if (iter->second == false)
				{
					// 서로 상대방을 충돌 목록으로 추가한다. 
					iter->second = true;

					// 오브젝트들의 콜라이더콜백함수를 호출하자
					pSrc->OnCollision_Enter(pDst, fTimeDelta);
					pDst->OnCollision_Enter(pSrc, fTimeDelta);

				}
				// #2. 기존 충돌된적이 있다면 계속 충돌 상태
				else
				{
					// 오브젝트들의 콜라이더콜백함수를 호출하자
					pSrc->OnCollision_Stay(pDst, fTimeDelta);
					pDst->OnCollision_Stay(pSrc, fTimeDelta);
				}
			}
			// 현재 충돌이 안된 상태에서, 이전에 충돌이 되고 있었다면, 
			// #3. 이제 막 충돌에서 떨어진 상태
			else if (iter->second == true)
			{
				// 서로 충돌이 안되므로 충돌목록에서 지워준다. 
				iter->second = false;

				// 오브젝트들의 콜라이더콜백함수를 호출하자
				pSrc->OnCollision_Leave(pDst, fTimeDelta);
				pDst->OnCollision_Leave(pSrc, fTimeDelta);
			}
		}
	}
}

bool CCollider_Manager::CheckCollision(CCollider* pSrc, CCollider* pDst, float deltaTime)
{
	bool bCollision = false;

	// AABB vs
	if (pSrc->Get_ColliderType() == CCollider::COL_TYPE::COL_TYPE_AABB)
	{
		if (pDst->Get_ColliderType() == CCollider::COL_TYPE::COL_TYPE_AABB)
		{
			bCollision = CollisionAABBToAABB(pSrc, pDst);
		}

		else if (pDst->Get_ColliderType() == CCollider::COL_TYPE::COL_TYPE_OBB)
		{
			bCollision = CollisionAABBToOBB(pSrc, pDst);
		}

		else if (pDst->Get_ColliderType() == CCollider::COL_TYPE::COL_TYPE_SPHERE)
		{
			bCollision = CollisionAABBToSPHERE(pSrc, pDst);
		}
	}

	// OBB vs
	else if (pSrc->Get_ColliderType() == CCollider::COL_TYPE::COL_TYPE_OBB)
	{
		if (pDst->Get_ColliderType() == CCollider::COL_TYPE::COL_TYPE_AABB)
		{
			bCollision = CollisionAABBToOBB(pDst, pSrc);
		}

		else if (pDst->Get_ColliderType() == CCollider::COL_TYPE::COL_TYPE_OBB)
		{
			bCollision = CollisionOBBToOBB(pSrc, pDst);
		}

		else if (pDst->Get_ColliderType() == CCollider::COL_TYPE::COL_TYPE_SPHERE)
		{
			bCollision = CollisionOBBToSPHERE(pSrc, pDst);
		}
	}


	//else if (pSrc->Get_ColliderType() == COLLIDER_TYPE::CT_SPHERE)
	//{
	//	if (pDst->Get_ColliderType() == COLLIDER_TYPE::CT_RECT)
	//	{
	//		bCollision = CollisionRectToSphere(pDst, pSrc); //역
	//	}
	//	else if (pDst->Get_ColliderType() == COLLIDER_TYPE::CT_POINT) // 포인트 투 포인트가 말이되냐고 ㅋㅋ 
	//	{
	//		bCollision = CollisionSphereToPoint(pSrc, pDst);
	//	}
	//	else if (pDst->Get_ColliderType() == COLLIDER_TYPE::CT_SPHERE)
	//	{
	//		bCollision = CollisionSphereToShpere(pSrc, pDst);
	//	}
	//}

	return bCollision;
}


bool CCollider_Manager::CollisionAABBToAABB(CCollider* pSrc, CCollider* pDst)
{
	return pSrc->Collision_AABB(pDst);
}

bool CCollider_Manager::CollisionAABBToOBB(CCollider* pSrc, CCollider* pDst)
{
	return false;
}

bool CCollider_Manager::CollisionAABBToSPHERE(CCollider* pSrc, CCollider* pDst)
{
	return false;
}


bool CCollider_Manager::CollisionOBBToOBB(CCollider* pSrc, CCollider* pDst)
{
	return pSrc->Collision_OBB(pDst);
}

bool CCollider_Manager::CollisionOBBToSPHERE(CCollider* pSrc, CCollider* pDst)
{
	return false;
}




void CCollider_Manager::Free()
{
	for (auto pGameObject : m_CollisionList)
	{
		Safe_Release(pGameObject);
	}
	m_CollisionList.clear();
}

