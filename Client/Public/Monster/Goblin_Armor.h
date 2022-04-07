#pragma once

#include "Monster\Monster.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CNavigation;
class CModel;
class CVIBuffer_MonsterHp_PointGS;
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
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	void Render_Goblin(_uint iPassIndex);

protected:
	virtual _int Update_Colliders(_matrix wolrdMatrix = XMMatrixIdentity());

private:
	CModel* m_pModelSpearCom = nullptr;
	CModel* m_pModelQuiverCom = nullptr;

	CVIBuffer_MonsterHp_PointGS* m_pVIHpBarGsBufferCom = nullptr;

	//----------------------------------------------
	// FSM
private:
	void UpdateState(); // m_eCurState Exit, m_eNextState Enter
	void DoState(float fTimeDelta); // m_eCurState Execute 

	_float Get_Target_Dis(float fTimeDelta = 0.f);
	_float GetDegree_Target();

private:
	const _float ATK_RANGE = 5.5f;
	const _float SPEAR_RANGE = 50.f;
	const _float CHASE_RANGE = 60.f;
	const _float IDLE_TIME_TO_ATK_DELAY = 1.5f;

	_bool m_bNotSpearAtk = false;

	_float m_fTimeIdle = 0.f;
	OBJECT_DIR m_eDir = OBJECT_DIR::DIR_F;

	_bool m_bSpearFired = false;
	_float4	m_vTargetDir;

	_bool m_bSpawning = false;

	//----------------------------------------------


public:	
	static CGoblin_Armor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};












// -----------------------------------
// CGoblin_Spear
class CGoblin_Spear final : public CGameObject
{
	friend class CGoblin;

public:
	typedef struct tagCloneDesc
	{
		_float4 initPos;
		_float4 initDir;
	}CLONEDESC;

private:
	typedef struct tagBoltDesc
	{
		_float4x4		OffsetMatrix;
		_float4x4* pBoneMatrix;
		_float4x4		PivotMatrix;
		_float4x4* pTargetWorldMatrix;
	}SPEARDESC;
	SPEARDESC		m_spearDesc;

private:
	explicit CGoblin_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGoblin_Spear(const CGoblin_Spear& rhs);
	virtual ~CGoblin_Spear() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIdx);

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;

private:
	// 화살 주인을 등록하여 주인이 죽었을 경우를 판단하자. (본 행렬을 주인꺼를 따라하기때문에 터질수가있다)
	class CGameObject* m_pOwner = nullptr;
	_float m_fLifeTime = 0.f;

public:
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

public:
	static CGoblin_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END