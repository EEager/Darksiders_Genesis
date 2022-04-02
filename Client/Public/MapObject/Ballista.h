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
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;

	// Legion 이 탑승중인지 아닌지 판단하자
private:
	_bool m_bLegionOn = false;
	const char* m_pCurState = "Ballista_A.ao|Balliista_A_Idle";
	const char* m_pNextState = "Ballista_A.ao|Balliista_A_Idle";

public:	
	static CBallista* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END