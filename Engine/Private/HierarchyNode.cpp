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
	if (nullptr != m_pParent) // �ڽ� ��� 
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else // ��Ʈ ���
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix));
}

void CHierarchyNode::Update_CombinedTransformationMatrix(IN _uint iCurrentAnimIndex, OUT _float4x4* pMatW, IN const char* pRootNodeName, IN CNavigation* pNaviCom, IN OBJECT_DIR eDir)
{
	// m_Channels���� iCurrentAnimIndex�� �ִϸ��̼ǿ� �ش��ϴ� ����� ������ ������ �ִ�. 
	// �� ä�ε��� Model �纻���鶧 Model�� �־��ش�. 
	if (iCurrentAnimIndex >= m_Channels.size())
		return;

	if (nullptr != m_Channels[iCurrentAnimIndex]) // iCurrentAnimIndex�� ������ ����� �����´�.
	{
		// pMatW �޾Ƽ� ���� �ִϸ��̼� �����ΰŸ� ��ŭ ������Ŀ� �ݿ��ϰ� �ʹ�
		if (pMatW && pRootNodeName && !strcmp(m_szName, pRootNodeName))
		{
			// Bone_War_Root �̵����� �������� -> Get_TransformationMatrix_4x4()->m[3] 
			_float4 offsetPos;
			memcpy(&offsetPos, &m_Channels[iCurrentAnimIndex]->Get_TransformationMatrix_4x4()->m[3], sizeof(_float4));
			if (offsetPos != m_prevOffsetPos)  // ������������ġ�� �ٸ���
			{
				// ù��° �ִϸ��̼��̰ų� offset�� ��ȭ�� ���� ���� ���� �����°��� �����Ѵ�
				if (m_Channels[iCurrentAnimIndex]->Get_KeyFrameIndex() == 0 || offsetPos == _float4(0.f, 0.f, 0.f, 1.f)) 
				{
					//if (XMVectorGetX(XMVector4Length(XMLoadFloat4(&offsetPos))) < 3.f) // Air_Land �������� ù��° Ű�������ε� �̹� ������ ��ŭ ������ ���� �����̴�.
					m_prevOffsetPos = offsetPos;
				}
				else // �����¸�ŭ ������ �̵���������
				{

					_float4 tmpOffset = {};
					// (�����������ġ - ������������ġ) ��ŭ ������Ŀ� ���Ҳ���.
					XMStoreFloat4(&tmpOffset, XMLoadFloat4(&offsetPos) - XMLoadFloat4(&m_prevOffsetPos));

					// ������ War Look ������ ����ؾ��ϱ⶧���� ���⼭ �̵���ų �Ÿ��� ������
					_float offsetLength = XMVectorGetX(XMVector4Length(XMLoadFloat4(&tmpOffset)));

					// War Look�������� offsetLength ���Ͽ�, tmpOffset�� ��������.
					// ������ ���� ��쵵 �ֱ⿡ ��Ȳ�� ���� �����̵��� ������
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

					// ���������� pMatW�� tmpOffset�� �����Ѵ�.
					// �׷��� �׺�Ž����� ���ٸ�? ������������
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
					// ���� ��������ġ�� �����Ѵ�. 
					m_prevOffsetPos = offsetPos;

				}
			}

			// ���� �ִϸ��̼� ��ġ(m_TransformationMatrix)�� 0���� ��������.
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
	else // ��Ʈ
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
