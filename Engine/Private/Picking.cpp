#include "..\public\Picking.h"
#include "PipeLine.h"
#include "Transform.h"
#include "VIBuffer.h"

IMPLEMENT_SINGLETON(CPicking)

CPicking::CPicking()
{
}

HRESULT CPicking::Transform_ToWorldSpace()
{
	POINT	ptMouse;
	GetCursorPos(&ptMouse);

	/* 뷰포트 스페이스*/
	ScreenToClient(m_hWnd, &ptMouse);

	RECT	rcClient;
	GetClientRect(m_hWnd, &rcClient);

	/* 투영 스페이스 */
	_float3		vMousePos;
	vMousePos.x = ptMouse.x / (rcClient.right*0.5f) - 1.f;
	vMousePos.y = ptMouse.y / (rcClient.bottom*-0.5f) + 1.f;
	vMousePos.z = 0.f;

	/* 뷰 스페이스로 */
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);
	_matrix		ProjMatrix = pPipeLine->Get_Transform(CPipeLine::TS_PROJ);

	// 프로젝션의 역행렬을 구한다
	XMMatrixInverse(nullptr, ProjMatrix); 
	// 뷰포트 -> 투영 스페이스
	XMStoreFloat3(&vMousePos, XMVector3TransformCoord(XMLoadFloat3(&vMousePos), ProjMatrix));

	m_vMouseRayPos = _float3(0.0f, 0.f, 0.f);
	XMStoreFloat3(&m_vMouseRay, XMLoadFloat3(&vMousePos) - XMLoadFloat3(&m_vMouseRayPos));

	/* 월드스페이스로 */
	_matrix		ViewMatrix = pPipeLine->Get_Transform(CPipeLine::TS_VIEW);
	XMMatrixInverse(nullptr, ViewMatrix);

	XMStoreFloat3(&m_vMouseRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vMouseRayPos), ViewMatrix));
	XMStoreFloat3(&m_vMouseRay, XMVector2Normalize(XMVector3TransformNormal(XMLoadFloat3(&m_vMouseRay), ViewMatrix)));

	RELEASE_INSTANCE(CPipeLine);

	return S_OK;
}

HRESULT CPicking::Transform_WorldSpaceToLocalSpace(CTransform * pTransform)
{
	_matrix			WorldMatrixInv = pTransform->Get_WorldMatrixInverse();

	XMStoreFloat3(&m_vLocalMouseRayPos, XMVector3TransformCoord(XMLoadFloat3(&m_vMouseRayPos), WorldMatrixInv));
	XMStoreFloat3(&m_vLocalMouseRay, XMVector2Normalize(XMVector3TransformNormal(XMLoadFloat3(&m_vMouseRay), WorldMatrixInv)));

	return S_OK;
}

_bool CPicking::Picking(CVIBuffer * pBuffer, _float3 * pPickPos)
{
	_uint		iNumFace = pBuffer->Get_NumPrimitive();

	//_float3*	pVerticesPos = pBuffer->Get_VerticesPos();

	//for (_uint i = 0; i < iNumFace; ++i)
	//{
	//	_uint	_1, _2, _3;
	//	pBuffer->Get_Indices(i, &_1, &_2, &_3);

	//	_float fU, fV, fDist;
	//	if (D3DXIntersectTri(&pVerticesPos[_1], &pVerticesPos[_2], &pVerticesPos[_3], &m_vLocalMouseRayPos, &m_vLocalMouseRay, &fU, &fV, &fDist))
	//	{
	//		*pPickPos = m_vLocalMouseRayPos + *D3DXVec3Normalize(&m_vLocalMouseRay, &m_vLocalMouseRay)*fDist;
	//		
	//		return true;
	//	}
	//}

	return false;
}

void CPicking::Free()
{
}
