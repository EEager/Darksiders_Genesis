#include "..\Public\VIBuffer_Trail.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Trail::NativeConstruct_Prototype(const _tchar* pShaderFilePath)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(Compile_Shader(ElementDesc, 2, pShaderFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Trail::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;


	// Init Stuffs
	m_maxvtxCnt = 1000;// 일단은 1000개로
	m_maxtriCnt = m_maxvtxCnt - 2;

	m_fDuration = 0.003f; // 몇초마다 트레일 정점을 넣을것인지. 0.01f가 부드러움.
	m_fAliveTime = 0.25f; // 넣은 트레일 정점이 몇초동안 살아남을 것인지.
	m_LerpCnt = 7.f; // 캣멀스플라인 보간에 사용될 값. 커질수록 트레일이 느리게 따라온다.

	// --------------------------------------------------------
	// Vertex Buffer Create.
	m_iStride = sizeof(VTXTEX); // 트레일은 위치와 UV 좌표만 갖는다.
	m_iNumVertices = m_maxvtxCnt;
	m_iNumVertexBuffers = 1;
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXTEX[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXTEX) * m_iNumVertices);
	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;
	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	// --------------------------------------------------------
	// Index Buffer Create
	m_iNumPrimitive = m_maxtriCnt; // Vertex개수에서 2를 뺀다.
	m_iIndicesSize = sizeof(FACEINDICES16);
	m_iNumIndicesPerFigure = 3;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;
	m_pPrimitiveIndices = new FACEINDICES16[m_iNumPrimitive];

	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Trail::Render(_uint iPassIndex)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* 장치에 동시에 여러개의 정점버퍼를 셋팅할 수 있기 때문. */
	ID3D11Buffer* pBuffers[] = {
		m_pVB
	};

	_uint		iStrides[] = {
		m_iStride
	};

	_uint		iOffset[] = {
		0
	};

	m_pDeviceContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pBuffers, iStrides, iOffset);

	m_pDeviceContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pDeviceContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	/* 정점셰이더에 입력되는 정점의 구성정보. */
	m_pDeviceContext->IASetInputLayout(m_PassesDesc[iPassIndex]->pInputlayout.Get());

	if (FAILED(m_PassesDesc[iPassIndex]->pPass->Apply(0, m_pDeviceContext)))
		return E_FAIL;

	m_pDeviceContext->DrawIndexed(m_iNumIndicesPerFigure * m_curTriCnt, 0, 0);

	return S_OK;
}

// upposition : 검 위부분 / downposition : 검 아래 부분
void CVIBuffer_Trail::AddNewTrail(const _float3& upposition, const _float3& downposition, _float fTimeDelta)
{
	m_fTimerAcc += fTimeDelta;
	if (m_fDuration < (double)m_fTimerAcc)
	{
		TrailData data(upposition, downposition);
		trailDatas.emplace_back(data);
		m_fTimerAcc = 0;
	}
}

void CVIBuffer_Trail::Update(_float fTimeDelta, _matrix* pWorldMat)
{
	if (trailDatas.empty())
		return;
	
	// #1. 지속시간 초과한 트레일 선들 지워주자.
	while (!trailDatas.empty())
	{
		auto frontData = trailDatas.front(); // 가장 앞에 있는 아이는 지속시간이 가장 큰 아이 이다. 
		if (frontData.timecount + fTimeDelta >= m_fAliveTime)
			trailDatas.pop_front(); 
		else
			break;
	}

	// #2. 지워봤는데 사이즈가 1개 이하면 나가자.
	if (trailDatas.size() <= 1)
		return;
	
	// #3. #1에서 살아남은 트레일들의 지속시간을 올려주자.
	for (auto iter = trailDatas.begin(); iter != trailDatas.end(); ++iter)
	{
		iter->timecount += fTimeDelta;
	}

	D3D11_MAPPED_SUBRESOURCE	SubResourceDataVB;
	D3D11_MAPPED_SUBRESOURCE	SubResourceDataIB;

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataVB);
	m_pDeviceContext->Map(m_pIB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResourceDataIB);

	std::size_t dataCnt = trailDatas.size();
	unsigned long index = 0;

	for (std::size_t i = 0; i < dataCnt; ++i)
	{
		SplineTrailPosition((VTXTEX*)SubResourceDataVB.pData, i, index, pWorldMat);

		if (m_maxvtxCnt <= index)
			break;
	}

	float uvRate = 1.f / float(index - 2);
	for (UINT i = 0; i < index; i += 2)
	{
		((VTXTEX*)SubResourceDataVB.pData)[i].vTexUV  = _float2((uvRate * i), 0.f);
		((VTXTEX*)SubResourceDataVB.pData)[i + 1].vTexUV = _float2((uvRate * i), 1.f);
	}

	m_curVtxCnt = index;
	m_curTriCnt = m_curVtxCnt - 2;
	for (unsigned long i = 0; i < m_curTriCnt; i += 2)
	{
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._0 = i;
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._1 = i + 3;
		((FACEINDICES16*)SubResourceDataIB.pData)[i]._2 = i + 1;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._0 = i;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._1 = i + 2;
		((FACEINDICES16*)SubResourceDataIB.pData)[i + 1]._2 = i + 3;
	}

	m_pDeviceContext->Unmap(m_pVB, 0);
	m_pDeviceContext->Unmap(m_pIB, 0);
}

