#pragma once

#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum LINE { LINE_AB, LINE_BC, LINE_CA, LINE_END };
public:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CCell() = default;

public:
	_vector Get_Point(POINT ePoint) {
		return XMLoadFloat3(&m_vPoints[ePoint]);
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_Neighbors[eLine] = pNeighbor;
	}

public:
	HRESULT NativeConstruct(_float3* pPoints, _uint iindex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	_uint					m_iIndex = 0;
	_float3					m_vPoints[POINT_END];
	CCell*					m_Neighbors[LINE_END];
public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float3* pPoints, _uint iindex);
	virtual void Free() override;
};

END