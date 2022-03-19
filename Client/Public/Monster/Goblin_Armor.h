#pragma once

#include "Monster\Monster.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CNavigation;
class CModel;
END

BEGIN(Client)

class CGoblin_Armor final : public CMonster
{
private:
	explicit CGoblin_Armor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGoblin_Armor(const CGoblin_Armor& rhs);
	virtual ~CGoblin_Armor() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:	
	static CGoblin_Armor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END