// @ index : m_pVB의 index
// @ dataindex : trailDatas의 index
void CVIBuffer_Trail::SplineTrailPosition(OUT VTXTEX* vtx, const size_t& dataindex, unsigned long& index, _matrix *pWorldMat)
{
	_matrix im = XMMatrixInverse(nullptr, *pWorldMat);

	if (m_maxvtxCnt <= index)
		return;

	size_t iCurIndex = index; // m_pVB

	// trailDatas를 로컬로 변환하자.
	_vector coordTemp = XMVector3TransformCoord(XMLoadFloat3(&trailDatas[dataindex].position[0]), im);
	XMStoreFloat3(&vtx[index].vPosition, coordTemp);
	++index;

	if (m_maxvtxCnt <= index)
		return;

	coordTemp = XMVector3TransformCoord(XMLoadFloat3(&trailDatas[dataindex].position[1]), im);
	XMStoreFloat3(&vtx[index].vPosition, coordTemp);
	++index;

	if (m_maxvtxCnt <= index)
		return;

	_vector vLerpPos[2];

	size_t iSize = trailDatas.size();
	for (unsigned long j = 1; j < m_LerpCnt; ++j)
	{
		size_t iEditIndexV0 = (dataindex < 1 ? 0 : dataindex - 1);
		size_t iEditIndexV2 = (dataindex + 1 >= iSize ? dataindex : dataindex + 1);
		size_t iEditIndexV3 = (dataindex + 2 >= iSize ? iEditIndexV2 : dataindex + 2);

		vLerpPos[0] = XMVectorCatmullRom(XMLoadFloat3(&trailDatas[iEditIndexV0].position[0]), // Start 이전 정보
			XMLoadFloat3(&trailDatas[dataindex].position[0]),// Start
			XMLoadFloat3(&trailDatas[iEditIndexV2].position[0]), // End
			XMLoadFloat3(&trailDatas[iEditIndexV3].position[0]), // End 이후 정보
			j / float(m_LerpCnt));

		vLerpPos[1] = XMVectorCatmullRom(XMLoadFloat3(&trailDatas[iEditIndexV0].position[1]),
			XMLoadFloat3(&trailDatas[dataindex].position[1]),
			XMLoadFloat3(&trailDatas[iEditIndexV2].position[1]),
			XMLoadFloat3(&trailDatas[iEditIndexV3].position[1]),
			j / float(m_LerpCnt));

		_vector coordTemp = XMVector3TransformCoord(vLerpPos[0], im);
		XMStoreFloat3(&vtx[index].vPosition, coordTemp);
		++index;
		if (m_maxvtxCnt <= index)
			return;
		coordTemp = XMVector3TransformCoord(vLerpPos[1], im);
		XMStoreFloat3(&vtx[index].vPosition, coordTemp);
		++index;
		if (m_maxvtxCnt <= index)
			return;
	}
}



CVIBuffer_Trail * CVIBuffer_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_Trail::Clone(void * pArg)
{
	CVIBuffer_Trail*	pInstance = new CVIBuffer_Trail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pVertices);
	Safe_Delete_Array(m_pPrimitiveIndices);
}
