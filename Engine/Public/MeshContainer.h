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
	virtual HRESULT NativeConstruct_Prototype(class CModel* pModel, _bool isAnimMesh, aiMesh* pMesh, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render();

public:
	HRESULT Create_VertexIndexBuffer();

private:
	/* ���� �޽������̳ʰ� � ���׸��� �ε����� ������ */
	_uint			m_iMaterialIndex = 0;

private:
	_uint			m_iNumBones = 0;

private:
	/* ������ �ÿ� ���� �޽������̳ʿ� ������ �ִ� ���� ��Ƽ� ���̴��� ������. */
	/* �� ������ ��Ĥ��� ��ƿ������� ���� ��Ƶγ�. */
	vector<class CHierarchyNode*>			m_Bones;
	typedef vector<class CHierarchyNode*>	BONES;


	

private:
	/* ������ ������ ä�쳮. */
	HRESULT SetUp_VerticesDesc(class CModel* pModel, aiMesh* pMesh, _bool isAnim, _fmatrix PivotMatrix);
	HRESULT SetUp_IndicesDesc(aiMesh* pMesh);	
	HRESULT SetUp_SkinnedDesc(class CModel* pModel, aiMesh* pMesh);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, class CModel* pModel, _bool isAnimMesh, aiMesh* pMesh, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END