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

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END