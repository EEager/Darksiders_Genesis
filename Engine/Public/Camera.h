#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		XMFLOAT3						vEye, vAt, vAxisy;
		float							fFovy, fAspect, fNear, fFar;
		CTransform::TRANSFORMDESC		TransformDesc;
	}CAMERADESC;
protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;	
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual _int LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CAMERADESC					m_CameraDesc;
	class CTransform*			m_pTransform = nullptr;	

public:// Used in ImGUI
	CTransform* Get_Camera_Transform() { return m_pTransform; }
	void Set_Camera_Speed(_float speed) {
		m_CameraDesc.TransformDesc.fSpeedPerSec = speed;
		if (m_pTransform)
			m_pTransform->Set_TransformDesc(m_CameraDesc.TransformDesc);
	}
	const CAMERADESC Get_Camera_Desc() const { return m_CameraDesc; }

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END