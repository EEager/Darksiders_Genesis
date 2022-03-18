#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStateMachine;
class CCollider;
class CNavigation;
END

BEGIN(Client)

class CWar final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };
	enum MODELTYPE { MODELTYPE_WAR, MODELTYPE_GAUNTLET, MODELTYPE_WEAPON, MODELTYPE_END };
	enum WAR_G_TYPE { G_TYPE_FIRE, G_TYPE_EARTH, G_TYPE_END	}; // 콤보 스킬 타입

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
	_vector Get_War_Pos(); // used in camera_fly

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CStateMachine* m_pStateMachineCom = nullptr;
	CCollider* m_pAABBCom = nullptr;
	CCollider* m_pOBBCom = nullptr;
	CModel* m_pModelCom[MODELTYPE_END] = { 0, };
	CModel* m_pModelCom_Ruin = nullptr;
	CNavigation* m_pNaviCom = nullptr;

private:
	ID3D11ShaderResourceView* pSRV = nullptr;

	// -----------------------------------------------------------------
	// 아래들은 거의 FSM에서 사용
public:
	void Set_Jump(_bool warJump = true, _bool clearJump = true);
	_bool Get_Jump() { return m_bJump; }

	void Set_Dont_Key(_bool _b) { m_bDont_Key = _b; }
	void Set_DontMove_OnlyTurn(_bool _b) { m_bDontMove_OnlyTurn = _b; }
	void Set_DontTurn_OnlyMove(_bool _b) { m_bDontTurn_OnlyMove = _b; }
	void Set_War_On_Ruin_State(_bool _b) { m_War_On_Ruin_State = _b; }
	_bool Get_War_On_Ruin_State() { return m_War_On_Ruin_State; }
	
	_float Get_Speed();
	void Set_Speed(const _float& fSpeed);

	WAR_G_TYPE Get_GType() { return m_eGType; }

private:
	_bool	m_bJump = false; // 점프했는지 안했느지 판단
	_bool	m_bDont_Key = false; // War_Key 타지말자
	_bool	m_bDontMove_OnlyTurn = false; // 땅 콤보 스킬 사용할때는 움직이면 안된다.
	_bool	m_bDontTurn_OnlyMove = false; // 불 콤보 스킬 사용할때는 회전하면 안된다
	_bool	m_War_Key_Lock = false; // 일부 FSM에서는 움직이지말아야한다. 하지만 이건 detail 작업이므로 나중에 넣자.
	_bool	m_War_On_Ruin_State = false; // 말타고 있는 상태.

	WAR_G_TYPE m_eGType = G_TYPE_EARTH;


public:
	_matrix		Get_WarPivot() { return XMLoadFloat4x4(&m_WarPivotMat); }  // 말타기 상태가 아닐때, War Model 피봇 매트릭스를 이걸로. 참고로 War_Ruin_Model은 ㄱㅊ
	_matrix		Get_WarRuinPivot() { return XMLoadFloat4x4(&m_WarRuinPivotMat); }// 말타기 상태일 때, War Model한테 이거를 셋해줘야한다. 참고로 War_Ruin_Model은 ㄱㅊ

private:
	_float4x4	m_WarPivotMat; // 말안타고 있을때의 War Model한테 적용할매트릭스를 잠시 저잦장. 참고로 War_Ruin_Model꺼 아님
	_float4x4	m_WarRuinPivotMat; // 말타고 있을때의 War Model한테 적용할매트릭스를 잠시 저잦장. 참고로 War_Ruin_Model꺼 아님

	// -----------------------------------------------------------------
	// 아래 3개는 War 키 입력에 사용
private:
	_float GetDegree(_ubyte downedKey);
	_bool KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit);
public:
	void War_Key(_float fTimeDelta);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(bool drawOutLine, int modelIdx = 0);
	HRESULT SetUp_Ruin_ConstantTable(bool drawOutLine);

	HRESULT War_Render();


public:
	static CWar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END