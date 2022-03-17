#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{	
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_ANIM_USE_OTHER, TYPE_ANIM_NEED_CONTINUE_BONES, TYPE_END };

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

	 _float4x4* Get_CombinedMatrixPtr(const char* pBoneName);
	 _float4x4 Get_OffsetMatrix(const char* pBoneName);
	 _float4x4 Get_PivotMatrix_Bones() {
		 return m_PivotMatrix;
	 }

	_fmatrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}
	void Set_PivotMatrix(_fmatrix vMat) {
		XMStoreFloat4x4(&m_PivotMatrix, vMat);
	}


public:
	virtual HRESULT NativeConstruct_Prototype(TYPE eType, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	void SetUp_Animation(_uint iAnimIndex, _bool isLoop = true);
	void SetUp_Animation(const char* pNameKey, _bool isLoop = true, _bool useLatestLerp = true);
	_bool Get_Animation_isFinished(const char* pNameKey);
	HRESULT Update_Animation(_float fTimeDelta, OUT _float4x4* pMatW = nullptr, const char* pRootNodeName = nullptr, class CNavigation* pNaviCom = nullptr);
	HRESULT Bind_Shader(_uint iPassIndex);
	HRESULT Render(_uint iMtrlIndex, _uint iPassIndex);

public:
	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, _uint iMeshContainerIndex, aiTextureType eTextureType);

public:
	class CHierarchyNode* Find_HierarchyNode(const char* pNodeName);

private:
	HRESULT Create_MeshContainers();
	HRESULT Create_Materials(const char* pModelFilePath);
	HRESULT Compile_Shader(const _tchar* pShaderFilePath);
	HRESULT Create_VertexIndexBuffers();
	HRESULT Create_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent = nullptr, _uint iDepth = 0);
	HRESULT Create_Animation();

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
	unordered_map<string, _uint>				m_AniNameKey_IdxValue_Map;
	typedef unordered_map<string, _uint>		ANIMATIONS_MAP;

private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;

private:
	_uint									m_iNumMeshes;

	vector<MESHMATERIAL>					m_Materials;
	typedef vector<MESHMATERIAL>			MATERIALS;
	_uint									m_iNumMaterials;

	TYPE									m_eType = TYPE_END;

private:
	_float4x4								m_PivotMatrix; // 모델자체 피봇

	_uint									m_iNumAnimation;
	_uint									m_iCurrentAnimIndex = 0;
	_bool									m_isLoop = true;


	
	// Enviroment 모델의 경우, 기준점이 되는 매쉬가 있다. (보통 plane vertices 6짜리.) 
	// 해당 기준점이 되는 매쉬의 위치를 가져올 수 있도록 하자.
public:
	_float3	m_vEnviromentBase = {};


	// For.Collider
public:
	void Set_Center(_fvector vCenter) {
		XMStoreFloat3(&m_Center, vCenter);
	}
	void Set_Extents(_fvector vExtents) {
		XMStoreFloat3(&m_Extents, vExtents);
	}

	_float3 Get_Center() {
		return m_Center;
		
	}
	_float3 Get_Extents() {
		return m_Extents;
	}

private: 
	_float3	m_Center; // Center of the box.
	_float3	m_Extents; // Distance from the center to each side.


private:
	ID3DX11Effect*							m_pEffect = nullptr;
	vector<PASSDESC*>						m_PassesDesc;

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eType, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END