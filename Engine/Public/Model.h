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

public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	HRESULT Render(_uint iMeshContainerIndex, _uint iPassIndex);

public:
	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, _uint iMeshContainerIndex, aiTextureType eTextureType);

private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;

private:
	vector<class CMeshContainer*>			m_MeshContainers;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;
	_uint									m_iNumMeshes;

	vector<MESHMATERIAL>					m_Materials;
	typedef vector<MESHMATERIAL>			MATERIALS;
	_uint									m_iNumMaterials;

	_bool									m_isAnimMesh = false;
	_float4x4								m_PivotMatrix;


private:
	ID3DX11Effect*							m_pEffect = nullptr;
	vector<PASSDESC*>						m_PassesDesc;

private:
	HRESULT Create_MeshContainers();
	HRESULT Create_Materials(const char* pModelFilePath);
	HRESULT Compile_Shader(const _tchar* pShaderFilePath);
	HRESULT Create_VertexIndexBuffers();
	



public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END