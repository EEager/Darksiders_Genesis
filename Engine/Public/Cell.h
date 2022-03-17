#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
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

	_uint Get_Index() const {
		return m_iIndex;
	}

	void Set_Neighbor(LINE eLine, CCell* pNeighbor) {
		m_Neighbors[eLine] = pNeighbor;
	}

	void Set_PickingPoint(POINT pt)
	{
		m_ePickingPoint = pt;
	}

	void Set_Constant_Shphere(_fmatrix fmatrix, _float4 vColor);

public:
	HRESULT NativeConstruct(_float3* pPoints, _uint iindex);
	_bool Compare_Points(_fvector vSourPoint, _fvector vDestPoint);
	_bool isIn(_vector vPoint, _float4x4* pWorldMatrix, CCell** ppNeighbor, OUT _vector* vDstPnt = nullptr);

#ifdef _DEBUG
public:
	HRESULT Render(_float4x4* pWorldMatrix, _uint iCurrentIndex);

#endif // _DEBUG

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

private:
	_uint					m_iIndex = 0;
	_float3					m_vPoints[POINT_END];
	_float3					m_vLine[LINE_END];
	CCell*					m_Neighbors[LINE_END];

private:
	enum POINT	m_ePickingPoint = POINT_END;

#ifdef _DEBUG
private:
	class CVIBuffer_Line* m_pVIBuffer = nullptr;
	class CVIBuffer_Sphere* m_pVIBufferSphere = nullptr;
#endif // _DEBUG


#ifdef _DEBUG
private:
	HRESULT Ready_DebugBuffer();
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float3* pPoints, _uint iindex);
	virtual void Free() override;
};

END