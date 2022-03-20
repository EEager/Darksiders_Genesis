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
	typedef struct tagSpearDesc
	{
		_float4x4		OffsetMatrix;
		_float4x4* pBoneMatrix;
		_float4x4		PivotMatrix;
		_float4x4* pTargetWorldMatrix;
	}SPEARDESC;
	SPEARDESC		m_spearDesc;

	typedef struct tagQuiverDesc
	{
		_float4x4		OffsetMatrix;
		_float4x4* pBoneMatrix;
		_float4x4		PivotMatrix;
		_float4x4* pTargetWorldMatrix;
	}QUIVERDESC;
	QUIVERDESC		m_quiverDesc;

private:
	explicit CGoblin_Armor(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGoblin_Armor(const CGoblin_Armor& rhs);
	virtual ~CGoblin_Armor() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

private:
	CModel* m_pModelSpearCom = nullptr;
	CModel* m_pModelQuiverCom = nullptr;

public:	
	static CGoblin_Armor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END