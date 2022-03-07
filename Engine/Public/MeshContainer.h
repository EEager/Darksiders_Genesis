#pragma once

/* 모델 = 다수의 메시컨테이너로 구성된다. */
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
	/* 현재 메쉬컨테이너가 어떤 머테리얼 인덱스를 쓰는지 */
	_uint			m_iMaterialIndex = 0;

private:
	_uint			m_iNumBones = 0;

private:
	/* 렌더링 시에 현재 메시컨테이너에 영향을 주는 뼈를 모아서 셰이더로 던진다. */
	/* 그 뼈들의 행렬ㄹ을 모아오기위해 뼈를 모아두낟. */
	vector<class CHierarchyNode*>			m_Bones; // 일단 모든 뼈들을 넣고보는군. 
	typedef vector<class CHierarchyNode*>	BONES;

	aiMesh* m_pAIMesh = nullptr;
 


	

private:
	/* 정점의 정보를 채우낟. */
	HRESULT SetUp_VerticesDesc(class CModel* pModel, aiMesh* pMesh, _fmatrix PivotMatrix, OUT XMVECTOR* pMin, OUT XMVECTOR* pMax);
	HRESULT SetUp_IndicesDesc(aiMesh* pMesh);	
	HRESULT SetUp_SkinnedDesc(class CModel* pModel, aiMesh* pMesh);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, CModel* pModel, aiMesh* pMesh, _fmatrix PivotMatrix, OUT XMVECTOR* pMin, OUT XMVECTOR* pMax);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END