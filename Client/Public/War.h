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
	enum WAR_G_TYPE { G_TYPE_FIRE, G_TYPE_EARTH, G_TYPE_END	}; // �޺� ��ų Ÿ��

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

private:
	ID3D11ShaderResourceView* pSRV = nullptr;

	// -----------------------------------------------------------------
	// �Ʒ����� ���� FSM���� ���
public:
	void Set_Jump(_bool warJump = true, _bool clearJump = true);
	_bool Get_Jump() { return m_bJump; }

	void Set_Dont_Key(_bool _b) { m_bDont_Key = _b; }
	void Set_DontMove_OnlyTurn(_bool _b) { m_bDontMove_OnlyTurn = _b; }
	void Set_DontTurn_OnlyMove(_bool _b) { m_bDontTurn_OnlyMove = _b; }

	_float Get_Speed();
	void Set_Speed(const _float& fSpeed);

	WAR_G_TYPE Get_GType() { return m_eGType; }

private:
	_bool	m_bJump = false; // �����ߴ��� ���ߴ��� �Ǵ�
	_bool	m_bDont_Key = false; // War_Key Ÿ������
	_bool	m_bDontMove_OnlyTurn = false; // �� �޺� ��ų ����Ҷ��� �����̸� �ȵȴ�.
	_bool	m_bDontTurn_OnlyMove = false; // �� �޺� ��ų ����Ҷ��� ȸ���ϸ� �ȵȴ�
	_bool	m_War_Key_Lock = false; // �Ϻ� FSM������ �����������ƾ��Ѵ�. ������ �̰� detail �۾��̹Ƿ� ���߿� ����.

	WAR_G_TYPE m_eGType = G_TYPE_EARTH;
	// -----------------------------------------------------------------
	// �Ʒ� 3���� War Ű �Է¿� ���
private:
	_float GetDegree(_ubyte downedKey);
	_bool KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit);
public:
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