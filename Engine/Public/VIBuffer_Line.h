#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Line final : public CVIBuffer
{
protected:
	CVIBuffer_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Line(const CVIBuffer_Line& rhs);
	virtual ~CVIBuffer_Line() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, _float3* pPoints, _uint iNumVertices);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render(_uint iPassIndex);

public:

public:
	static CVIBuffer_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, _float3* pPoints, _uint iNumVertices, const _tchar* pShaderFilePath = TEXT("../Bin/ShaderFiles/shader_Line.hlsl"));
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
