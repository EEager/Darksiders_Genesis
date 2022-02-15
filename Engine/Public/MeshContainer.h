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
	virtual HRESULT NativeConstruct_Prototype(_bool isAnimMesh, aiMesh* pMesh);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render(_uint iPassIndex);

public:
	HRESULT Create_VertexIndexBuffer();


	

private:
	/* ������ ������ ä�쳮. */
	HRESULT SetUp_VerticesDesc(aiMesh* pMesh, _bool isAnim);
	HRESULT SetUp_IndicesDesc(aiMesh* pMesh);	

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _bool isAnimMesh, aiMesh* pMesh);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END