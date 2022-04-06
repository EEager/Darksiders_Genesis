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
	void Set_Target(class CGameObject* pObj)
	{
		Safe_Release(m_pTarget);
		m_pTarget = pObj;
		Safe_AddRef(m_pTarget);
	}
	CAMERA_MODE Get_Type() { return m_eType; }
	void Set_Type(CAMERA_MODE eType) { m_eType=eType; }

private:
	void CameraFly_Key(_float fTimeDelta);

private:
	CAMERA_MODE m_eType = MODE_FREE;

public: // 타겟팅
	_bool m_bSetTargetOnce = false; // War 타겟팅을 Tick에서 하는데 한번만하자.
	class CGameObject* m_pTarget = nullptr;
	_float m_fRadius = 13.1f; // 타겟간의 거리 
	_float m_fRadian = XMConvertToRadians(250.f); // +x가 0도임. 반시계방향...
	_float m_fHeight = 13.f;

public:
	// 카메라 위치를 vector에 저장하자. 만약 누군가가 채워넣었다면. 하나씩 꺼내어 현재 위치에서 Lerp 시키도록 하자. 

	// 카메라 시선을 vector에 저장하자. 만약 눈군가가 채워, 하나씩 꺼내어 현재 위치에서 Lerp 시키도록 하자

public:
	static CCamera_Fly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END