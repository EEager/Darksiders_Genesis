#pragma once

#include "Monster\Monster.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CNavigation;
class CModel;
END

BEGIN(Client)

class CLegion final : public CMonster
{
private:
	typedef struct tagSwordDesc
	{
		_float4x4		OffsetMatrix;
		_float4x4* pBoneMatrix;
		_float4x4		PivotMatrix;
		_float4x4* pTargetWorldMatrix;
	}SWORDDESC;

private:
	explicit CLegion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLegion(const CLegion& rhs);
	virtual ~CLegion() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CModel* m_pModelWeaponCom = nullptr;
	SWORDDESC		m_WarSwordDesc;

public:	
	static CLegion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END