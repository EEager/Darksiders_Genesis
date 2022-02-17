#pragma once

/* �� = �ټ��� �޽������̳ʷ� �����ȴ�. */
#include "VIBuffer.h"

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	explicit CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT NativeConstruct_Prototype(_bool isAnimMesh, aiMesh* pMesh, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render();

public:
	HRESULT Create_VertexIndexBuffer();

private:
	/* ���� �޽������̳ʰ� � ���׸��� �ε����� ������ */
	_uint	m_iMaterialIndex = 0;


	

private:
	/* ������ ������ ä�쳮. */
	HRESULT SetUp_VerticesDesc(aiMesh* pMesh, _bool isAnim, _fmatrix PivotMatrix);
	HRESULT SetUp_IndicesDesc(aiMesh* pMesh);	
	HRESULT SetUp_SkinnedDesc(aiMesh* pMesh);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _bool isAnimMesh, aiMesh* pMesh, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END