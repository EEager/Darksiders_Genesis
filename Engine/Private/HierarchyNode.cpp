#include "..\Public\HierarchyNode.h"
#include "Navigation.h"



CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::NativeConstruct(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth)
{
	strcpy_s(m_szName, pName);
	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
	XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	
	m_pParent = pParent;
	m_iDepth = iDepth;

	return S_OK;
}

HRESULT CHierarchyNode::Reserve_Channels(_uint iNumAnimation)
{
	m_Channels.resize(iNumAnimation);

	return S_OK;
}

void CHierarchyNode::Update_CombinedTransformationMatrix()
{
	if (nullptr != m_pParent) // 자식 노드 
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else // 루트 노드
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix));
}

void CHierarchyNode::Update_CombinedTransformationMatrix(IN _uint iCurrentAnimIndex, OUT _float4x4* pMatW, IN const char* pRootNodeName, IN CNavigation* pNaviCom, IN OBJECT_DIR eDir)
{
	// m_Channels에서 iCurrentAnimIndex의 애니메이션에 해당하는 뼈행렬 정보를 가지고 있다. 
	// 이 채널들은 Model 사본만들때 Model이 넣어준다. 
	if (iCurrentAnimIndex >= m_Channels.size())
		return;

	if (nullptr != m_Channels[iCurrentAnimIndex]) // iCurrentAnimIndex의 보간한 행렬을 가져온다.
	{
		// pMatW 받아서 로컬 애니메이션 움직인거리 만큼 월드행렬에 반영하고 싶다
		if (pMatW && pRootNodeName && !strcmp(m_szName, pRootNodeName))
		{
			// Bone_War_Root 이동값만 가져오자 -> Get_TransformationMatrix_4x4()->m[3] 
			_float4 offsetPos;
			memcpy(&offsetPos, &m_Channels[iCurrentAnimIndex]->Get_TransformationMatrix_4x4()->m[3], sizeof(_float4));
			if (offsetPos != m_prevOffsetPos)  // 이전오프셋위치와 다르고
			{
				// 첫번째 애니메이션이거나 offset에 변화가 없는 경우는 이전 오프셋값만 갱신한다
				if (m_Channels[iCurrentAnimIndex]->Get_KeyFrameIndex() == 0 || offsetPos == _float4(0.f, 0.f, 0.f, 1.f)) 
				{
					//if (XMVectorGetX(XMVector4Length(XMLoadFloat4(&offsetPos))) < 3.f) // Air_Land 같은것은 첫번째 키프레임인데 이미 오프셋 만큼 앞으로 나간 상태이다.
					m_prevOffsetPos = offsetPos;
				}
				else // 오프셋만큼 월행을 이동시켜주자
				{

					_float4 tmpOffset = {};
					// (현재오프셋위치 - 이전오프셋위치) 만큼 월드행렬에 더할꺼다.
					XMStoreFloat4(&tmpOffset, XMLoadFloat4(&offsetPos) - XMLoadFloat4(&m_prevOffsetPos));

					// 하지만 War Look 방향을 고려해야하기때문에 여기서 이동시킬 거리만 얻어오자
					_float offsetLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&tmpOffset)));

					// War Look방향으로 offsetLength 곱하여, tmpOffset에 저장하자.
					// 옆으로 가는 경우도 있기에 상황에 따라 움직이도록 해주자
					switch (eDir)
					{
					case DIR_L:
						XMStoreFloat4(&tmpOffset, XMVector4Normalize(XMLoadFloat4((_float4*)pMatW->m[CTransform::STATE_RIGHT])) * -offsetLength);
						break;
					case DIR_R:
						XMStoreFloat4(&tmpOffset, XMVector4Normalize(XMLoadFloat4((_float4*)pMatW->m[CTransform::STATE_RIGHT])) * offsetLength);
						break;
					case DIR_F:
						XMStoreFloat4(&tmpOffset, XMVector4Normalize(XMLoadFloat4((_float4*)pMatW->m[CTransform::STATE_LOOK])) * offsetLength);
						break;
					case DIR_B:
						XMStoreFloat4(&tmpOffset, XMVector4Normalize(XMLoadFloat4((_float4*)pMatW->m[CTransform::STATE_LOOK])) * -offsetLength);
						break;
					case DIR_U:
						XMStoreFloat4(&tmpOffset, XMVector4Normalize(XMLoadFloat4((_float4*)pMatW->m[CTransform::STATE_UP])) * offsetLength);
						break;
					default:
						assert(0);
						break;
					}

					// 최종적으로 pMatW에 tmpOffset를 적용한다.
					// 그러나 네비매쉬위에 없다면? 적용하지말자
					if (pNaviCom)
					{
						if (pNaviCom->isMove((XMLoadFloat4((_float4*)&pMatW->m[3]) + XMLoadFloat4(&tmpOffset))) == 1)
						{
							_matrix dstMat = XMLoadFloat4x4(pMatW) * XMMatrixTranslation(tmpOffset.x, tmpOffset.y, tmpOffset.z);
							XMStoreFloat4x4(pMatW, dstMat);
						}
					}
					else
					{
						_matrix dstMat = XMLoadFloat4x4(pMatW) * XMMatrixTranslation(tmpOffset.x, tmpOffset.y, tmpOffset.z);
						XMStoreFloat4x4(pMatW, dstMat);
					}
					// 이전 오프셋위치를 저장한다. 
					m_prevOffsetPos = offsetPos;

				}
			}

			// 로컬 애니메이션 위치(m_TransformationMatrix)는 0으로 고정하자.
			_float4x4 ZeroOffset;
			XMStoreFloat4x4(&ZeroOffset, m_Channels[iCurrentAnimIndex]->Get_TransformationMatrix());
			memcpy(ZeroOffset.m[3], &_float4(0.f, 0.f, 0.f, 1.f), sizeof(_float4));
			XMStoreFloat4x4(&m_TransformationMatrix, XMLoadFloat4x4(&ZeroOffset));
			// Test
			//m_Channels[iCurrentAnimIndex]->Set_TransformationMatrix(XMLoadFloat4x4(&ZeroOffset));
		}
		else
		{
			XMStoreFloat4x4(&m_TransformationMatrix, m_Channels[iCurrentAnimIndex]->Get_TransformationMatrix());
		}
	}

	if (nullptr != m_pParent)
	{
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	}
	else // 루트
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix));
}

CHierarchyNode * CHierarchyNode::Create(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth)
{
	CHierarchyNode*	pInstance = new CHierarchyNode();

	if (FAILED(pInstance->NativeConstruct(pName, TransformationMatrix, pParent, iDepth)))
	{
		MSG_BOX("Failed To Creating CHierarchyNode");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CHierarchyNode::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
