#include "..\Public\VIBuffer_MonsterHp_PointGS.h"

CVIBuffer_MonsterHp_PointGS::CVIBuffer_MonsterHp_PointGS(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{

}

CVIBuffer_MonsterHp_PointGS::CVIBuffer_MonsterHp_PointGS(const CVIBuffer_MonsterHp_PointGS & rhs)
	: CVIBuffer(rhs)

{

}

HRESULT CVIBuffer_MonsterHp_PointGS::NativeConstruct_Prototype(const _tchar* pShaderFilePath)
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	m_iStride = sizeof(VTXPOINTHPBAR);
	m_iNumVertices = 1;
	m_iNumVertexBuffers = 1;
	
	// Desc
	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	m_pVertices = new VTXPOINTHPBAR[m_iNumVertices];
	ZeroMemory(m_pVertices, sizeof(VTXPOINTHPBAR) * m_iNumVertices);


	// 몬스터 머리 위에 올리자
	((VTXPOINTHPBAR*)m_pVertices)[0].vPosition = _float3(0.f, 0.f, 0.f);
	((VTXPOINTHPBAR*)m_pVertices)[0].fPSizeX = 2.f;
	((VTXPOINTHPBAR*)m_pVertices)[0].fPSizeY = .2f;

	((VTXPOINTHPBAR*)m_pVertices)[0].vRight = _float4(1.f, 0.f, 0.f, 0.f);
	((VTXPOINTHPBAR*)m_pVertices)[0].vUp = _float4(0.f, 1.f, 0.f, 0.f);
	((VTXPOINTHPBAR*)m_pVertices)[0].vLook = _float4(0.f, 0.f, 1.f, 0.f);

	ZeroMemory(&m_VBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubresourceData.pSysMem = m_pVertices;	

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_iNumPrimitive = 1;
	m_iIndicesSize = sizeof(_ushort);
	m_iNumIndicesPerFigure = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));

	m_IBDesc.ByteWidth = m_iIndicesSize * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	m_pPrimitiveIndices = new _ushort[m_iNumPrimitive];	

	for (_uint i = 0; i < 1; ++i) // 버텍스를 점으로 만들었으니 인덱스는 인스턴스 개수만큼 할당해주자
	{
		((_ushort*)m_pPrimitiveIndices)[0] = 0;		
	}

	ZeroMemory(&m_IBSubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubresourceData.pSysMem = m_pPrimitiveIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;


	// Shader_PointInstance.hlsl 시멘틱 
	D3D11_INPUT_ELEMENT_DESC		ElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "PSIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "PSIZE", 1, DXGI_FORMAT_R32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 52, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};	

	if (FAILED(Compile_Shader(ElementDesc, 6, pShaderFilePath)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_MonsterHp_PointGS::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_MonsterHp_PointGS * CVIBuffer_MonsterHp_PointGS::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar* pShaderFilePath)
{
	CVIBuffer_MonsterHp_PointGS*	pInstance = new CVIBuffer_MonsterHp_PointGS(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pShaderFilePath)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_MonsterHp_PointGS");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CVIBuffer_MonsterHp_PointGS::Clone(void * pArg)
{
	CVIBuffer_MonsterHp_PointGS*	pInstance = new CVIBuffer_MonsterHp_PointGS(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_MonsterHp_PointGS");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_MonsterHp_PointGS::Free()
{
	__super::Free();




}
