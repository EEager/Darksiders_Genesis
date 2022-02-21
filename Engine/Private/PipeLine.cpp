#include "..\public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(TRANSFORMTYPE eTransformType, _fmatrix TransformMatrix)
{
	if (eTransformType == TS_VIEW)
	{
		XMStoreFloat4x4(&m_ViewMatrix, TransformMatrix);
		
	}
	else
	{
		XMStoreFloat4x4(&m_ProjMatrix, TransformMatrix);
		
	}
}

void CPipeLine::Tick()
{
	XMStoreFloat4x4(&m_ViewMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_ViewMatrix)));

	memcpy(&m_vCamPosition, &m_ViewMatrixInverse.m[3][0], sizeof(_float4));
	memcpy(&m_vCamLook, &m_ViewMatrixInverse.m[2][0], sizeof(_float4));
}

void CPipeLine::Free()
{
}


