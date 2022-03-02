#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{	
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_ANIM_USE_OTHER, TYPE_END };

	typedef struct tagModelDesc
	{
		tagModelDesc() {
			ZeroMemory(this, sizeof(this));
		}
		vector<class CHierarchyNode*>* pHierarchyNodes;
		vector<class CAnimation*>* pAnimations;
		CHierarchyNode* pHierarchyNode;
	}MODELDESC;

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

	TYPE Get_MeshType() const {
		return m_eType;
	}

	 vector<class CHierarchyNode*>* Get_HierarchyNodes() {
		return &m_HierarchyNodes;
	}

	 vector<class CAnimation*>* Get_Animations() {
		return &m_Animations;
	}

public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eType, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	void SetUp_Animation(_uint iAnimIndex, _bool isLoop = true);
	void SetUp_Animation(const char* pNameKey, _bool isLoop = true);
	HRESULT Update_Animation(_float fTimeDelta);
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

	// Anim Name : "War_Mesh.ao|War_Idle"
	map<string, _uint>				m_AniNameKey_IdxValue_Map;
	typedef map<string, _uint>		ANIMATIONS_MAP;



private:
	_uint									m_iNumMeshes;

	vector<MESHMATERIAL>					m_Materials;
	typedef vector<MESHMATERIAL>			MATERIALS;
	_uint									m_iNumMaterials;

	TYPE									m_eType = TYPE_END;
	_float4x4								m_PivotMatrix;

	_uint									m_iNumAnimation;
	_uint									m_iCurrentAnimIndex = 0;
	_bool									m_isLoop = true;


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
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END