#pragma once

#include "Base.h"

BEGIN(Engine)

class CPipeLine final : public CBase
{
	DECLARE_SINGLETON(CPipeLine)
public:
	enum TRANSFORMTYPE { TS_VIEW, TS_PROJ, TS_END };
private:
	CPipeLine();
	virtual ~CPipeLine() = default;
public:
	_matrix Get_Transform(TRANSFORMTYPE eTransformType) {
		return eTransformType == TS_VIEW ? XMLoadFloat4x4(&m_ViewMatrix) : XMLoadFloat4x4(&m_ProjMatrix);
	}

	_vector Get_CamPosition() {
		return XMLoadFloat4(&m_vCamPosition);
	}
	_vector Get_CamLook() {
		return XMLoadFloat4(&m_vCamLook);
	}


public:
	void Set_Transform(TRANSFORMTYPE eTransformType, _fmatrix TransformMatrix);

public:
	void Tick();


private:
	_float4x4					m_ViewMatrix;
	_float4x4					m_ViewMatrixInverse;

	_float4x4					m_ProjMatrix;

	_float4						m_vCamPosition;
	_float4						m_vCamLook;

public:
	virtual void Free() override;
};

END