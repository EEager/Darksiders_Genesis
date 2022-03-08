#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStateMachine;
class CCollider;
END

BEGIN(Client)

class CWar final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };
	enum MODELTYPE { MODELTYPE_WAR, MODELTYPE_GAUNTLET, MODELTYPE_WEAPON, MODELTYPE_END };

private:
	explicit CWar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CWar(const CWar& rhs);
	virtual ~CWar() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render();


public:
	_vector Get_War_Pos();

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CStateMachine*				m_pStateMachineCom = nullptr;
	CCollider*					m_pAABBCom = nullptr;
	CCollider*					m_pOBBCom = nullptr;
	CModel*						m_pModelCom[MODELTYPE_END] = { 0, };

private:
	ID3D11ShaderResourceView*	pSRV = nullptr;


private:
	_bool	m_bJump = false;

private:
	_float GetDegree(_ubyte downedKey);
	_bool KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit);
	void War_Key(_float fTimeDelta);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(bool drawOutLine, int modelIdx = 0);	

public:	
	static CWar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END