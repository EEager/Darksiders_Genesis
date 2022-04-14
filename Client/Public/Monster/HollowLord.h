#pragma once

#include "Monster\Monster.h"
#include "UI_HollowLord_HpBar.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CNavigation;
class CModel;
END

BEGIN(Client)

class CHollowLord final : public CMonster
{
private:
	typedef struct tagBoneDesc
	{
		_float4x4	OffsetMatrix;
		_float4x4*	pBoneMatrix;
		_float4x4	PivotMatrix;
		_float4x4*	pTargetWorldMatrix;
	}BONEDESC;
	BONEDESC		m_Lord_RightHandDesc;
	BONEDESC		m_Lord_LeftHandDesc;

private:
	explicit CHollowLord(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHollowLord(const CHollowLord& rhs);
	virtual ~CHollowLord() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);


	//----------------------------------------------
	// FSM
protected:
	void UpdateState(); // m_eCurState Exit, m_eNextState Enter
	void DoState(float fTimeDelta); // m_eCurState Execute 
	virtual _int Update_Colliders(_matrix wolrdMatrix = XMMatrixIdentity());
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);



private:
	const _float IDLE_TIME_TO_ATK_DELAY = 5.6f;
	const _float ATK_RANGE = 10.f;
	const _float INIT_RANGE = 50.f;

	_float m_fTimeIdle = 0.f;
	OBJECT_DIR m_eDir = OBJECT_DIR::DIR_F;
	//----------------------------------------------

private:
	class CUI_HollowLord_HpBar* m_pHpBar = nullptr;


public:	
	static CHollowLord* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END