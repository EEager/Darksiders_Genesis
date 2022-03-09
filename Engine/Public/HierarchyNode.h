#pragma once

#include "Channel.h"

BEGIN(Engine)

class CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	_uint Get_Depth() const  {
		return m_iDepth;
	}

	const char* Get_Name() const {
		return m_szName;
	}

	_matrix Get_OffsetMatix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}

	_matrix Get_TransformationMatrix() {
		return XMLoadFloat4x4(&m_TransformationMatrix);
	}
	_matrix Get_CombinedMatix() {
		return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
	}

	void Set_OffsetMatrix(_fmatrix OffsetMatrix) {
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}

	void Add_Channel(_uint iAnimIndex, class CChannel* pChannel) {
		m_Channels[iAnimIndex] = pChannel;
		Safe_AddRef(pChannel);
	}


public:
	HRESULT NativeConstruct(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	HRESULT Reserve_Channels(_uint iNumAnimation);
	void Update_CombinedTransformationMatrix(); // �����°��
	void Update_CombinedTransformationMatrix(IN _uint iCurrentAnimIndex, OUT _float4x4* pMatW = nullptr, const char* pRootNodeName = nullptr); // �Ϲ����ΰ��


private:
	char					m_szName[MAX_PATH] = ""; // aiNode's Name
	_float4x4				m_OffsetMatrix; // �� -> ���� 
	_float4x4				m_TransformationMatrix; // ä�ο��� ������ ���� �ִϸ��̼��� �� ����ε���?
	/* m_CombinedTransformationMatrix =
	TransformationMatrix(�������) * pParent->CombinedTransformationMatrix(�θ����) */
	_float4x4				m_CombinedTransformationMatrix; // �� �̰� ���� �򰥸��� �ù߰�
	CHierarchyNode*			m_pParent = nullptr;
	_uint					m_iDepth = 0;

private:
	// �ִϸ��̼� �������� ������Ŀ� �����ϱ� ���� �����ִϸ��̼� ��ġ��.
	_float4 m_prevOffsetPos = _float4(0.f, 0.f, 0.f, 1.f); // init zero

private:
	vector<class CChannel*>			m_Channels;
	typedef vector<class CChannel*>	CHANNELS;

public:
	static CHierarchyNode* Create(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	virtual void Free() override;
};

END