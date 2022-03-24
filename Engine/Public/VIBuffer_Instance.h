#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instance abstract : public CVIBuffer
{
protected:
	CVIBuffer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CVIBuffer_Instance(const CVIBuffer_Instance& rhs);
	virtual ~CVIBuffer_Instance() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual HRESULT Render(_uint iPassIndex);

public:
	void Update(_float fTimeDelta);

protected:
	ID3D11Buffer*				m_pVBInst = nullptr;
	D3D11_BUFFER_DESC			m_VBInstDesc;
	D3D11_SUBRESOURCE_DATA		m_VBInstSubresourceData;
	_uint						m_iNumInstance = 0; // 인스턴스 총 개수 
	_float*						m_pInstanceSpeed = nullptr; // 각 포인트마다 속도를 가지고 있다. 
	_uint						m_iIndexCountPerInstance = 0;

public:	
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END