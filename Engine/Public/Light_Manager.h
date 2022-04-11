#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight_Manager final : public CBase
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
	HRESULT Update(_float fTimeDelta);

private:
	list<class CLight*>				m_Lights;
	typedef list<class CLight*>		LIGHTS;

private:
	MTRLDESC m_tMtrlDesc;

private:
	class CVIBuffer_Rect* m_pVIBuffer = nullptr;
	_float4x4						m_TransformMatrix;
	_float4x4						m_OrthoMatrix;


	// --------------------------------------------
	// For.ShadowMapping
public:
	_float4x4* Get_Env_Light_View() { return &m_LightView_Env; }
	_float4x4* Get_Env_Light_Proj() { return &m_LightProj_Env; }
	_float4x4* Get_Env_Light_ShadowMat() { return &m_ShadowTransform_Env; }

	_float4x4* Get_Objects_Light_View() {return &m_LightView_Objects;}
	_float4x4* Get_Objects_Light_Proj() {return &m_LightProj_Objects;}
	_float4x4* Get_Objects_Light_ShadowMat() {return &m_ShadowTransform_Objects;}

private:
	_float4x4						m_LightView_Env;
	_float4x4						m_LightProj_Env;
	_float4x4						m_ShadowTransform_Env;

	_float4x4						m_LightView_Objects;
	_float4x4						m_LightProj_Objects;
	_float4x4						m_ShadowTransform_Objects;

private:
	class CGameObject* m_pTarget = nullptr;
	class CTransform* m_pTargetTransform = nullptr;

public:
	_float m_fSceneRadius = SHADOWMAP_INIT_RADIUS;

	// ----------------------------------------------------

public:
	virtual void Free() override;
};

END