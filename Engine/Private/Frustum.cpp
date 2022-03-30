#include "..\Public\Frustum.h"
#include "PipeLine.h"

IMPLEMENT_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}

HRESULT CFrustum::NativeConstruct()
{
	m_vPosition[0] = _float3(-1.f, 1.f, 0.f); // 왼위 뒤
	m_vPosition[1] = _float3(1.f, 1.f, 0.f); // 오위 뒤
	m_vPosition[2] = _float3(1.f, -1.f, 0.f); // 오아 뒤
	m_vPosition[3] = _float3(-1.f, -1.f, 0.f); // 왼아 뒤

	m_vPosition[4] = _float3(-1.f, 1.f, 1.f); // .. 앞
	m_vPosition[5] = _float3(1.f, 1.f, 1.f);
	m_vPosition[6] = _float3(1.f, -1.f, 1.f);
	m_vPosition[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	CPipeLine*	pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix		ViewMatrix = pPipeLine->Get_Transform(CPipeLine::TS_VIEW);
	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix); // ViewMatrix 역행렬

	_matrix		ProjMatrix = pPipeLine->Get_Transform(CPipeLine::TS_PROJ);
	ProjMatrix = XMMatrixInverse(nullptr, ProjMatrix); // ProjMatrix 역행렬

	for (_uint i = 0; i < 8; ++i)
	{
		// 로컬 월드 뷰스페이스 투영스페이스
		XMStoreFloat3(&m_vWorldPos[i], XMVector3TransformCoord(XMLoadFloat3(&m_vPosition[i]), ProjMatrix)); // 투영스페이스 => 뷰스페이스
		XMStoreFloat3(&m_vWorldPos[i], XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPos[i]), ViewMatrix)); // 뷰스페이스 => 월드스페이스
	}

	// 6개의 평면방정식을 구한다. 
	XMStoreFloat4(&m_WorldPlane[0], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[1]), 
		XMLoadFloat3(&m_vWorldPos[5]), 
		XMLoadFloat3(&m_vWorldPos[6])));

	XMStoreFloat4(&m_WorldPlane[1], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[4]),
		XMLoadFloat3(&m_vWorldPos[0]),
		XMLoadFloat3(&m_vWorldPos[3])));

	XMStoreFloat4(&m_WorldPlane[2], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[4]),
		XMLoadFloat3(&m_vWorldPos[5]),
		XMLoadFloat3(&m_vWorldPos[1])));

	XMStoreFloat4(&m_WorldPlane[3], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[3]),
		XMLoadFloat3(&m_vWorldPos[2]),
		XMLoadFloat3(&m_vWorldPos[6])));

	XMStoreFloat4(&m_WorldPlane[4], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[5]),
		XMLoadFloat3(&m_vWorldPos[4]),
		XMLoadFloat3(&m_vWorldPos[7])));

	XMStoreFloat4(&m_WorldPlane[5], XMPlaneFromPoints(
		XMLoadFloat3(&m_vWorldPos[0]),
		XMLoadFloat3(&m_vWorldPos[1]),
		XMLoadFloat3(&m_vWorldPos[2])));

	RELEASE_INSTANCE(CPipeLine);
}

_bool CFrustum::isIn_WorldSpace(_fvector vWorldPos, _float fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{		
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_WorldPlane[i]), vWorldPos)))
			return false;		
	}

	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vLocalPos, _float fRadius)
{
	for (_uint i = 0; i < 6; ++i)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_LocalPlane[i]), vLocalPos)))
			return false;
	}

	return true;
}

void CFrustum::Transform_LocalSpace(_fmatrix WorldMatrixInv)
{
	_vector		vLocalPos[8];

	for (_uint i = 0; i < 8; ++i)	
		vLocalPos[i] = XMVector3TransformCoord(XMLoadFloat3(&m_vWorldPos[i]), WorldMatrixInv);

	XMStoreFloat4(&m_LocalPlane[0], XMPlaneFromPoints(
		vLocalPos[1],
		vLocalPos[5],
		vLocalPos[6]));

	XMStoreFloat4(&m_LocalPlane[1], XMPlaneFromPoints(
		vLocalPos[4],
		vLocalPos[0],
		vLocalPos[3]));

	XMStoreFloat4(&m_LocalPlane[2], XMPlaneFromPoints(
		vLocalPos[4],
		vLocalPos[5],
		vLocalPos[1]));

	XMStoreFloat4(&m_LocalPlane[3], XMPlaneFromPoints(
		vLocalPos[3],
		vLocalPos[2],
		vLocalPos[6]));

	XMStoreFloat4(&m_LocalPlane[4], XMPlaneFromPoints(
		vLocalPos[5],
		vLocalPos[4],
		vLocalPos[7]));

	XMStoreFloat4(&m_LocalPlane[5], XMPlaneFromPoints(
		vLocalPos[0],
		vLocalPos[1],
		vLocalPos[2]));

}





void CFrustum::Free()
{

}
