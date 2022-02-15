#pragma once

#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager);
public:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex = 0) const;

public:
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);

private:
	list<class CLight*>				m_Lights;
	typedef list<class CLight*>		LIGHTS;

public:
	virtual void Free() override;
};

END