#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CCollider;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CSword final : public CGameObject
{
private:
	typedef struct tagSwordDesc
	{
		_float4x4		OffsetMatrix;
		_float4x4*		pBoneMatrix;
		_float4x4		PivotMatrix;
		_float4x4*		pTargetWorldMatrix;
	}SWORDDESC;
private:
	explicit CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSword(const CSword& rhs);
	virtual ~CSword() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;	
	CCollider*					m_pSphereCom = nullptr;
	SWORDDESC					m_SwordDesc;

private:
	

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_BoneMatrix();
	HRESULT SetUp_ConstantTable();	

public:	
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END