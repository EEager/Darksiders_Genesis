#pragma once

#include "Base.h"

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

	void Set_OffsetMatrix(_fmatrix OffsetMatrix) {
		XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
	}

public:
	HRESULT NativeConstruct(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);

private:
	char					m_szName[MAX_PATH] = ""; // aiNode's Name
	_float4x4				m_OffsetMatrix; // »À -> Á¤Á¡ 
	_float4x4				m_TransformationMatrix;
	CHierarchyNode*			m_pParent = nullptr;
	_uint					m_iDepth = 0;

public:
	static CHierarchyNode* Create(const char* pName, _fmatrix TransformationMatrix, CHierarchyNode* pParent, _uint iDepth);
	virtual void Free() override;
};

END