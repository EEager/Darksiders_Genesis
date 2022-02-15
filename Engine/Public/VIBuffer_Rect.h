#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
protected:
	CVIBuffer_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Rect(const CVIBuffer_Rect& rhs);
	virtual ~CVIBuffer_Rect() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT NativeConstruct(void* pArg);

public:
	static CVIBuffer_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END
