#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };
	enum MODELTYPE { MODELTYPE_WAR, MODELTYPE_GAUNTLET, MODELTYPE_END };// MODELTYPE_WEAPON, MODELTYPE_END };

private:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;


	CModel*						m_pModelCom[MODELTYPE_END] = { 0, };

private:
	ID3D11ShaderResourceView*	pSRV = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(int modelIdx = 0);	

public:	
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END