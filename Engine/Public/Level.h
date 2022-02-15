#pragma once

/* 클라이언트에 정의한 레벨클래스들의 부모클래스다. */

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	explicit CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel() = default;
public:
	virtual HRESULT NativeConstruct();
	virtual _int Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pDeviceContext = nullptr;

public:
	virtual void Free() override;
};

END