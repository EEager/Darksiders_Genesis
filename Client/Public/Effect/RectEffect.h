#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_RectInstance;
END

BEGIN(Client)

class CRectEffect final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };
private:
	explicit CRectEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRectEffect(const CRectEffect& rhs);
	virtual ~CRectEffect() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_RectInstance*		m_pModelCom = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable();	

public:	
	static CRectEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END