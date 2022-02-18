#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{	
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	_uint Get_NumMeshContainer() const {
		return m_iNumMeshes;
	}

	_uint Get_NumMaterials() const {
		return m_iNumMaterials;
	}

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Bind_Shader(_uint iPassIndex);
	HRESULT Render(_uint iMtrlIndex, _uint iPassIndex);

public:
	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, _uint iMeshContainerIndex, aiTextureType eTextureType);

public:
	class CHierarchyNode* Find_HierarchyNode(const char* pNodeName);


private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;

private:
	vector<vector<class CMeshContainer*>>			m_MeshContainers;
	typedef vector<vector<class CMeshContainer*>>	MESHCONTAINERS;

private:
	vector<class CHierarchyNode*>					m_HierarchyNodes;
	typedef vector<class CHierarchyNode*>			HIERARCHYNODES;

private:
	vector<class CAnimation*>						m_Animations;
	typedef vector<class CAnimation*>				ANIMATIONS;

private:
	_uint									m_iNumMeshes;

	vector<MESHMATERIAL>					m_Materials;
	typedef vector<MESHMATERIAL>			MATERIALS;
	_uint									m_iNumMaterials;

	_bool									m_isAnimMesh = false;
	_float4x4								m_PivotMatrix;

	_uint									m_iNumAnimation;


private:
	ID3DX11Effect*							m_pEffect = nullptr;
	vector<PASSDESC*>						m_PassesDesc;

private:
	HRESULT Create_MeshContainers();
	HRESULT Create_Materials(const char* pModelFilePath);
	HRESULT Compile_Shader(const _tchar* pShaderFilePath);
	HRESULT Create_VertexIndexBuffers();
	HRESULT Create_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent = nullptr, _uint iDepth = 0);
	HRESULT Create_Animation();
	



public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END