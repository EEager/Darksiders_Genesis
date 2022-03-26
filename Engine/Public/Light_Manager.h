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
	LIGHTDESC* Get_LightDesc(_uint iIndex = 0);
	void Set_LightDesc(_uint iIndex, const LIGHTDESC& LightDesc);

	list<class CLight*>* Get_LightList_Addr() { return &m_Lights; }


public:
	HRESULT NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	HRESULT Render();

private:
	list<class CLight*>				m_Lights;
	typedef list<class CLight*>		LIGHTS;

private:
	class CVIBuffer_Rect* m_pVIBuffer = nullptr;
	_float4x4						m_TransformMatrix;
	_float4x4						m_OrthoMatrix;

public:
	virtual void Free() override;
};

END