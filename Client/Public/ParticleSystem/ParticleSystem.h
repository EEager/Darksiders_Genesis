#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

#include "Component.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CTexture;
END

BEGIN(Client)

class CParticleSystem final : public CGameObject
{
private:
	explicit CParticleSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CParticleSystem(const CParticleSystem& rhs);
	virtual ~CParticleSystem() = default;
public:
	virtual HRESULT NativeConstruct_Prototype(const _tchar* pShaderFilePath);
	virtual HRESULT NativeConstruct(void* pArg);
	virtual _int Tick(_float fTimeDelta);
	virtual _int LateTick(_float fTimeDelta);
	virtual HRESULT Render(_uint iPassIndex = 0);
	virtual HRESULT PostRender(unique_ptr<SpriteBatch>& m_spriteBatch, unique_ptr<SpriteFont>& m_spriteFont);

private:
	void CParticleSystem::BuildVB(ID3D11Device* pDevice);
	HRESULT CParticleSystem::Set_RawValue(const char* pConstantName, void* pData, _uint iSize);

private:	
	ComPtr<CRenderer>			m_pRendererCom = nullptr;
	ComPtr<CTransform>			m_pTransformCom = nullptr;
	ComPtr<CTexture>			m_pTextureCom = nullptr;

	ID3D11InputLayout*			m_ParticleLayout = 0;
	ID3DX11EffectTechnique*		m_StreamOutTech = nullptr;
	ID3DX11EffectTechnique*		m_DrawTech = nullptr;

	ID3D11Buffer*				mInitVB;
	ID3D11Buffer*				mDrawVB;
	ID3D11Buffer*				mStreamOutVB;

	ComPtr<ID3DX11Effect>		m_pEffect = nullptr;
	vector<PASSDESC*>			m_PassesDesc;

	UINT mMaxParticles;
	bool mFirstRun;
	float mTimeStep = 0.f;
	float mAge;

	_float3 mEmitPosW;
	_float3 mEmitDirW;

private:
	HRESULT SetUp_Component();
	HRESULT SetUp_ConstantTable(_uint iPassIndex);	

public:	
	static CParticleSystem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const _tchar* pShaderFilePath);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END