#include "..\Public\HierarchyNode.h"



CHierarchyNode::CHierarchyNode()
{
}

HRESULT CHierarchyNode::NativeConstruct(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth)
{
	strcpy_s(m_szName, pName);
	XMStoreFloat4x4(&m_TransformationMatrix, TransformationMatrix);
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
	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix));
}

void CHierarchyNode::Update_CombinedTransformationMatrix(_uint iCurrentAnimIndex)
{
	if (iCurrentAnimIndex >= m_Channels.size())
		return;

	if (nullptr != m_Channels[iCurrentAnimIndex]) // iCurrentAnimIndex의 보간한 행렬을 가져온다.
		XMStoreFloat4x4(&m_TransformationMatrix, m_Channels[iCurrentAnimIndex]->Get_TransformationMatrix());

	if (nullptr != m_pParent)
		XMStoreFloat4x4(&m_CombinedTransformationMatrix,
			XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformationMatrix));
	else
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
