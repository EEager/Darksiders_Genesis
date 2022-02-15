#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Fly final : public CCamera
{
protected:
	explicit CCamera_Fly(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera_Fly(const CCamera& rhs);
	virtual ~CCamera_Fly() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_float fTimeDelta) override;
	virtual _int LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


public:
	static CCamera_Fly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END