#include "..\Public\Navigation.h"
#include "Cell.h"

_float4x4* CNavigation::m_pWorldMatrixPtr = nullptr;

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
{
	m_pWorldMatrixPtr = new _float4x4;
	XMStoreFloat4x4(m_pWorldMatrixPtr, XMMatrixIdentity());
}

CNavigation::CNavigation(const CNavigation& rhs)
	: CComponent(rhs)
	, m_Cells(rhs.m_Cells)
	, m_iCurrentIndex(rhs.m_iCurrentIndex)
{
	for (auto& pCell : m_Cells)
		Safe_AddRef(pCell);
}

HRESULT CNavigation::NativeConstruct_Prototype(const _tchar * pNavigationDataFile)
{
	Load_Cells();

	//_ulong		dwByte = 0;

	//HANDLE		hFile = CreateFile(pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	//if (0 == hFile)
	//	return E_FAIL;

	//_float3		vPoints[CCell::POINT_END];	

	//while (true)
	//{
	//	ZeroMemory(vPoints, sizeof(_float3) * CCell::POINT_END);

	//	ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);

	//	if (0 == dwByte)
	//		break;

	//	CCell*			pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, (_uint)m_Cells.size());
	//	if (nullptr == pCell)
	//		return E_FAIL;

	//	m_Cells.push_back(pCell);

	//}

	//CloseHandle(hFile);

	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNavigation::NativeConstruct(void* pArg)
{
	if (nullptr != pArg)
	{
		Safe_Delete(m_pWorldMatrixPtr); // 다른 이가  m_pWorldMatrixPtr를 설정해주려고 한다면 기존것은 매너적으로 지워주자
		m_pWorldMatrixPtr = (_float4x4*)pArg;
	}

	return S_OK;
}

void CNavigation::SetUp_CurrentIdx(_fvector vPos)
{
	// Cells 돌면서~ isIn 인지 검사하면되는거 아녀? 
	CCell* pNeighbor = nullptr;

	for (int i = 0;i < m_Cells.size();i++)
	{
		if (m_Cells[i]->isIn(vPos, m_pWorldMatrixPtr, &pNeighbor))
		{
			m_iCurrentIndex = i;
			return;
		}
	}

	return;
}

// 네비매쉬 따라 움직일수 있는지 없는지 확인하는 함수
int CNavigation::isMove(_vector vPosition, OUT _vector* vDstPnt)
{
	CCell* pNeighbor = nullptr;
	int	loopCnt = 0;
	int MAX_LOOP_CNT = (int)m_Cells.size();

	// isIn에서 변경된 vPosition값을 반환받기위해서 포인터를 넘겨주자
	if (false == m_Cells[m_iCurrentIndex]->isIn(vPosition, m_pWorldMatrixPtr, &pNeighbor, vDstPnt))
	{
		if (nullptr == pNeighbor)
		{
			// 모서리 이동하려면 여기서 
			// return 1; 모서리 처리는 나중에...
			return 0;
		}
		else
		{
			while (loopCnt++<MAX_LOOP_CNT) // vPosition 위치에 Cell이 있는지 계속 체크하기 위함. 
			{
				// isIn에서 vPosition 변경할 필요가 없기때문에 값을 넘겨주자
				_bool ret = pNeighbor->isIn(vPosition, m_pWorldMatrixPtr, &pNeighbor);
				if (ret == true)
				{
					m_iCurrentIndex = pNeighbor->Get_Index();
					return 1;
				}
				else
				{
					if (pNeighbor == nullptr) // 이웃셀쪽에 위치해 있는데 이웃 셀이없다. return false다 
					{
						return 0;
					}
				}
			}
		}
	}

	return 1;
}


// 현재 피킹된 점 범위 내에 Cell내의 점이 있다면 내가 정점을 return 아니면 인자값 그대로 리턴
_vector CNavigation::Get_Nearest_Point(_float3 vPickingPt)
{
	_vector retVec = XMLoadFloat3(&vPickingPt);
	for (auto& pCell : m_Cells)
	{
		pCell->Set_PickingPoint((CCell::POINT::POINT_END));

		for (int i = 0; i < 3; i++)
		{
			if (XMVectorGetX(XMVector3Length(pCell->Get_Point((CCell::POINT)i) - XMLoadFloat3(&vPickingPt))) < 0.5f)
			{
				pCell->Set_PickingPoint((CCell::POINT)i);
				retVec = pCell->Get_Point((CCell::POINT)i);
			}
		}
	}

	return retVec;
}


#ifdef _DEBUG
HRESULT CNavigation::Render()
{
	for (auto& pCell : m_Cells)
	{
		pCell->Render(m_pWorldMatrixPtr, m_iCurrentIndex);
	}
	return S_OK;
}
#endif // _DEBUG


HRESULT CNavigation::SetUp_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))			
				pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
				pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
				pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);		
		}
	}

	return S_OK;
}

_float CNavigation::Compute_Height(_fvector vPos)
{
	if (m_Cells[m_iCurrentIndex] == nullptr)
		return 77.f;

	_vector		vPlane;
	vPlane = XMPlaneFromPoints(m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT::POINT_A),
		m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT::POINT_B),
		m_Cells[m_iCurrentIndex]->Get_Point(CCell::POINT::POINT_C));

	return (XMVectorGetX(vPlane) * -1 * XMVectorGetX(vPos) // -ax
		- XMVectorGetZ(vPlane) * XMVectorGetZ(vPos)
		- XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);
}

HRESULT CNavigation::Save_Cells()
{
	_ulong		dwByte = 0;

	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/NavigationData.txt"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	for (auto pCell : m_Cells)
	{
		_float3		vPoint[3];
		ZeroMemory(vPoint, sizeof(_float3) * 3);
		XMStoreFloat3(&vPoint[0], pCell->Get_Point(CCell::POINT::POINT_A));
		XMStoreFloat3(&vPoint[1], pCell->Get_Point(CCell::POINT::POINT_B));
		XMStoreFloat3(&vPoint[2], pCell->Get_Point(CCell::POINT::POINT_C));
		WriteFile(hFile, vPoint, sizeof(_float3) * 3, &dwByte, nullptr);
	}

	CloseHandle(hFile);

	return S_OK;
}

HRESULT CNavigation::Load_Cells()
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/NavigationData.txt"), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_float3		vPoints[CCell::POINT_END];	

	while (true)
	{
		ZeroMemory(vPoints, sizeof(_float3) * CCell::POINT_END);
		ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr);
		if (0 == dwByte)
			break;
		CCell*			pCell = CCell::Create(m_pDevice, m_pDeviceContext, vPoints, (_uint)m_Cells.size());
		if (nullptr == pCell)
			return E_FAIL;
		m_Cells.push_back(pCell);
	}
	CloseHandle(hFile);
	if (FAILED(SetUp_Neighbor()))
		return E_FAIL;

	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pNavigationDataFile)
{
	CNavigation*	pInstance = new CNavigation(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pNavigationDataFile)))
	{
		MSG_BOX("Failed To Creating CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CNavigation::Clone(void * pArg)
{
	CNavigation*	pInstance = new CNavigation(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	Safe_Delete(m_pWorldMatrixPtr);
}
