#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CBallista final : public CGameObject
{
	friend class CLegion;

private:
	// 바리스타 화살
	typedef struct tagBoltDesc
	{
		_float4x4		OffsetMatrix;
		_float4x4* pBoneMatrix;
		_float4x4		PivotMatrix;
		_float4x4* pTargetWorldMatrix;
	}SPEARDESC;
	SPEARDESC		m_spearDesc;

private:
	explicit CBallista(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBallista(const CBallista& rhs);
	virtual ~CBallista() = default;

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
	virtual void OnCollision_Enter(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Stay(CCollider* pSrc, CCollider* pDst, float fTimeDelta);
	virtual void OnCollision_Leave(CCollider* pSrc, CCollider* pDst, float fTimeDelta);

private:
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CModel* m_pModelDestroyedCom = nullptr;
	CModel* m_pModelGoblinCom = nullptr;

private:
	// FSM
	_bool m_bHitted = false;
	_float m_fHitPower = 0.f;
	_bool m_bWillDead = false;

	_bool forSoundBool1 = false;
	_bool forSoundBool2 = false;
	_float forSoundTimeAcc = 0.f;


	// Legion 이 탑승중인지 아닌지 판단하자
private:
	bool boltOnce = false;
	_bool m_bLegionOn = false;
	const char* m_pCurState = "Ballista_A.ao|Balliista_A_Idle";
	const char* m_pNextState = "Ballista_A.ao|Balliista_A_Idle";

public:	
	static CBallista* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};














// Ballsta_Bolt
class CBallista_Bolt final : public CGameObject
{
	friend class CBallista;

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
	explicit CBallista_Bolt(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBallista_Bolt(const CBallista& rhs);
	virtual ~CBallista_Bolt() = default;

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
	class CGameObject*	m_pOwner = nullptr;

	_float m_fLifeTime = 0.f;

public:
	static CBallista_Bolt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};


END