#pragma once

/* �� = �ټ��� �޽������̳ʷ� �����ȴ�. */
#include "VIBuffer.h"

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	explicit CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT NativeConstruct_Prototype(class CModel* pModel, aiMesh* pMesh, _fmatrix PivotMatrix, OUT XMVECTOR* pMin, OUT XMVECTOR* pMax);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render();

public:
	HRESULT Create_VertexIndexBuffer();
	HRESULT SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);
	HRESULT Add_Bones(class CModel* pModel, class CHierarchyNode* parentHierarchyNode = nullptr);

private:
	/* ���� �޽������̳ʰ� � ���׸��� �ε����� ������ */
	_uint			m_iMaterialIndex = 0;

private:
	_uint			m_iNumBones = 0;

private:
	/* ������ �ÿ� ���� �޽������̳ʿ� ������ �ִ� ���� ��Ƽ� ���̴��� ������. */
	/* �� ������ ��Ĥ��� ��ƿ������� ���� ��Ƶγ�. */
	vector<class CHierarchyNode*>			m_Bones; // �ϴ� ��� ������ �ְ��±�. 
	typedef vector<class CHierarchyNode*>	BONES;

	aiMesh* m_pAIMesh = nullptr;
 


	

private:
	/* ������ ������ ä�쳮. */
	HRESULT SetUp_VerticesDesc(class CModel* pModel, aiMesh* pMesh, _fmatrix PivotMatrix, OUT XMVECTOR* pMin, OUT XMVECTOR* pMax);
	HRESULT SetUp_IndicesDesc(aiMesh* pMesh);	
	HRESULT SetUp_SkinnedDesc(class CModel* pModel, aiMesh* pMesh);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CModel* pModel, aiMesh* pMesh, _fmatrix PivotMatrix, OUT XMVECTOR* pMin, OUT XMVECTOR* pMax);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END