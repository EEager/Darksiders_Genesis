#include "..\Public\Cell.h"
#include "VIBuffer_Line.h"
#include "PipeLine.h"
#include "VIBuffer_Sphere.h"
#include "Renderer.h"

CCell::CCell(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
{
Safe_AddRef(m_pDevice);
Safe_AddRef(m_pDeviceContext);
}

void CCell::Set_Constant_Shphere(_fmatrix fmatrix, _float4 vColor)
{
	m_pVIBufferSphere->Set_RawValue("g_WorldMatrix", (_float4x4*)&fmatrix, sizeof(_float4x4));
	m_pVIBufferSphere->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(CPipeLine::GetInstance()->Get_Transform(CPipeLine::TS_VIEW)), sizeof(_float4x4));
	m_pVIBufferSphere->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(CPipeLine::GetInstance()->Get_Transform(CPipeLine::TS_PROJ)), sizeof(_float4x4));
	m_pVIBufferSphere->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
}

HRESULT CCell::NativeConstruct(_float3* pPoints, _uint iIndex)
{
	m_iIndex = iIndex;

	ZeroMemory(m_Neighbors, sizeof(CCell*) * LINE_END);

	memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

	XMStoreFloat3(&m_vLine[LINE_AB], XMLoadFloat3(&m_vPoints[POINT_B]) - XMLoadFloat3(&m_vPoints[POINT_A]));
	XMStoreFloat3(&m_vLine[LINE_BC], XMLoadFloat3(&m_vPoints[POINT_C]) - XMLoadFloat3(&m_vPoints[POINT_B]));
	XMStoreFloat3(&m_vLine[LINE_CA], XMLoadFloat3(&m_vPoints[POINT_A]) - XMLoadFloat3(&m_vPoints[POINT_C]));


#ifdef _DEBUG
	if (FAILED(Ready_DebugBuffer()))
		return E_FAIL;
#endif // _DEBUG

	return S_OK;
}

_bool CCell::Compare_Points(_fvector vDestPoint1, _fvector vDestPoint2)
{

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint1))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint2))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint2))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint1))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint2))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint2))
			return true;
	}

	if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_C]), vDestPoint1))
	{
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_A]), vDestPoint2))
			return true;
		if (XMVector3Equal(XMLoadFloat3(&m_vPoints[POINT_B]), vDestPoint2))
			return true;
	}

	return false;
}

// 모서리 이동 없음
_bool CCell::isIn(_vector vPoint, _float4x4* pWorldMatrix, CCell** ppNeighbor, OUT _vector* vDstPnt)
{
	// 3개의 변을 돌면서 vPoint가 내부점에 있는지 확인해야한다
	for (_uint i = 0; i < LINE_END; ++i)
	{
		_vector		vDirW = vPoint - XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[i]), XMLoadFloat4x4(pWorldMatrix));
		_vector		vNormalW = XMVectorSet(m_vLine[i].z * -1, m_vPoints[i].y, m_vLine[i].x, 0.f);

		vNormalW = XMVector3TransformNormal(vNormalW, XMLoadFloat4x4(pWorldMatrix));

		// vPoint이 cell 내부에 있는지 확인하자. Dot 연산이 0보다 작으면 90각 이상, 즉, 삼각형 밖에 있다.
		if (0 < XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirW), XMVector3Normalize(vNormalW))))
		{
			// 모서리 이동가능하도록 여기서 값을 반환해주자
			// 모서리 이동 위치 : AB의 노말라이즈 벡터 X (A노말벡터 dot vDirW)
			if (vDstPnt)
			{
				_vector lineDirNormal = XMVector3Normalize(XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]));
				*vDstPnt = XMLoadFloat3(&m_vPoints[i]) + lineDirNormal * XMVectorGetX(XMVector3Dot(vDirW, lineDirNormal));
			}

			// 하나라도 밖에 있다면 이웃셀을 검사하도록, ppNeighbor를 다음 이웃셀 포인터를 반환해주자
			*ppNeighbor = m_Neighbors[i];
			return false;
		}
	}

	// 3개의 변을 전부 검사한 후 점이 안에 있다면 해당 Ccell 안에 있는것이다.
	*ppNeighbor = this;
	return true;
}

#ifdef _DEBUG
HRESULT CCell::Render(_float4x4* pWorldMatrix, _uint iCurrentIndex)
{
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	_float4		vColor = _float4(1.f, 1.f, 1.f, 1.f);

	if (m_iIndex == iCurrentIndex)
		vColor = _float4(1.f, 0.f, 0.f, 1.f);

	CPipeLine* pPipeLine = GET_INSTANCE(CPipeLine);

	//m_pVIBuffer->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMLoadFloat4x4(pWorldMatrix)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_WorldMatrix", &XMMatrixTranspose(XMMatrixIdentity()), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ViewMatrix", &XMMatrixTranspose(pPipeLine->Get_Transform(CPipeLine::TS_VIEW)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_ProjMatrix", &XMMatrixTranspose(pPipeLine->Get_Transform(CPipeLine::TS_PROJ)), sizeof(_float4x4));
	m_pVIBuffer->Set_RawValue("g_vColor", &vColor, sizeof(_float4));
	m_pVIBuffer->Render(0);

	// 구매쉬 Render 
	for (int i = 0; i < (int)POINT::POINT_END; i++)
	{
		if (i == (int)m_ePickingPoint) // 현재 선택된 포인트이면 핑크색으로
			Set_Constant_Shphere(XMMatrixTranspose(XMMatrixTranslation(m_vPoints[i].x, m_vPoints[i].y, m_vPoints[i].z)), _float4(1.f, 0.f, 1.f, 1.f));
		else
			Set_Constant_Shphere(XMMatrixTranspose(XMMatrixTranslation(m_vPoints[i].x, m_vPoints[i].y, m_vPoints[i].z)), _float4(1.f, 1.f, 1.f, 1.f));
		
		m_pVIBufferSphere->Render(0);
	}


	RELEASE_INSTANCE(CPipeLine);

	return S_OK;
}
#endif // _DEBUG

#ifdef _DEBUG
HRESULT CCell::Ready_DebugBuffer()
{
	_float3		vPoints[] = {
		m_vPoints[POINT_A],
		m_vPoints[POINT_B],
		m_vPoints[POINT_C],
		m_vPoints[POINT_A]
	};

	m_pVIBuffer = CVIBuffer_Line::Create(m_pDevice, m_pDeviceContext, vPoints, 4);

	if (nullptr == m_pVIBuffer)
		return E_FAIL;


	// 구매쉬 등록
	m_pVIBufferSphere = CVIBuffer_Sphere::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Sphere.hlsl"), 0.1f);

	return S_OK;
}
#endif // _DEBUG

CCell * CCell::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, _float3* pPoints, _uint iIndex)
{
	CCell*	pInstance = new CCell(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pPoints, iIndex)))
	{
		MSG_BOX("Failed To Creating CCell");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCell::Free()
{
#ifdef _DEBUG
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pVIBufferSphere);
#endif // _DEBUG

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}
