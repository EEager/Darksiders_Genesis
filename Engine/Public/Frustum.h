#pragma once

#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
	DECLARE_SINGLETON(CFrustum)
public:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT NativeConstruct();
	void Update();

	_bool isIn_WorldSpace(_fvector vWorldPos, _float fRadius = 0.f);
	_bool isIn_LocalSpace(_fvector vLocalPos, _float fRadius = 0.f);
	void Transform_LocalSpace(_fmatrix WorldMatrixInv);

private:
	_float3			m_vPosition[8];
	_float3			m_vWorldPos[8];

	_float4			m_WorldPlane[6];
	_float4			m_LocalPlane[6];


public:	
	virtual void Free() override;
};

END