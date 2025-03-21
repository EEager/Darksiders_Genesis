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
		if (pSrc->m_bColliderDisble)
			continue;
		for (auto& pDst : *pDstList)
		{
			if (pDst->m_bColliderDisble)
				continue;

			COLLIDER_ID ID;
			map<ULONGLONG, bool>::iterator iter;
			ID.Src_id = pSrc->Get_ID();
			ID.Dst_id = pDst->Get_ID();
			iter = m_mapColInfo.find(ID.ID);
			if (iter == m_mapColInfo.end()) // 처음 등록된 상태
			{
				m_mapColInfo.insert(make_pair(ID.ID, false));
				iter = m_mapColInfo.find(ID.ID);
			}
			else // 기록이 있는 상태 
			{
				// 상황 : War칼이 바리스타 몸통 콜라이더 안에 있는경우, 계속 공격해도 공격판정이안된다.
				// 콜라이더 하나라도 다시 Enter 콜이 필요한경우, m_mapColInfo 기록을 말소해주자.
				if (pSrc->m_bColliderNeedEnter || pDst->m_bColliderNeedEnter)
				{
					iter->second = false; // 말소해주자
				}
			}

			if (CheckCollision(pSrc, pDst, fTimeDelta)) // 둘이 충돌했다.
			{
				// 만약 충돌상태에서 한쪽이 죽은 상태라면, LEAVE 콜백 해주도록 하자
				if (pSrc->Get_Owner()->IsDead() || pDst->Get_Owner()->IsDead())
				{
					// 오브젝트들의 LEAVE 콜라이더콜백함수를 호출하자
					pSrc->OnCollision_Leave(pDst, fTimeDelta);
					pDst->OnCollision_Leave(pSrc, fTimeDelta);
					continue; 
				}

				// 충돌목록에서 이전에 충돌된적이 없다면
				// #1. 처음 막 충돌된 상태
				if (iter->second == false)
				{
					// 서로 상대방을 충돌 목록으로 추가한다.  
					iter->second = true;

					pDst->m_bColliderNeedEnter = false;

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

	bCollision = pSrc->Collider_Intersects(pDst);
	
	return bCollision;
}

void CCollider_Manager::Free()
{
	for (auto pGameObject : m_CollisionList)
	{
		Safe_Release(pGameObject);
	}
	m_CollisionList.clear();
}

