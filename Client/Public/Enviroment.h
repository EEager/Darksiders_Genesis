#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CEnviroment final : public CGameObject
{



private:
	explicit CEnviroment(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEnviroment(const CEnviroment& rhs);
	virtual ~CEnviroment() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);


private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CCollider*					m_pAABBCom = nullptr; // 범위체크용

	_tchar m_szModelName[MAX_PATH] = { 0, };


public:
	void Set_CullingRadian(_float r) { m_fCullingRadian = r; }
private:
	_float m_fCullingRadian = -1.f;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIndex);

public:	
	static CEnviroment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END