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
	const char* m_pCurState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle";
	const char* m_pNextState = "Goblin_Armor_Mesh.ao|Goblin_SnS_Idle";

	void UpdateState(); // m_eCurState Exit, m_eNextState Enter
	void DoState(float fTimeDelta); // m_eCurState Execute 

	_float Get_Target_Dis(float fTimeDelta = 0.f);
	_float GetDegree_Target();

private:
	const _float ATK_RANGE = 5.5f;
	const _float SPEAR_RANGE = 50.f;
	const _float CHASE_RANGE = 60.f;
	const _float IDLE_TIME_TO_ATK_DELAY = 1.5f;

	CGameObject* m_pTarget = nullptr;
	CTransform* m_pTargetTransform = nullptr;

	_bool m_bNotSpearAtk = false;

	_float m_fTimeIdle = 0.f;
	OBJECT_DIR m_eDir = OBJECT_DIR::DIR_F;
	//----------------------------------------------


public:	
	static CGoblin_Armor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END