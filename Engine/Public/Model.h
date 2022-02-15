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
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName);
	virtual HRESULT NativeConstruct(void* pArg);

private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;

private:
	vector<class CMeshContainer*>			m_MeshContainers;
	typedef vector<class CMeshContainer*>	MESHCONTAINERS;
	_uint									m_iNumMeshes;
	_bool									m_isAnimMesh = false;

private:
	ID3DX11Effect*							m_pEffect = nullptr;
	vector<PASSDESC*>						m_PassesDesc;

private:
	HRESULT Create_MeshContainers();
	HRESULT Compile_Shader(const _tchar* pShaderFilePath);
	HRESULT Create_VertexIndexBuffers();
	



public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const char* pModelFilePath, const char* pModelFileName);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END