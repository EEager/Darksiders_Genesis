#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "ParticleSystem\ParticleSystem.h"


BEGIN(Engine)

class CCollider;
class CRenderer;
class CTransform;
class CVIBuffer_RectInstance;
class CVIBuffer_PointInstance;
END

BEGIN(Client)

class CFireEffect final : public CGameObject
{
public:
	enum TEXTURETYPE { TYPE_DIFFUSE, TYPE_FILTER, TYPE_BRUSH, TYPE_END };
private:
	explicit CFireEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CFireEffect(const CFireEffect& rhs);
	virtual ~CFireEffect() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);

private:	
	CRenderer*					m_pRendererCom = nullptr;
	CTransform*					m_pTransformCom = nullptr;
	CTexture*					m_pTextureDiffuse = nullptr;
	CTexture*					m_pTextureNoise = nullptr;
	CTexture*					m_pTextureAlpha = nullptr;
	CVIBuffer_RectInstance*		m_pModelCom_RectInstance = nullptr;
	CVIBuffer_Rect*				m_pModelCom_Rect = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(CVIBuffer* pVIBuffer);

	_float m_fMyTimeDelta = 0.f;

public:	
	static CFireEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END