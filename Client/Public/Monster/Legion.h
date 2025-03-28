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

class CLegion final : public CMonster
{
private:
	typedef struct tagSwordDesc
	{
		_float4x4		OffsetMatrix;
		_float4x4* pBoneMatrix;
		_float4x4		PivotMatrix;
		_float4x4* pTargetWorldMatrix;
	}SWORDDESC;
	SWORDDESC		m_WarSwordDesc;

private:
	explicit CLegion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CLegion(const CLegion& rhs);
	virtual ~CLegion() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	HRESULT Render_Weapon(CModel* pModel, _float fRadian, _uint iPassIndex);

private:
	CModel* m_pModelWeaponLCom = nullptr;
	CModel* m_pModelWeaponRCom = nullptr;
	CVIBuffer_MonsterHp_PointGS* m_pVIHpBarGsBufferCom = nullptr;

	//----------------------------------------------
	// FSM
protected: 
	virtual void UpdateState(); // m_eCurState Exit, m_eNextState Enter
	virtual void DoState(float fTimeDelta); // m_eCurState Execute 
	_float GetDegree_Target_Ballista(class CTransform* pTargetTransform, class CTransform* pBallistaTransformCom);

private:
	const _float ATK_RANGE = 4.5f;
	const _float CHASE_RANGE = 60.f;
	const _float IDLE_TIME_TO_ATK_DELAY = 1.5f;

	_float m_fTimeIdle = 0.f;
	_float m_fTimeKnockBackLoop = 0.f;
	OBJECT_DIR m_eDir = OBJECT_DIR::DIR_F;
	_bool m_bSpawning = false;

public:
	CGameObject* Get_Ballista() { return m_pBallista; }
private:
	class CGameObject* m_pBallista = nullptr;
	//----------------------------------------------

private:
	_float4 m_vFloatingDir; // 죽기전에 날아가는 방향. Nativecontstruct에서 정의해주ㅏ
	_float m_fFloatingPwr; // 죽기전에 날아가는 힘.
	_float m_fKnockBack_Start_Height = 0.f;


protected:
	virtual _int Update_Colliders(_matrix wolrdMatrix = XMMatrixIdentity());

public:	
	static CLegion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END