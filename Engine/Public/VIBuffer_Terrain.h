#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, const _tchar* pHeightMapFilePath);
	HRESULT NativeConstruct_Prototype(const _tchar * pShaderFilePath, _uint iNumVerticesX, _uint iNumVerticesZ);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	_float Compute_Height(_fvector vPosition);

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, const _tchar* pHeighMapFilePath);
	static CVIBuffer_Terrain* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, const _tchar * pShaderFilePath, _uint iNumVerticesX, _uint iNumVerticexZ);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
