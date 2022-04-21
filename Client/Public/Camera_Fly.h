#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "War.h"

BEGIN(Client)

class CCamera_Fly final : public CCamera
{
public:
	enum CAMERA_MODE { MODE_FREE, MODE_TARGET, MODE_END};
protected:
	explicit CCamera_Fly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Fly(const CCamera& rhs);
	virtual ~CCamera_Fly() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual _int LateTick(_float fTimeDelta) override;
	virtual HRESULT Render(_uint iPassIndex=0) override;


public:
	CAMERA_MODE Get_Type() { return m_eType; }
	void Set_Type(CAMERA_MODE eType) { m_eType=eType; }

private:
	void CameraFly_Key(_float fTimeDelta);

private:
	CAMERA_MODE m_eType = MODE_TARGET;

public: // 타겟팅
	_bool m_bSetTargetOnce = false; // War 타겟팅을 Tick에서 하는데 한번만하자.
	void Set_BoneMat(bool useBone, _matrix transformationMat)
	{
		if (useBone)
			XMStoreFloat4x4(&m_matTransformation, transformationMat);
		m_bUseBoneMat = useBone;

	}
	_float4x4 m_matTransformation;
	_bool m_bUseBoneMat = false;
	

public:
	void Set_Radius(_float r) {
		m_fRadius = r;
	}
	void Set_Radian(_float r) {
		m_fRadian = r;
		if (m_fRadian < 0)
			m_fRadian = 2 * XM_PI;
		if (m_fRadian >= 2 * XM_PI)
			m_fRadian = 0.f;
	}
	void Set_Height(_float r) {
		m_fHeight = r;
	}
	void Set_Position_Ratio(_float r) {
		m_fPositionRatio = r;
	}
	void Set_LookAt_Ratio(_float r) {
		m_fLookAtRatio = r;
	}

	_float m_fRadius = 13.1f; // 타겟간의 거리 
	_float m_fRadian = XMConvertToRadians(250.f); // +x가 0도임. 반시계방향
	_float m_fHeight = 11.f;
	_float m_fPositionRatio = 0.03f;
	_float m_fLookAtRatio = 0.05f;

public:
	static CCamera_Fly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END