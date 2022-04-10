#pragma once

#include "Monster\Monster.h"

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

private:
	const _float IDLE_TIME_TO_ATK_DELAY = 5.6f;
	const _float ATK_RANGE = 10.f;
	const _float INIT_RANGE = 50.f;

	_float m_fTimeIdle = 0.f;
	OBJECT_DIR m_eDir = OBJECT_DIR::DIR_F;
	//----------------------------------------------


public:	
	static CHollowLord* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END