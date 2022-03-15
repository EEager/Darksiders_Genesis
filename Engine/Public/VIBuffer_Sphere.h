#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Sphere final : public CVIBuffer
{
protected:
	CVIBuffer_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Sphere(const CVIBuffer_Sphere& rhs);
	virtual ~CVIBuffer_Sphere() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath, float radiust, _uint stackCount, _uint sliceCount);
	virtual HRESULT NativeConstruct(void* pArg);

private:
	float radius;
	UINT stackCount;
	UINT sliceCount;

public:
	static CVIBuffer_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath, float radiust, _uint stackCount = 10, _uint sliceCount = 10);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
