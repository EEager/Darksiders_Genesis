#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "State_War.h"

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
	friend class CGlobal_State_War;

private:
	typedef struct tagSwordDesc
	{
		_float4x4		OffsetMatrix;
		_float4x4*		pBoneMatrix;
		_float4x4		PivotMatrix;
		_float4x4*		pTargetWorldMatrix;
	}SWORDDESC;
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
	virtual HRESULT Render(_uint iPassIndex=0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);


public:
	_vector Get_War_Pos(); // used in camera_fly
	CStateMachine* Get_StateMachine() { return m_pStateMachineCom; }

private:
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CStateMachine*	m_pStateMachineCom = nullptr;
	CCollider*		m_pAABBCom = nullptr;
	CCollider*		m_pOBBCom = nullptr;
	CModel*			m_pModelCom[MODELTYPE_END] = { 0, };
	CModel*			m_pModelCom_Ruin = nullptr;
	CNavigation*	m_pNaviCom = nullptr;
	SWORDDESC		m_WarSwordDesc;

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
	void Set_War_On_Ruin_State(_bool _b) { m_War_On_Ruin_State = _b; }
	_bool Get_War_On_Ruin_State() { return m_War_On_Ruin_State; }
	
	_float Get_Speed();
	void Set_Speed(const _float& fSpeed);

	WAR_G_TYPE Get_GType() { return m_eGType; }

private:
	_bool	m_bJump = false; // �����ߴ��� ���ߴ��� �Ǵ�
	_bool	m_bDont_Key = false; // War_Key Ÿ������
	_bool	m_bDontMove_OnlyTurn = false; // �� �޺� ��ų ����Ҷ��� �����̸� �ȵȴ�.
	_bool	m_bDontTurn_OnlyMove = false; // �� �޺� ��ų ����Ҷ��� ȸ���ϸ� �ȵȴ�
	_bool	m_War_Key_Lock = false; // �Ϻ� FSM������ �����������ƾ��Ѵ�. ������ �̰� detail �۾��̹Ƿ� ���߿� ����.
	_bool	m_War_On_Ruin_State = false; // ��Ÿ�� �ִ� ����.

	WAR_G_TYPE m_eGType = G_TYPE_EARTH;


public:
	_matrix		Get_WarPivot() { return XMLoadFloat4x4(&m_WarPivotMat); }  // ��Ÿ�� ���°� �ƴҶ�, War Model �Ǻ� ��Ʈ������ �̰ɷ�. ����� War_Ruin_Model�� ����
	_matrix		Get_WarRuinPivot() { return XMLoadFloat4x4(&m_WarRuinPivotMat); } // ��Ÿ�� ������ ��, War Model���� �̰Ÿ� ��������Ѵ�. ����� War_Ruin_Model�� ����

private:
	_float4x4	m_WarPivotMat; // ����Ÿ�� �������� War Model���� �����Ҹ�Ʈ������ ��� ������. ����� War_Ruin_Model�� �ƴ�
	_float4x4	m_WarRuinPivotMat; // ��Ÿ�� �������� War Model���� �����Ҹ�Ʈ������ ��� ������. ����� War_Ruin_Model�� �ƴ�

public:
	_bool m_bDontMoveInWorld = false;

	// -----------------------------------------------------------------
	// �Ʒ� 3���� War Ű �Է¿� ���
private:
	_float GetDegree(_ubyte downedKey);
	_bool KeyCheck(IN _ubyte key, OUT _ubyte& keyDownCheckBit);
public:
	void War_Key(_float fTimeDelta);


	// ----------------------------------------------------------------
	// Collider
	virtual _int Update_Colliders(_matrix wolrdMatrix = XMMatrixIdentity()) override;

public:
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_BoneMatrix();
	HRESULT SetUp_ConstantTable(_uint iPssIndex, bool drawOutLine, int modelIdx = 0);
	HRESULT SetUp_Ruin_ConstantTable(_uint iPassIndex, bool drawOutLine);

	HRESULT War_Render(_uint iPassIndex = 0);
	HRESULT War_Outline_Render(_uint iPassIndex = 0);

	// GamePlay
	_bool m_bHitted = false; // ���� �ǰݽ�. Render������ ���̴��� �����, Tick������ ü�°�������
	_float m_fHitPower = 0.f; // ���� �ǰݽ�. Render������ ���̴��� �����, Tick������ ü�°�������
	const _float HIT_DELAY = 5.f; // �ǰ� ���ӽð�
	_float m_fHitTimeAcc = 0.f;

#define HIT_FROM_FRONT 0
#define HIT_FROM_BACK 1
#define HIT_FROM_RIGHT 2
#define HIT_FROM_LEFT 3
	void Set_Collision_Direction(CTransform* pDstTransform); // m_iHitDir�� Set �Ѵ�

public:
	_bool m_bSuperArmor = false; // Ư�� �ִϸ��̼��� ���۾Ƹӻ����̴�. �з����� �ʴ´�.
	int m_iHitDir = -1; // ������⿡�� �ǰ��Ͽ����� �� �� �ְ�����. -1 : ���� �ǰݾȴ��ߴ�.
	OBJECT_DIR m_eDir = OBJECT_DIR::DIR_F;

private:
	class CTrail* m_pTrail = nullptr;
public:
	bool m_bTrailOn = false;



public:
	static CWar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END