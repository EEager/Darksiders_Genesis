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

	// iter 2���� �̿��Ͽ� m_CollisionList�� ��ϵ� GameObject�� ��ƴ���� ��ȸ���� 
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

	// ������ iter�� release
	// ex) 0 1 2 3 -> �ΰ�� 0 1 2 ���� ������ release�� �ߴ�. ���⼭ 3�� release����
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

	// CGameObject�� ������ �ִ� CCollider ���� �浹üũ�� Ȯ���ϴ�
	for (auto& pSrc : *pSrcList)
	{
		for (auto& pDst : *pDstList)
		{
			COLLIDER_ID ID;
			ID.Src_id = pSrc->Get_ID();
			ID.Dst_id = pDst->Get_ID();
			map<ULONGLONG, bool>::iterator iter = m_mapColInfo.find(ID.ID);
			if (iter == m_mapColInfo.end()) // ó�� ��ϵ� ����
			{
				m_mapColInfo.insert(make_pair(ID.ID, false));
				iter = m_mapColInfo.find(ID.ID);
			}

			if (CheckCollision(pSrc, pDst, fTimeDelta)) // ���� �浹�ߴ�.
			{
				// �浹��Ͽ��� ������ �浹������ ���ٸ�
				// #1. ó�� �� �浹�� ����
				if (iter->second == false)
				{
					// ���� ������ �浹 ������� �߰��Ѵ�. 
					iter->second = true;

					// ������Ʈ���� �ݶ��̴��ݹ��Լ��� ȣ������
					pSrc->OnCollision_Enter(pDst, fTimeDelta);
					pDst->OnCollision_Enter(pSrc, fTimeDelta);

				}
				// #2. ���� �浹������ �ִٸ� ��� �浹 ����
				else
				{
					// ������Ʈ���� �ݶ��̴��ݹ��Լ��� ȣ������
					pSrc->OnCollision_Stay(pDst, fTimeDelta);
					pDst->OnCollision_Stay(pSrc, fTimeDelta);
				}
			}
			// ���� �浹�� �ȵ� ���¿���, ������ �浹�� �ǰ� �־��ٸ�, 
			// #3. ���� �� �浹���� ������ ����
			else if (iter->second == true)
			{
				// ���� �浹�� �ȵǹǷ� �浹��Ͽ��� �����ش�. 
				iter->second = false;

				// ������Ʈ���� �ݶ��̴��ݹ��Լ��� ȣ������
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
	//		bCollision = CollisionRectToSphere(pDst, pSrc); //��
	//	}
	//	else if (pDst->Get_ColliderType() == COLLIDER_TYPE::CT_POINT) // ����Ʈ �� ����Ʈ�� ���̵ǳİ� ���� 
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

