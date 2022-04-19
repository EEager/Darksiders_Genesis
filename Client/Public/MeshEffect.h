#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "ParticleSystem\ParticleSystem.h"

BEGIN(Engine)
class CCollider;
class CRenderer;
class CTransform;
class CTexture;
class CModel;
END

BEGIN(Client)
typedef struct tagChaosEater
{
	_vector vPos;
	_vector vLook;
}CHAOSEATERDESC;
class CMeshEffect_ChaosEater final : public CGameObject
{


private:
	explicit CMeshEffect_ChaosEater(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMeshEffect_ChaosEater(const CMeshEffect_ChaosEater& rhs);
	virtual ~CMeshEffect_ChaosEater() = default;
public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	_float dissolvePower = 0.001f;

private:	
	ComPtr<CRenderer>			m_pRendererCom = nullptr;
	ComPtr<CTransform>			m_pTransformCom = nullptr;
	ComPtr<CModel>				m_pModelCom = nullptr;
	ComPtr<CTexture>			m_pDissolveTextureCom = nullptr;
	ComPtr<CParticleSystem>		m_pParticleSystem = nullptr;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIndex);	

public:	
	static CMeshEffect_ChaosEater* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END