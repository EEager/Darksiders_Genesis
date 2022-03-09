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
	void Update_CombinedTransformationMatrix(); // 뼈없는경우
	void Update_CombinedTransformationMatrix(IN _uint iCurrentAnimIndex, OUT _float4x4* pMatW = nullptr, const char* pRootNodeName = nullptr); // 일반적인경우


private:
	char					m_szName[MAX_PATH] = ""; // aiNode's Name
	_float4x4				m_OffsetMatrix; // 뼈 -> 정점 
	_float4x4				m_TransformationMatrix; // 채널에서 보간한 현재 애니메이션의 뼈 행렬인데요?
	/* m_CombinedTransformationMatrix =
	TransformationMatrix(보간행렬) * pParent->CombinedTransformationMatrix(부모행렬) */
	_float4x4				m_CombinedTransformationMatrix; // 아 이거 조금 헷갈리네 시발것
	CHierarchyNode*			m_pParent = nullptr;
	_uint					m_iDepth = 0;

private:
	// 애니메이션 오프셋을 월드행렬에 적용하기 위한 이전애니메이션 위치값.
	_float4 m_prevOffsetPos = _float4(0.f, 0.f, 0.f, 1.f); // init zero

private:
	vector<class CChannel*>			m_Channels;
	typedef vector<class CChannel*>	CHANNELS;

public:
	static CHierarchyNode* Create(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	virtual void Free() override;
};

END