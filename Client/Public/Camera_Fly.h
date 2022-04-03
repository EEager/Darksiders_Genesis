#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "War.h"

BEGIN(Client)

class CCamera_Fly final : public CCamera
{
public:
	enum TYPE_MODE { MODE_FREE, MODE_WAR, MODE_END};
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
	TYPE_MODE Get_Type() { return m_eType; }

private:
	void CameraFly_Key(_float fTimeDelta);

private:
	TYPE_MODE m_eType = MODE_FREE;

public: // Ÿ����
	_bool m_bSetTargetOnce = false; // War Ÿ������ Tick���� �ϴµ� �ѹ�������.
	CWar* m_pWar = nullptr; // War Ÿ��������
	_float m_fRadius = 13.1f; // Ÿ�ٰ��� �Ÿ� 
	_float m_fRadian = XMConvertToRadians(250.f); // +x�� 0����. �ݽð����...
	_float m_fHeight = 13.f;

public:
	static CCamera_Fly* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END