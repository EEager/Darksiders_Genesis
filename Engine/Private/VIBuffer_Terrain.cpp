#include "..\Public\VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
	, m_iNumVerticesX(rhs.m_iNumVerticesX)
	, m_iNumVerticesZ(rhs.m_iNumVerticesZ)
{
}

HRESULT CVIBuffer_Terrain::NativeConstruct_Prototype(const _tchar* pShaderFilePath, const _tchar* pHeightMapFilePath)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	_ulong		dwByte = 0;
	HANDLE		hFile = CreateFile(pHeightMapFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	BITMAPFILEHEADER		fh;
	BITMAPINFOHEADER		ih;

	ReadFile(hFile, &fh, sizeof(fh), &dwByte, nullptr);
	ReadFile(hFile, &ih, sizeof(ih), &dwByte, nullptr);

	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;

	_ulong*		pPixel = new _ulong[m_iNumVerticesX * m_iNumVerticesZ];	

	ReadFile(hFile, pPixel, sizeof(_ulong) * m_iNumVerticesX * m_iNumVerticesZ, &dwByte, nullptr);

	m_iStride = sizeof(VTXNORTEX);
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			((VTXNORTEX*)m_pVertices)[iIndex].vPosition = _float3((float)j, (pPixel[iIndex] & 0x000000ff) / 10.f, (float)i);
			((VTXNORTEX*)m_pVertices)[iIndex].vNormal = _float3(0.0f, 0.0f, 0.f);
			((VTXNORTEX*)m_pVertices)[iIndex].vTexUV = _float2((float)j / _float(m_iNumVerticesX - 1), i / _float(m_iNumVerticesZ - 1));
		}
	}

	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;	


	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iIndicesSize = sizeof(FACEINDICES32);
	m_iNumIndicesPerFigure = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pPrimitiveIndices = new FACEINDICES32[m_iNumPrimitive];

	VTXNORTEX*		pVertices = (VTXNORTEX*)m_pVertices;

	_uint		iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = {
				iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1, 
				iIndex				
			};

			((FACEINDICES32*)m_pPrimitiveIndices)[iNumPrimitive]._0 = iIndices[0];
			((FACEINDICES32*)m_pPrimitiveIndices)[iNumPrimitive]._1 = iIndices[1];
			((FACEINDICES32*)m_pPrimitiveIndices)[iNumPrimitive]._2 = iIndices[2];

			_vector		vSour, vDest, vNormal;

			vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal, 
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[1]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));
			++iNumPrimitive;

			((FACEINDICES32*)m_pPrimitiveIndices)[iNumPrimitive]._0 = iIndices[0];
			((FACEINDICES32*)m_pPrimitiveIndices)[iNumPrimitive]._1 = iIndices[2];
			((FACEINDICES32*)m_pPrimitiveIndices)[iNumPrimitive]._2 = iIndices[3];

			vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
			vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
			vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

			XMStoreFloat3(&pVertices[iIndices[0]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[2]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal));
			XMStoreFloat3(&pVertices[iIndices[3]].vNormal,
				XMVector3Normalize(XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal));

			++iNumPrimitive;
		}
	}


	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;


	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};	

	if (FAILED(Compile_Shader(ElementDesc, 3, pShaderFilePath)))
		return E_FAIL;

	CloseHandle(hFile);
	Safe_Delete_Array(pPixel);
	

	return S_OK;
}

HRESULT CVIBuffer_Terrain::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

_float CVIBuffer_Terrain::Compute_Height(_fvector vPosition)
{
	_uint		iIndex = (_uint)XMVectorGetZ(vPosition) * m_iNumVerticesX + (_uint)XMVectorGetX(vPosition);

	_uint		iIndices[] = {
		iIndex + m_iNumVerticesX,
		iIndex + m_iNumVerticesX + 1,
		iIndex + 1,
		iIndex
	};

	VTXNORTEX* pVertices = (VTXNORTEX*)m_pVertices;

	_float fWidth = XMVectorGetX(vPosition) - pVertices[iIndices[0]].vPosition.x;
	_float fDepth = pVertices[iIndices[0]].vPosition.z - XMVectorGetZ(vPosition);

	_vector		vPlane;

	if (fWidth > fDepth)
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&pVertices[iIndices[0]].vPosition),
			XMLoadFloat3(&pVertices[iIndices[1]].vPosition),
			XMLoadFloat3(&pVertices[iIndices[2]].vPosition));
	}

	else
	{
		vPlane = XMPlaneFromPoints(XMLoadFloat3(&pVertices[iIndices[0]].vPosition),
			XMLoadFloat3(&pVertices[iIndices[2]].vPosition),
			XMLoadFloat3(&pVertices[iIndices[3]].vPosition));
	}

	/*ax + by + cz + d = 0;
	y = (-ax - cz - d) / b;*/

	return (XMVectorGetX(vPlane) * -1 * XMVectorGetX(vPosition) // -ax
		- XMVectorGetZ(vPlane) * XMVectorGetZ(vPosition)
		- XMVectorGetW(vPlane)) / XMVectorGetY(vPlane);

}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath, const _tchar* pHeighMapFilePath)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath, pHeighMapFilePath)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Terrain");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();


}
