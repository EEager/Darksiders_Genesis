#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
	CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render(_uint iPassIndex);

public:
	HRESULT Set_RawValue(const char* pConstantName, void* pData, _uint iSize);
	HRESULT	Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV);



protected:
	ID3D11Buffer*				m_pVB = nullptr;
	D3D11_BUFFER_DESC			m_VBDesc;
	D3D11_SUBRESOURCE_DATA		m_VBSubresourceData;
	_uint						m_iNumVertexBuffers = 0;
	void*						m_pVertices = nullptr;

	_uint						m_iStride = 0;
	_uint						m_iNumVertices = 0;


	ID3D11Buffer*				m_pIB = nullptr;
	D3D11_BUFFER_DESC			m_IBDesc;
	D3D11_SUBRESOURCE_DATA		m_IBSubresourceData;
	_uint						m_iNumPrimitive = 0;
	_uint						m_iIndicesSize = 0;
	void*						m_pPrimitiveIndices = nullptr;
	_uint						m_iNumIndicesPerFigure = 0;
	DXGI_FORMAT					m_eIndexFormat;
	D3D11_PRIMITIVE_TOPOLOGY	m_ePrimitiveTopology;	

protected:
	ID3DX11Effect*				m_pEffect = nullptr;
	vector<PASSDESC*>			m_PassesDesc;


protected:
	HRESULT Create_VertexBuffer();
	HRESULT Create_IndexBuffer();
	HRESULT Compile_Shader(D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements, const _tchar* pShaderFilePath);

public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END